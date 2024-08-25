#include "khg_phy/spatial_index.h"
#include "khg_utl/error_func.h"
#include "khg_phy/phy_private.h"
#include <stdlib.h>

static inline phy_spatial_index_class *Klass(void);

typedef struct Node Node;
typedef struct Pair Pair;

struct phy_BB_tree {
	phy_spatial_index spatialIndex;
	phy_BB_tree_velocity_func velocityFunc;
	
	phy_hash_set *leaves;
	Node *root;
	
	Node *pooledNodes;
	Pair *pooledPairs;
	phy_array *allocatedBuffers;
	
	phy_timestamp stamp;
};

struct Node {
	void *obj;
	phy_bb bb;
	Node *parent;
	
	union {
		// Internal nodes
		struct { Node *a, *b; } children;
		
		// Leaves
		struct {
			phy_timestamp stamp;
			Pair *pairs;
		} leaf;
	} node;
};

// Can't use anonymous unions and still get good x-compiler compatability
#define A node.children.a
#define B node.children.b
#define STAMP node.leaf.stamp
#define PAIRS node.leaf.pairs

typedef struct Thread {
	Pair *prev;
	Node *leaf;
	Pair *next;
} Thread;

struct Pair {
	Thread a, b;
	phy_collision_id id;
};

//MARK: Misc Functions

static inline phy_bb
GetBB(phy_BB_tree *tree, void *obj)
{
	phy_bb bb = tree->spatialIndex.bbfunc(obj);
	
	phy_BB_tree_velocity_func velocityFunc = tree->velocityFunc;
	if(velocityFunc){
		float coef = 0.1f;
		float x = (bb.r - bb.l)*coef;
		float y = (bb.t - bb.b)*coef;
		
		phy_vect v = phy_v_mult(velocityFunc(obj), 0.1f);
		return phy_bb_new(bb.l + phy_min(-x, v.x), bb.b + phy_min(-y, v.y), bb.r + phy_max(x, v.x), bb.t + phy_max(y, v.y));
	} else {
		return bb;
	}
}

static inline phy_BB_tree *
GetTree(phy_spatial_index *index)
{
	return (index && index->class == Klass() ? (phy_BB_tree *)index : NULL);
}

static inline Node *
GetRootIfTree(phy_spatial_index *index){
	return (index && index->class == Klass() ? ((phy_BB_tree *)index)->root : NULL);
}

static inline phy_BB_tree *
GetMasterTree(phy_BB_tree *tree)
{
	phy_BB_tree *dynamicTree = GetTree(tree->spatialIndex.dynamic_index);
	return (dynamicTree ? dynamicTree : tree);
}

static inline void
IncrementStamp(phy_BB_tree *tree)
{
	phy_BB_tree *dynamicTree = GetTree(tree->spatialIndex.dynamic_index);
	if(dynamicTree){
		dynamicTree->stamp++;
	} else {
		tree->stamp++;
	}
}

//MARK: Pair/Thread Functions

static void
PairRecycle(phy_BB_tree *tree, Pair *pair)
{
	// Share the pool of the master tree.
	// TODO: would be lovely to move the pairs stuff into an external data structure.
	tree = GetMasterTree(tree);
	
	pair->a.next = tree->pooledPairs;
	tree->pooledPairs = pair;
}

static Pair *
PairFromPool(phy_BB_tree *tree)
{
	// Share the pool of the master tree.
	// TODO: would be lovely to move the pairs stuff into an external data structure.
	tree = GetMasterTree(tree);
	
	Pair *pair = tree->pooledPairs;
	
	if(pair){
		tree->pooledPairs = pair->a.next;
		return pair;
	} else {
		// Pool is exhausted, make more
		int count = PHY_BUFFER_BYTES/sizeof(Pair);
    if (!count) {
		  utl_error_func("Buffer size is too small", utl_user_defined_data);
    }
		
		Pair *buffer = (Pair *)calloc(1, PHY_BUFFER_BYTES);
		phy_array_push(tree->allocatedBuffers, buffer);
		
		// push all but the first one, return the first instead
		for(int i=1; i<count; i++) PairRecycle(tree, buffer + i);
		return buffer;
	}
}

