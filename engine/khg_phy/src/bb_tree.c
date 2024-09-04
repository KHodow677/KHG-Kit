#include "khg_phy/spatial_index.h"
#include "khg_utl/error_func.h"
#include "khg_phy/phy_private.h"
#include <stdlib.h>

static inline phy_spatial_index_class *phy_class(void);
static phy_collision_id phy_void_query_func(void *obj1, void *obj2, phy_collision_id id, void *data) { return id; }

typedef struct phy_node phy_node;
typedef struct phy_pair phy_pair;

struct phy_bb_tree {
	phy_spatial_index spatial_index;
	phy_bb_tree_velocity_func velocity_func;
	phy_hash_set *leaves;
	phy_node *root;
	phy_node *pooled_nodes;
	phy_pair *pooled_pairs;
	phy_array *allocated_buffers;
	phy_timestamp stamp;
};

struct phy_node {
	void *obj;
	phy_bb bb;
	phy_node *parent;
	union {
		struct { 
      phy_node *a, *b;
    } children;
		struct {
			phy_timestamp stamp;
			phy_pair *pairs;
		} leaf;
	} node;
};

#define A node.children.a
#define B node.children.b
#define STAMP node.leaf.stamp
#define PAIRS node.leaf.pairs

typedef struct phy_thread {
	phy_pair *prev;
	phy_node *leaf;
	phy_pair *next;
} phy_thread;

struct phy_pair {
	phy_thread a, b;
	phy_collision_id id;
};


typedef struct phy_mark_context {
	phy_bb_tree *tree;
	phy_node *staticRoot;
	phy_spatial_index_query_func func;
	void *data;
} phy_mark_context;

typedef struct phy_each_context {
	phy_spatial_index_iterator_func func;
	void *data;
} phy_each_context;

static inline phy_bb phy_get_bb(phy_bb_tree *tree, void *obj) {
	phy_bb bb = tree->spatial_index.bbfunc(obj);
	phy_bb_tree_velocity_func velocityFunc = tree->velocity_func;
  if (velocityFunc) {
		float coef = 0.1f;
		float x = (bb.r - bb.l)*coef;
		float y = (bb.t - bb.b)*coef;
		phy_vect v = phy_v_mult(velocityFunc(obj), 0.1f);
		return phy_bb_new(bb.l + phy_min(-x, v.x), bb.b + phy_min(-y, v.y), bb.r + phy_max(x, v.x), bb.t + phy_max(y, v.y));
	} 
  else {
		return bb;
	}
}

static inline phy_bb_tree *phy_get_tree(phy_spatial_index *index) {
	return (index && index->class == phy_class() ? (phy_bb_tree *)index : NULL);
}

static inline phy_node *get_root_if_tree(phy_spatial_index *index){
	return (index && index->class == phy_class() ? ((phy_bb_tree *)index)->root : NULL);
}

static inline phy_bb_tree *get_master_tree(phy_bb_tree *tree) {
	phy_bb_tree *dynamicTree = phy_get_tree(tree->spatial_index.dynamic_index);
	return (dynamicTree ? dynamicTree : tree);
}

static inline void phy_increment_stamp(phy_bb_tree *tree) {
	phy_bb_tree *dynamicTree = phy_get_tree(tree->spatial_index.dynamic_index);
  if (dynamicTree) {
		dynamicTree->stamp++;
	} 
  else {
		tree->stamp++;
	}
}

static void phy_pair_recycle(phy_bb_tree *tree, phy_pair *pair) {
	tree = get_master_tree(tree);
	pair->a.next = tree->pooled_pairs;
	tree->pooled_pairs = pair;
}

static phy_pair *phy_pair_from_pool(phy_bb_tree *tree) {
	tree = get_master_tree(tree);
	phy_pair *pair = tree->pooled_pairs;
  if (pair) {
		tree->pooled_pairs = pair->a.next;
		return pair;
	} 
  else {
		int count = PHY_BUFFER_BYTES/sizeof(phy_pair);
    if (!count) {
		  utl_error_func("Buffer size is too small", utl_user_defined_data);
    }
		phy_pair *buffer = (phy_pair *)calloc(1, PHY_BUFFER_BYTES);
		phy_array_push(tree->allocated_buffers, buffer);
		for (int i=1; i<count; i++) {
      phy_pair_recycle(tree, buffer + i);
    }
		return buffer;
	}
}