static inline void
ThreadUnlink(Thread thread)
{
	Pair *next = thread.next;
	Pair *prev = thread.prev;
	
	if(next){
		if(next->a.leaf == thread.leaf) next->a.prev = prev; else next->b.prev = prev;
	}
	
	if(prev){
		if(prev->a.leaf == thread.leaf) prev->a.next = next; else prev->b.next = next;
	} else {
		thread.leaf->PAIRS = next;
	}
}

static void
PairsClear(Node *leaf, phy_BB_tree *tree)
{
	Pair *pair = leaf->PAIRS;
	leaf->PAIRS = NULL;
	
	while(pair){
		if(pair->a.leaf == leaf){
			Pair *next = pair->a.next;
			ThreadUnlink(pair->b);
			PairRecycle(tree, pair);
			pair = next;
		} else {
			Pair *next = pair->b.next;
			ThreadUnlink(pair->a);
			PairRecycle(tree, pair);
			pair = next;
		}
	}
}

static void
PairInsert(Node *a, Node *b, phy_BB_tree *tree)
{
	Pair *nextA = a->PAIRS, *nextB = b->PAIRS;
	Pair *pair = PairFromPool(tree);
	Pair temp = {{NULL, a, nextA},{NULL, b, nextB}, 0};
	
	a->PAIRS = b->PAIRS = pair;
	*pair = temp;
	
	if(nextA){
		if(nextA->a.leaf == a) nextA->a.prev = pair; else nextA->b.prev = pair;
	}
	
	if(nextB){
		if(nextB->a.leaf == b) nextB->a.prev = pair; else nextB->b.prev = pair;
	}
}


//MARK: Node Functions

static void
NodeRecycle(phy_BB_tree *tree, Node *node)
{
	node->parent = tree->pooledNodes;
	tree->pooledNodes = node;
}

static Node *
NodeFromPool(phy_BB_tree *tree)
{
	Node *node = tree->pooledNodes;
	
	if(node){
		tree->pooledNodes = node->parent;
		return node;
	} else {
		// Pool is exhausted, make more
		int count = PHY_BUFFER_BYTES/sizeof(Node);
    if (!count) {
		  utl_error_func("Buffer size is too small", utl_user_defined_data);
    }
		
		Node *buffer = (Node *)calloc(1, PHY_BUFFER_BYTES);
		phy_array_push(tree->allocatedBuffers, buffer);
		
		// push all but the first one, return the first instead
		for(int i=1; i<count; i++) NodeRecycle(tree, buffer + i);
		return buffer;
	}
}

static inline void
NodeSetA(Node *node, Node *value)
{
	node->A = value;
	value->parent = node;
}

static inline void
NodeSetB(Node *node, Node *value)
{
	node->B = value;
	value->parent = node;
}

static Node *
NodeNew(phy_BB_tree *tree, Node *a, Node *b)
{
	Node *node = NodeFromPool(tree);
	
	node->obj = NULL;
	node->bb = phy_bb_merge(a->bb, b->bb);
	node->parent = NULL;
	
	NodeSetA(node, a);
	NodeSetB(node, b);
	
	return node;
}

static inline bool
NodeIsLeaf(Node *node)
{
	return (node->obj != NULL);
}

static inline Node *
NodeOther(Node *node, Node *child)
{
	return (node->A == child ? node->B : node->A);
}

static inline void
NodeReplaceChild(Node *parent, Node *child, Node *value, phy_BB_tree *tree)
{
  if (NodeIsLeaf(parent)) {
	  utl_error_func("Cannot replace child of a leaf", utl_user_defined_data);
  }
  if (!(child == parent->A || child == parent->B)) {
	  utl_error_func("Node is not a child of parent.", utl_user_defined_data);
  }
	if(parent->A == child){
		NodeRecycle(tree, parent->A);
		NodeSetA(parent, value);
	} else {
		NodeRecycle(tree, parent->B);
		NodeSetB(parent, value);
	}
	
	for(Node *node=parent; node; node = node->parent){
		node->bb = phy_bb_merge(node->A->bb, node->B->bb);
	}
}