static inline void phy_thread_unlink(phy_thread thread) {
	phy_pair *next = thread.next;
	phy_pair *prev = thread.prev;
	if (next){
    if (next->a.leaf == thread.leaf) {
      next->a.prev = prev; 
    }
    else {
      next->b.prev = prev;
    }
	}
  if (prev) {
    if (prev->a.leaf == thread.leaf) {
      prev->a.next = next; 
    } 
    else {
      prev->b.next = next;
    }
	} 
  else {
		thread.leaf->PAIRS = next;
	}
}

static void phy_pairs_clear(phy_node *leaf, phy_bb_tree *tree) {
	phy_pair *pair = leaf->PAIRS;
	leaf->PAIRS = NULL;
	while (pair) {
    if (pair->a.leaf == leaf) {
			phy_pair *next = pair->a.next;
			phy_thread_unlink(pair->b);
			phy_pair_recycle(tree, pair);
			pair = next;
		} 
    else {
			phy_pair *next = pair->b.next;
			phy_thread_unlink(pair->a);
			phy_pair_recycle(tree, pair);
			pair = next;
		}
	}
}

static void phy_pair_insert(phy_node *a, phy_node *b, phy_bb_tree *tree) {
	phy_pair *nextA = a->PAIRS, *nextB = b->PAIRS;
	phy_pair *pair = phy_pair_from_pool(tree);
	phy_pair temp = { { NULL, a, nextA },{ NULL, b, nextB }, 0 };
	a->PAIRS = b->PAIRS = pair;
	*pair = temp;
	if (nextA) {
    if (nextA->a.leaf == a) {
      nextA->a.prev = pair; 
    }
    else {
      nextA->b.prev = pair;
    }
	}
	if (nextB) {
    if (nextB->a.leaf == b) {
      nextB->a.prev = pair; 
    }
    else {
      nextB->b.prev = pair;
    }
	}
}

static void phy_node_recycle(phy_bb_tree *tree, phy_node *node) {
	node->parent = tree->pooled_nodes;
	tree->pooled_nodes = node;
}

static phy_node *phy_node_from_pool(phy_bb_tree *tree) {
	phy_node *node = tree->pooled_nodes;
  if (node) {
		tree->pooled_nodes = node->parent;
		return node;
	} 
  else {
		int count = PHY_BUFFER_BYTES/sizeof(phy_node);
    if (!count) {
		  utl_error_func("Buffer size is too small", utl_user_defined_data);
    }
		phy_node *buffer = (phy_node *)calloc(1, PHY_BUFFER_BYTES);
		phy_array_push(tree->allocated_buffers, buffer);
		for (int i=1; i<count; i++) {
      phy_node_recycle(tree, buffer + i);
    }
		return buffer;
	}
}

static inline void phy_node_set_A(phy_node *node, phy_node *value) {
	node->A = value;
	value->parent = node;
}

static inline void phy_node_set_B(phy_node *node, phy_node *value) {
	node->B = value;
	value->parent = node;
}

static phy_node *phy_node_new(phy_bb_tree *tree, phy_node *a, phy_node *b) {
	phy_node *node = phy_node_from_pool(tree);
	node->obj = NULL;
	node->bb = phy_bb_merge(a->bb, b->bb);
	node->parent = NULL;
	phy_node_set_A(node, a);
	phy_node_set_B(node, b);
	return node;
}

static inline bool phy_node_is_leaf(phy_node *node) {
	return (node->obj != NULL);
}

static inline phy_node *phy_node_other(phy_node *node, phy_node *child) {
	return (node->A == child ? node->B : node->A);
}

static inline void phy_node_replace_child(phy_node *parent, phy_node *child, phy_node *value, phy_bb_tree *tree) {
  if (phy_node_is_leaf(parent)) {
	  utl_error_func("Cannot replace child of a leaf", utl_user_defined_data);
  }
  if (!(child == parent->A || child == parent->B)) {
	  utl_error_func("Node is not a child of parent.", utl_user_defined_data);
  }
  if (parent->A == child){
		phy_node_recycle(tree, parent->A);
		phy_node_set_A(parent, value);
	} 
  else {
		phy_node_recycle(tree, parent->B);
		phy_node_set_B(parent, value);
	}
	for (phy_node *node=parent; node; node = node->parent){
		node->bb = phy_bb_merge(node->A->bb, node->B->bb);
	}
}

static inline float phy_bb_proximity(phy_bb a, phy_bb b) {
	return phy_abs(a.l + a.r - b.l - b.r) + phy_abs(a.b + a.t - b.b - b.t);
}

static phy_node *phy_subtree_insert(phy_node *subtree, phy_node *leaf, phy_bb_tree *tree) {
  if (subtree == NULL){
		return leaf;
	} 
  else if (phy_node_is_leaf(subtree)) {
		return phy_node_new(tree, leaf, subtree);
	} 
  else {
		float cost_a = phy_bb_area(subtree->B->bb) + phy_bb_merged_area(subtree->A->bb, leaf->bb);
		float cost_b = phy_bb_area(subtree->A->bb) + phy_bb_merged_area(subtree->B->bb, leaf->bb);
		if (cost_a == cost_b) {
			cost_a = phy_bb_proximity(subtree->A->bb, leaf->bb);
			cost_b = phy_bb_proximity(subtree->B->bb, leaf->bb);
		}
    if (cost_b < cost_a) {
			phy_node_set_B(subtree, phy_subtree_insert(subtree->B, leaf, tree));
		} 
    else {
			phy_node_set_A(subtree, phy_subtree_insert(subtree->A, leaf, tree));
		}
		subtree->bb = phy_bb_merge(subtree->bb, leaf->bb);
		return subtree;
	}
}

static void phy_subtree_query(phy_node *subtree, void *obj, phy_bb bb, phy_spatial_index_query_func func, void *data) {
	if (phy_bb_intersects(subtree->bb, bb)) {
    if (phy_node_is_leaf(subtree)) {
			func(obj, subtree->obj, 0, data);
		} 
    else {
			phy_subtree_query(subtree->A, obj, bb, func, data);
			phy_subtree_query(subtree->B, obj, bb, func, data);
		}
	}
}

static float phy_subtree_segment_query(phy_node *subtree, void *obj, phy_vect a, phy_vect b, float t_exit, phy_spatial_index_segment_query_func func, void *data) {
  if(phy_node_is_leaf(subtree)){
		return func(obj, subtree->obj, data);
	} 
  else {
		float t_a = phy_bb_segment_query(subtree->A->bb, a, b);
		float t_b = phy_bb_segment_query(subtree->B->bb, a, b);
    if(t_a < t_b){
			if(t_a < t_exit) t_exit = phy_min(t_exit, phy_subtree_segment_query(subtree->A, obj, a, b, t_exit, func, data));
			if(t_b < t_exit) t_exit = phy_min(t_exit, phy_subtree_segment_query(subtree->B, obj, a, b, t_exit, func, data));
		} 
    else {
			if(t_b < t_exit) t_exit = phy_min(t_exit, phy_subtree_segment_query(subtree->B, obj, a, b, t_exit, func, data));
			if(t_a < t_exit) t_exit = phy_min(t_exit, phy_subtree_segment_query(subtree->A, obj, a, b, t_exit, func, data));
		}
		return t_exit;
	}
}

static void phy_subtree_recycle(phy_bb_tree *tree, phy_node *node) {
	if (!phy_node_is_leaf(node)) {
		phy_subtree_recycle(tree, node->A);
		phy_subtree_recycle(tree, node->B);
		phy_node_recycle(tree, node);
	}
}

static inline phy_node *phy_subtree_remove(phy_node *subtree, phy_node *leaf, phy_bb_tree *tree) {
  if (leaf == subtree){
		return NULL;
	} 
  else {
		phy_node *parent = leaf->parent;
    if (parent == subtree) {
			phy_node *other = phy_node_other(subtree, leaf);
			other->parent = subtree->parent;
			phy_node_recycle(tree, subtree);
			return other;
		} 
    else {
			phy_node_replace_child(parent->parent, parent, phy_node_other(parent, leaf), tree);
			return subtree;
		}
	}
}

static void phy_mark_leaf_query(phy_node *subtree, phy_node *leaf, bool left, phy_mark_context *context) {
	if (phy_bb_intersects(leaf->bb, subtree->bb)) {
    if (phy_node_is_leaf(subtree)) {
      if (left) {
				phy_pair_insert(leaf, subtree, context->tree);
			} 
      else {
				if (subtree->STAMP < leaf->STAMP) {
          phy_pair_insert(subtree, leaf, context->tree);
        }
				context->func(leaf->obj, subtree->obj, 0, context->data);
			}
		} 
    else {
			phy_mark_leaf_query(subtree->A, leaf, left, context);
			phy_mark_leaf_query(subtree->B, leaf, left, context);
		}
	}
}