//MARK: Subtree Functions

static inline float
cpBBProximity(phy_bb a, phy_bb b)
{
	return phy_abs(a.l + a.r - b.l - b.r) + phy_abs(a.b + a.t - b.b - b.t);
}

static Node *
SubtreeInsert(Node *subtree, Node *leaf, phy_BB_tree *tree)
{
	if(subtree == NULL){
		return leaf;
	} else if(NodeIsLeaf(subtree)){
		return NodeNew(tree, leaf, subtree);
	} else {
		float cost_a = phy_bb_area(subtree->B->bb) + phy_bb_merged_area(subtree->A->bb, leaf->bb);
		float cost_b = phy_bb_area(subtree->A->bb) + phy_bb_merged_area(subtree->B->bb, leaf->bb);
		
		if(cost_a == cost_b){
			cost_a = cpBBProximity(subtree->A->bb, leaf->bb);
			cost_b = cpBBProximity(subtree->B->bb, leaf->bb);
		}
		
		if(cost_b < cost_a){
			NodeSetB(subtree, SubtreeInsert(subtree->B, leaf, tree));
		} else {
			NodeSetA(subtree, SubtreeInsert(subtree->A, leaf, tree));
		}
		
		subtree->bb = phy_bb_merge(subtree->bb, leaf->bb);
		return subtree;
	}
}

static void
SubtreeQuery(Node *subtree, void *obj, phy_bb bb, phy_spatial_index_query_func func, void *data)
{
	if(phy_bb_intersects(subtree->bb, bb)){
		if(NodeIsLeaf(subtree)){
			func(obj, subtree->obj, 0, data);
		} else {
			SubtreeQuery(subtree->A, obj, bb, func, data);
			SubtreeQuery(subtree->B, obj, bb, func, data);
		}
	}
}


static float
SubtreeSegmentQuery(Node *subtree, void *obj, phy_vect a, phy_vect b, float t_exit, phy_spatial_index_segment_query_func func, void *data)
{
	if(NodeIsLeaf(subtree)){
		return func(obj, subtree->obj, data);
	} else {
		float t_a = phy_bb_segment_query(subtree->A->bb, a, b);
		float t_b = phy_bb_segment_query(subtree->B->bb, a, b);
		
		if(t_a < t_b){
			if(t_a < t_exit) t_exit = phy_min(t_exit, SubtreeSegmentQuery(subtree->A, obj, a, b, t_exit, func, data));
			if(t_b < t_exit) t_exit = phy_min(t_exit, SubtreeSegmentQuery(subtree->B, obj, a, b, t_exit, func, data));
		} else {
			if(t_b < t_exit) t_exit = phy_min(t_exit, SubtreeSegmentQuery(subtree->B, obj, a, b, t_exit, func, data));
			if(t_a < t_exit) t_exit = phy_min(t_exit, SubtreeSegmentQuery(subtree->A, obj, a, b, t_exit, func, data));
		}
		
		return t_exit;
	}
}

static void
SubtreeRecycle(phy_BB_tree *tree, Node *node)
{
	if(!NodeIsLeaf(node)){
		SubtreeRecycle(tree, node->A);
		SubtreeRecycle(tree, node->B);
		NodeRecycle(tree, node);
	}
}

static inline Node *
SubtreeRemove(Node *subtree, Node *leaf, phy_BB_tree *tree)
{
	if(leaf == subtree){
		return NULL;
	} else {
		Node *parent = leaf->parent;
		if(parent == subtree){
			Node *other = NodeOther(subtree, leaf);
			other->parent = subtree->parent;
			NodeRecycle(tree, subtree);
			return other;
		} else {
			NodeReplaceChild(parent->parent, parent, NodeOther(parent, leaf), tree);
			return subtree;
		}
	}
}

//MARK: Marking Functions