static void phy_mark_leaf(phy_node *leaf, phy_mark_context *context) {
	phy_bb_tree *tree = context->tree;
  if (leaf->STAMP == get_master_tree(tree)->stamp) {
		phy_node *staticRoot = context->staticRoot;
		if (staticRoot) {
      phy_mark_leaf_query(staticRoot, leaf, false, context);
    }
		for (phy_node *node = leaf; node->parent; node = node->parent) {
      if (node == node->parent->A) {
				phy_mark_leaf_query(node->parent->B, leaf, true, context);
			} 
      else {
				phy_mark_leaf_query(node->parent->A, leaf, false, context);
			}
		}
	} 
  else {
		phy_pair *pair = leaf->PAIRS;
		while (pair) {
      if (leaf == pair->b.leaf) {
				pair->id = context->func(pair->a.leaf->obj, leaf->obj, pair->id, context->data);
				pair = pair->b.next;
			} 
      else {
				pair = pair->a.next;
			}
		}
	}
}

static void MarkSubtree(phy_node *subtree, phy_mark_context *context) {
  if (phy_node_is_leaf(subtree)) {
		phy_mark_leaf(subtree, context);
	} 
  else {
		MarkSubtree(subtree->A, context);
		MarkSubtree(subtree->B, context);
	}
}

static phy_node *phy_leaf_new(phy_bb_tree *tree, void *obj, phy_bb bb) {
	phy_node *node = phy_node_from_pool(tree);
	node->obj = obj;
	node->bb = phy_get_bb(tree, obj);
	node->parent = NULL;
	node->STAMP = 0;
	node->PAIRS = NULL;
	return node;
}

static bool phy_leaf_update(phy_node *leaf, phy_bb_tree *tree) {
	phy_node *root = tree->root;
	phy_bb bb = tree->spatial_index.bbfunc(leaf->obj);
  if (!phy_bb_contains_bb(leaf->bb, bb)) {
		leaf->bb = phy_get_bb(tree, leaf->obj);
		root = phy_subtree_remove(root, leaf, tree);
		tree->root = phy_subtree_insert(root, leaf, tree);
		phy_pairs_clear(leaf, tree);
		leaf->STAMP = get_master_tree(tree)->stamp;
		return true;
	} 
  else {
		return false;
	}
}

static void phy_leaf_add_pairs(phy_node *leaf, phy_bb_tree *tree) {
	phy_spatial_index *dynamicIndex = tree->spatial_index.dynamic_index;
  if (dynamicIndex) {
		phy_node *dynamicRoot = get_root_if_tree(dynamicIndex);
		if (dynamicRoot) {
			phy_bb_tree *dynamicTree = phy_get_tree(dynamicIndex);
			phy_mark_context context = { dynamicTree, NULL, NULL, NULL };
			phy_mark_leaf_query(dynamicRoot, leaf, true, &context);
		}
	} 
  else {
		phy_node *staticRoot = get_root_if_tree(tree->spatial_index.static_index);
		phy_mark_context context = { tree, staticRoot, phy_void_query_func, NULL };
		phy_mark_leaf(leaf, &context);
	}
}

phy_bb_tree *phy_bb_tree_alloc(void) {
	return (phy_bb_tree *)calloc(1, sizeof(phy_bb_tree));
}

static int phy_leaf_set_eql(void *obj, phy_node *node) {
	return (obj == node->obj);
}

static void *phy_leaf_set_trans(void *obj, phy_bb_tree *tree) {
	return phy_leaf_new(tree, obj, tree->spatial_index.bbfunc(obj));
}

phy_spatial_index *phy_bb_tree_init(phy_bb_tree *tree, phy_spatial_index_BB_func bbfunc, phy_spatial_index *staticIndex) {
	phy_spatial_index_init((phy_spatial_index *)tree, phy_class(), bbfunc, staticIndex);
	tree->velocity_func = NULL;
	tree->leaves = cp_hash_set_new(0, (phy_hash_set_eql_func)phy_leaf_set_eql);
	tree->root = NULL;
	tree->pooled_nodes = NULL;
	tree->allocated_buffers = phy_array_new(0);
	tree->stamp = 0;
	return (phy_spatial_index *)tree;
}

void phy_bb_tree_set_velocity_func(phy_spatial_index *index, phy_bb_tree_velocity_func func) {
	if (index->class != phy_class()) {
		utl_error_func("Ignoring velocity set call to non-tree spatial index", utl_user_defined_data);
    return;
	}
	((phy_bb_tree *)index)->velocity_func = func;
}

phy_spatial_index *phy_bb_tree_new(phy_spatial_index_BB_func bbfunc, phy_spatial_index *staticIndex) {
	return phy_bb_tree_init(phy_bb_tree_alloc(), bbfunc, staticIndex);
}

static void phy_bb_tree_destroy(phy_bb_tree *tree) {
	phy_hash_set_free(tree->leaves);
	if (tree->allocated_buffers) {
    phy_array_free_each(tree->allocated_buffers, free);
  }
	phy_array_free(tree->allocated_buffers);
}

static void phy_bb_tree_insert(phy_bb_tree *tree, void *obj, phy_hash_value hashid) {
	phy_node *leaf = (phy_node *)phy_hash_set_insert(tree->leaves, hashid, obj, (phy_hash_set_trans_func)phy_leaf_set_trans, tree);
	phy_node *root = tree->root;
	tree->root = phy_subtree_insert(root, leaf, tree);
	leaf->STAMP = get_master_tree(tree)->stamp;
	phy_leaf_add_pairs(leaf, tree);
	phy_increment_stamp(tree);
}

static void phy_bb_tree_remove(phy_bb_tree *tree, void *obj, phy_hash_value hashid) {
	phy_node *leaf = (phy_node *)phy_hash_set_remove(tree->leaves, hashid, obj);
	tree->root = phy_subtree_remove(tree->root, leaf, tree);
	phy_pairs_clear(leaf, tree);
	phy_node_recycle(tree, leaf);
}

static bool phy_bb_tree_contains(phy_bb_tree *tree, void *obj, phy_hash_value hashid) {
	return (phy_hash_set_find(tree->leaves, hashid, obj) != NULL);
}

static void phy_leaf_update_wrap(phy_node *leaf, phy_bb_tree *tree) { 
  phy_leaf_update(leaf, tree); 
}

static void phy_bb_tree_reindex_query(phy_bb_tree *tree, phy_spatial_index_query_func func, void *data) {
	if (!tree->root) {
    return;
  }
	phy_hash_set_each(tree->leaves, (phy_hash_set_iterator_func)phy_leaf_update_wrap, tree);
	phy_spatial_index *staticIndex = tree->spatial_index.static_index;
	phy_node *staticRoot = (staticIndex && staticIndex->class == phy_class() ? ((phy_bb_tree *)staticIndex)->root : NULL);
	phy_mark_context context = {tree, staticRoot, func, data};
	MarkSubtree(tree->root, &context);
	if (staticIndex && !staticRoot) {
    phy_spatial_index_collide_static((phy_spatial_index *)tree, staticIndex, func, data);
  }
	phy_increment_stamp(tree);
}

static void phy_bb_tree_reindex(phy_bb_tree *tree) {
	phy_bb_tree_reindex_query(tree, phy_void_query_func, NULL);
}

static void phy_bb_tree_reindex_object(phy_bb_tree *tree, void *obj, phy_hash_value hashid) {
	phy_node *leaf = (phy_node *)phy_hash_set_find(tree->leaves, hashid, obj);
	if (leaf) {
		if (phy_leaf_update(leaf, tree)) {
      phy_leaf_add_pairs(leaf, tree);
    }
		phy_increment_stamp(tree);
	}
}

static void phy_bb_tree_segment_query(phy_bb_tree *tree, void *obj, phy_vect a, phy_vect b, float t_exit, phy_spatial_index_segment_query_func func, void *data) {
	phy_node *root = tree->root;
	if (root) {
    phy_subtree_segment_query(root, obj, a, b, t_exit, func, data);
  }
}

static void phy_bb_tree_query(phy_bb_tree *tree, void *obj, phy_bb bb, phy_spatial_index_query_func func, void *data) {
	if (tree->root) {
    phy_subtree_query(tree->root, obj, bb, func, data);
  }
}

static int phy_bb_tree_count(phy_bb_tree *tree) {
	return phy_hash_set_count(tree->leaves);
}

static void phy_each_helper(phy_node *node, phy_each_context *context) {
  context->func(node->obj, context->data);
}