typedef struct MarkContext {
	phy_BB_tree *tree;
	Node *staticRoot;
	phy_spatial_index_query_func func;
	void *data;
} MarkContext;

static void
MarkLeafQuery(Node *subtree, Node *leaf, bool left, MarkContext *context)
{
	if(phy_bb_intersects(leaf->bb, subtree->bb)){
		if(NodeIsLeaf(subtree)){
			if(left){
				PairInsert(leaf, subtree, context->tree);
			} else {
				if(subtree->STAMP < leaf->STAMP) PairInsert(subtree, leaf, context->tree);
				context->func(leaf->obj, subtree->obj, 0, context->data);
			}
		} else {
			MarkLeafQuery(subtree->A, leaf, left, context);
			MarkLeafQuery(subtree->B, leaf, left, context);
		}
	}
}

static void
MarkLeaf(Node *leaf, MarkContext *context)
{
	phy_BB_tree *tree = context->tree;
	if(leaf->STAMP == GetMasterTree(tree)->stamp){
		Node *staticRoot = context->staticRoot;
		if(staticRoot) MarkLeafQuery(staticRoot, leaf, false, context);
		
		for(Node *node = leaf; node->parent; node = node->parent){
			if(node == node->parent->A){
				MarkLeafQuery(node->parent->B, leaf, true, context);
			} else {
				MarkLeafQuery(node->parent->A, leaf, false, context);
			}
		}
	} else {
		Pair *pair = leaf->PAIRS;
		while(pair){
			if(leaf == pair->b.leaf){
				pair->id = context->func(pair->a.leaf->obj, leaf->obj, pair->id, context->data);
				pair = pair->b.next;
			} else {
				pair = pair->a.next;
			}
		}
	}
}

static void
MarkSubtree(Node *subtree, MarkContext *context)
{
	if(NodeIsLeaf(subtree)){
		MarkLeaf(subtree, context);
	} else {
		MarkSubtree(subtree->A, context);
		MarkSubtree(subtree->B, context); // TODO: Force TCO here?
	}
}

//MARK: Leaf Functions

static Node *
LeafNew(phy_BB_tree *tree, void *obj, phy_bb bb)
{
	Node *node = NodeFromPool(tree);
	node->obj = obj;
	node->bb = GetBB(tree, obj);
	
	node->parent = NULL;
	node->STAMP = 0;
	node->PAIRS = NULL;
	
	return node;
}

static bool
LeafUpdate(Node *leaf, phy_BB_tree *tree)
{
	Node *root = tree->root;
	phy_bb bb = tree->spatialIndex.bbfunc(leaf->obj);
	
	if(!phy_bb_contains_bb(leaf->bb, bb)){
		leaf->bb = GetBB(tree, leaf->obj);
		
		root = SubtreeRemove(root, leaf, tree);
		tree->root = SubtreeInsert(root, leaf, tree);
		
		PairsClear(leaf, tree);
		leaf->STAMP = GetMasterTree(tree)->stamp;
		
		return true;
	} else {
		return false;
	}
}

static phy_collision_id VoidQueryFunc(void *obj1, void *obj2, phy_collision_id id, void *data){return id;}

static void
LeafAddPairs(Node *leaf, phy_BB_tree *tree)
{
	phy_spatial_index *dynamicIndex = tree->spatialIndex.dynamic_index;
	if(dynamicIndex){
		Node *dynamicRoot = GetRootIfTree(dynamicIndex);
		if(dynamicRoot){
			phy_BB_tree *dynamicTree = GetTree(dynamicIndex);
			MarkContext context = {dynamicTree, NULL, NULL, NULL};
			MarkLeafQuery(dynamicRoot, leaf, true, &context);
		}
	} else {
		Node *staticRoot = GetRootIfTree(tree->spatialIndex.static_index);
		MarkContext context = {tree, staticRoot, VoidQueryFunc, NULL};
		MarkLeaf(leaf, &context);
	}
}

//MARK: Memory Management Functions

phy_BB_tree *
phy_BB_tree_alloc(void)
{
	return (phy_BB_tree *)calloc(1, sizeof(phy_BB_tree));
}

static int
leafSetEql(void *obj, Node *node)
{
	return (obj == node->obj);
}

static void *
leafSetTrans(void *obj, phy_BB_tree *tree)
{
	return LeafNew(tree, obj, tree->spatialIndex.bbfunc(obj));
}

phy_spatial_index *
phy_BB_tree_init(phy_BB_tree *tree, phy_spatial_index_BB_func bbfunc, phy_spatial_index *staticIndex)
{
	phy_spatial_index_init((phy_spatial_index *)tree, Klass(), bbfunc, staticIndex);
	
	tree->velocityFunc = NULL;
	
	tree->leaves = cp_hash_set_new(0, (phy_hash_set_eql_func)leafSetEql);
	tree->root = NULL;
	
	tree->pooledNodes = NULL;
	tree->allocatedBuffers = phy_array_new(0);
	
	tree->stamp = 0;
	
	return (phy_spatial_index *)tree;
}

void
phy_BB_tree_set_velocity_func(phy_spatial_index *index, phy_BB_tree_velocity_func func)
{
	if(index->class != Klass()){
		utl_error_func("Ignoring velocity set call to non-tree spatial index", utl_user_defined_data);
		// return;
	}
	((phy_BB_tree *)index)->velocityFunc = func;
}

phy_spatial_index *
phy_BB_tree_new(phy_spatial_index_BB_func bbfunc, phy_spatial_index *staticIndex)
{
	return phy_BB_tree_init(phy_BB_tree_alloc(), bbfunc, staticIndex);
}

static void
cpBBTreeDestroy(phy_BB_tree *tree)
{
	phy_hash_set_free(tree->leaves);
	
	if(tree->allocatedBuffers) phy_array_free_each(tree->allocatedBuffers, free);
	phy_array_free(tree->allocatedBuffers);
}

//MARK: Insert/Remove

static void
cpBBTreeInsert(phy_BB_tree *tree, void *obj, phy_hash_value hashid)
{
	Node *leaf = (Node *)phy_hash_set_insert(tree->leaves, hashid, obj, (phy_hash_set_trans_func)leafSetTrans, tree);
	
	Node *root = tree->root;
	tree->root = SubtreeInsert(root, leaf, tree);
	
	leaf->STAMP = GetMasterTree(tree)->stamp;
	LeafAddPairs(leaf, tree);
	IncrementStamp(tree);
}

static void
cpBBTreeRemove(phy_BB_tree *tree, void *obj, phy_hash_value hashid)
{
	Node *leaf = (Node *)phy_hash_set_remove(tree->leaves, hashid, obj);
	
	tree->root = SubtreeRemove(tree->root, leaf, tree);
	PairsClear(leaf, tree);
	NodeRecycle(tree, leaf);
}

static bool
cpBBTreeContains(phy_BB_tree *tree, void *obj, phy_hash_value hashid)
{
	return (phy_hash_set_find(tree->leaves, hashid, obj) != NULL);
}

//MARK: Reindex

static void LeafUpdateWrap(Node *leaf, phy_BB_tree *tree) {LeafUpdate(leaf, tree);}

static void
cpBBTreeReindexQuery(phy_BB_tree *tree, phy_spatial_index_query_func func, void *data)
{
	if(!tree->root) return;
	
	// LeafUpdate() may modify tree->root. Don't cache it.
	phy_hash_set_each(tree->leaves, (phy_hash_set_iterator_func)LeafUpdateWrap, tree);
	
	phy_spatial_index *staticIndex = tree->spatialIndex.static_index;
	Node *staticRoot = (staticIndex && staticIndex->class == Klass() ? ((phy_BB_tree *)staticIndex)->root : NULL);
	
	MarkContext context = {tree, staticRoot, func, data};
	MarkSubtree(tree->root, &context);
	if(staticIndex && !staticRoot) phy_spatial_index_collide_static((phy_spatial_index *)tree, staticIndex, func, data);
	
	IncrementStamp(tree);
}