static void phy_bb_tree_each(phy_bb_tree *tree, phy_spatial_index_iterator_func func, void *data) {
	phy_each_context context = { func, data };
	phy_hash_set_each(tree->leaves, (phy_hash_set_iterator_func)phy_each_helper, &context);
}

static phy_spatial_index_class class = {
	(phy_spatial_index_destroy_impl)phy_bb_tree_destroy,
	(phy_spatial_index_count_impl)phy_bb_tree_count,
	(phy_spatial_index_each_impl)phy_bb_tree_each,
	(phy_spatial_index_contains_impl)phy_bb_tree_contains,
	(phy_spatial_index_insert_impl)phy_bb_tree_insert,
	(phy_spatial_index_remove_impl)phy_bb_tree_remove,
	(phy_spatial_index_reindex_impl)phy_bb_tree_reindex,
	(phy_spatial_index_reindex_object_impl)phy_bb_tree_reindex_object,
	(phy_spatial_index_reindex_query_impl)phy_bb_tree_reindex_query,
	(phy_spatial_index_query_impl)phy_bb_tree_query,
	(phy_spatial_index_segment_query_impl)phy_bb_tree_segment_query,
};

static inline phy_spatial_index_class *phy_class(){ 
  return &class;
}

static int phy_cpfcompare(const float *a, const float *b){
	return (*a < *b ? -1 : (*b < *a ? 1 : 0));
}

static void phy_fill_node_array(phy_node *node, phy_node ***cursor){
	(**cursor) = node;
	(*cursor)++;
}

static phy_node *phy_partition_nodes(phy_bb_tree *tree, phy_node **nodes, int count) {
  if (count == 1) {
		return nodes[0];
	} 
  else if(count == 2) {
		return phy_node_new(tree, nodes[0], nodes[1]);
	}
	phy_bb bb = nodes[0]->bb;
	for(int i=1; i<count; i++) {
    bb = phy_bb_merge(bb, nodes[i]->bb);
  }
	bool splitWidth = (bb.r - bb.l > bb.t - bb.b);
	float *bounds = (float *)calloc(count*2, sizeof(float));
  if (splitWidth) {
		for (int i=0; i<count; i++) {
			bounds[2*i + 0] = nodes[i]->bb.l;
			bounds[2*i + 1] = nodes[i]->bb.r;
		}
	} 
  else {
		for (int i=0; i<count; i++) {
			bounds[2*i + 0] = nodes[i]->bb.b;
			bounds[2*i + 1] = nodes[i]->bb.t;
		}
	}
	qsort(bounds, count*2, sizeof(float), (int (*)(const void *, const void *))phy_cpfcompare);
	float split = (bounds[count - 1] + bounds[count]) * 0.5f; 
	free(bounds);
	phy_bb a = bb, b = bb;
  if (splitWidth) {
    a.r = b.l = split; 
  }
  else {
    a.t = b.b = split;
  }
	int right = count;
	for (int left=0; left < right;) {
		phy_node *node = nodes[left];
    if (phy_bb_merged_area(node->bb, b) < phy_bb_merged_area(node->bb, a)) {
			right--;
			nodes[left] = nodes[right];
			nodes[right] = node;
		} 
    else {
			left++;
		}
	}
	if (right == count) {
		phy_node *node = NULL;
		for (int i=0; i<count; i++) {
      node = phy_subtree_insert(node, nodes[i], tree);
    }
		return node;
	}
	return phy_node_new(tree, phy_partition_nodes(tree, nodes, right), phy_partition_nodes(tree, nodes + right, count - right));
}

void phy_bb_tree_optimize(phy_spatial_index *index) {
	if (index->class != &class) {
		utl_error_func("Ignoring optimize call to non-tree spatial index", utl_user_defined_data);
		return;
	}
	phy_bb_tree *tree = (phy_bb_tree *)index;
	phy_node *root = tree->root;
	if(!root) {
    return;
  }
	int count = phy_bb_tree_count(tree);
	phy_node **nodes = (phy_node **)calloc(count, sizeof(phy_node *));
	phy_node **cursor = nodes;
	phy_hash_set_each(tree->leaves, (phy_hash_set_iterator_func)phy_fill_node_array, &cursor);
	phy_subtree_recycle(tree, root);
	tree->root = phy_partition_nodes(tree, nodes, count);
	free(nodes);
}