static void
cpBBTreeReindex(phy_BB_tree *tree)
{
	cpBBTreeReindexQuery(tree, VoidQueryFunc, NULL);
}

static void
cpBBTreeReindexObject(phy_BB_tree *tree, void *obj, phy_hash_value hashid)
{
	Node *leaf = (Node *)phy_hash_set_find(tree->leaves, hashid, obj);
	if(leaf){
		if(LeafUpdate(leaf, tree)) LeafAddPairs(leaf, tree);
		IncrementStamp(tree);
	}
}

//MARK: Query

static void
cpBBTreeSegmentQuery(phy_BB_tree *tree, void *obj, phy_vect a, phy_vect b, float t_exit, phy_spatial_index_segment_query_func func, void *data)
{
	Node *root = tree->root;
	if(root) SubtreeSegmentQuery(root, obj, a, b, t_exit, func, data);
}

static void
cpBBTreeQuery(phy_BB_tree *tree, void *obj, phy_bb bb, phy_spatial_index_query_func func, void *data)
{
	if(tree->root) SubtreeQuery(tree->root, obj, bb, func, data);
}

//MARK: Misc

static int
cpBBTreeCount(phy_BB_tree *tree)
{
	return phy_hash_set_count(tree->leaves);
}

typedef struct eachContext {
	phy_spatial_index_iterator_func func;
	void *data;
} eachContext;

static void each_helper(Node *node, eachContext *context){context->func(node->obj, context->data);}

static void
cpBBTreeEach(phy_BB_tree *tree, phy_spatial_index_iterator_func func, void *data)
{
	eachContext context = {func, data};
	phy_hash_set_each(tree->leaves, (phy_hash_set_iterator_func)each_helper, &context);
}

static phy_spatial_index_class klass = {
	(phy_spatial_index_destroy_impl)cpBBTreeDestroy,
	
	(phy_spatial_index_count_impl)cpBBTreeCount,
	(phy_spatial_index_each_impl)cpBBTreeEach,
	
	(phy_spatial_index_contains_impl)cpBBTreeContains,
	(phy_spatial_index_insert_impl)cpBBTreeInsert,
	(phy_spatial_index_remove_impl)cpBBTreeRemove,
	
	(phy_spatial_index_reindex_impl)cpBBTreeReindex,
	(phy_spatial_index_reindex_object_impl)cpBBTreeReindexObject,
	(phy_spatial_index_reindex_query_impl)cpBBTreeReindexQuery,
	
	(phy_spatial_index_query_impl)cpBBTreeQuery,
	(phy_spatial_index_segment_query_impl)cpBBTreeSegmentQuery,
};

static inline phy_spatial_index_class *Klass(){return &klass;}


//MARK: Tree Optimization

static int
cpfcompare(const float *a, const float *b){
	return (*a < *b ? -1 : (*b < *a ? 1 : 0));
}

static void
fillNodeArray(Node *node, Node ***cursor){
	(**cursor) = node;
	(*cursor)++;
}

static Node *
partitionNodes(phy_BB_tree *tree, Node **nodes, int count)
{
	if(count == 1){
		return nodes[0];
	} else if(count == 2) {
		return NodeNew(tree, nodes[0], nodes[1]);
	}
	
	// Find the AABB for these nodes
	phy_bb bb = nodes[0]->bb;
	for(int i=1; i<count; i++) bb = phy_bb_merge(bb, nodes[i]->bb);
	
	// Split it on it's longest axis
	bool splitWidth = (bb.r - bb.l > bb.t - bb.b);
	
	// Sort the bounds and use the median as the splitting point
	float *bounds = (float *)calloc(count*2, sizeof(float));
	if(splitWidth){
		for(int i=0; i<count; i++){
			bounds[2*i + 0] = nodes[i]->bb.l;
			bounds[2*i + 1] = nodes[i]->bb.r;
		}
	} else {
		for(int i=0; i<count; i++){
			bounds[2*i + 0] = nodes[i]->bb.b;
			bounds[2*i + 1] = nodes[i]->bb.t;
		}
	}
	
	qsort(bounds, count*2, sizeof(float), (int (*)(const void *, const void *))cpfcompare);
	float split = (bounds[count - 1] + bounds[count])*0.5f; // use the medain as the split
	free(bounds);

	// Generate the child BBs
	phy_bb a = bb, b = bb;
	if(splitWidth) a.r = b.l = split; else a.t = b.b = split;
	
	// Partition the nodes
	int right = count;
	for(int left=0; left < right;){
		Node *node = nodes[left];
		if(phy_bb_merged_area(node->bb, b) < phy_bb_merged_area(node->bb, a)){
//		if(cpBBProximity(node->bb, b) < cpBBProximity(node->bb, a)){
			right--;
			nodes[left] = nodes[right];
			nodes[right] = node;
		} else {
			left++;
		}
	}
	
	if(right == count){
		Node *node = NULL;
		for(int i=0; i<count; i++) node = SubtreeInsert(node, nodes[i], tree);
		return node;
	}
	
	// Recurse and build the node!
	return NodeNew(tree,
		partitionNodes(tree, nodes, right),
		partitionNodes(tree, nodes + right, count - right)
	);
}

//static void
//cpBBTreeOptimizeIncremental(cpBBTree *tree, int passes)
//{
//	for(int i=0; i<passes; i++){
//		Node *root = tree->root;
//		Node *node = root;
//		int bit = 0;
//		unsigned int path = tree->opath;
//		
//		while(!NodeIsLeaf(node)){
//			node = (path&(1<<bit) ? node->a : node->b);
//			bit = (bit + 1)&(sizeof(unsigned int)*8 - 1);
//		}
//		
//		root = subtreeRemove(root, node, tree);
//		tree->root = subtreeInsert(root, node, tree);
//	}
//}

void
phy_BB_tree_optimize(phy_spatial_index *index)
{
	if(index->class != &klass){
		utl_error_func("Ignoring optimize call to non-tree spatial index", utl_user_defined_data);
		return;
	}
	
	phy_BB_tree *tree = (phy_BB_tree *)index;
	Node *root = tree->root;
	if(!root) return;
	
	int count = cpBBTreeCount(tree);
	Node **nodes = (Node **)calloc(count, sizeof(Node *));
	Node **cursor = nodes;
	
	phy_hash_set_each(tree->leaves, (phy_hash_set_iterator_func)fillNodeArray, &cursor);
	
	SubtreeRecycle(tree, root);
	tree->root = partitionNodes(tree, nodes, count);
	free(nodes);
}

//MARK: Debug Draw

//#define CP_BBTREE_DEBUG_DRAW
#ifdef CP_BBTREE_DEBUG_DRAW
#include "OpenGL/gl.h"
#include "OpenGL/glu.h"
#include <GLUT/glut.h>

static void
NodeRender(Node *node, int depth)
{
	if(!NodeIsLeaf(node) && depth <= 10){
		NodeRender(node->a, depth + 1);
		NodeRender(node->b, depth + 1);
	}
	
	cpBB bb = node->bb;
	
//	GLfloat v = depth/2.0f;	
//	glColor3f(1.0f - v, v, 0.0f);
	glLineWidth(cpfmax(5.0f - depth, 1.0f));
	glBegin(GL_LINES); {
		glVertex2f(bb.l, bb.b);
		glVertex2f(bb.l, bb.t);
		
		glVertex2f(bb.l, bb.t);
		glVertex2f(bb.r, bb.t);
		
		glVertex2f(bb.r, bb.t);
		glVertex2f(bb.r, bb.b);
		
		glVertex2f(bb.r, bb.b);
		glVertex2f(bb.l, bb.b);
	}; glEnd();
}

void
cpBBTreeRenderDebug(cpSpatialIndex *index){
	if(index->klass != &klass){
		cpAssertWarn(cpFalse, "Ignoring cpBBTreeRenderDebug() call to non-tree spatial index.");
		return;
	}
	
	cpBBTree *tree = (cpBBTree *)index;
	if(tree->root) NodeRender(tree->root, 0);
}
#endif
