#include "khg_phy/phy_private.h"
#include "khg_phy/prime.h"
#include "khg_phy/spatial_index.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

typedef struct cpSpaceHashBin cpSpaceHashBin;
typedef struct cpHandle cpHandle;

struct phy_space_hash {
	phy_spatial_index spatialIndex;
	
	int numcells;
	float celldim;
	
	cpSpaceHashBin **table;
	phy_hash_set *handleSet;
	
	cpSpaceHashBin *pooledBins;
	phy_array *pooledHandles;
	phy_array *allocatedBuffers;
	
	phy_timestamp stamp;
};


//MARK: Handle Functions

struct cpHandle {
	void *obj;
	int retain;
	phy_timestamp stamp;
};

static cpHandle*
cpHandleInit(cpHandle *hand, void *obj)
{
	hand->obj = obj;
	hand->retain = 0;
	hand->stamp = 0;
	
	return hand;
}

static inline void cpHandleRetain(cpHandle *hand){hand->retain++;}

static inline void
cpHandleRelease(cpHandle *hand, phy_array *pooledHandles)
{
	hand->retain--;
	if(hand->retain == 0) phy_array_push(pooledHandles, hand);
}

static int handleSetEql(void *obj, cpHandle *hand){return (obj == hand->obj);}

static void *
handleSetTrans(void *obj, phy_space_hash *hash)
{
	if(hash->pooledHandles->num == 0){
		// handle pool is exhausted, make more
		int count = PHY_BUFFER_BYTES/sizeof(cpHandle);
		if (!count) {
      utl_error_func("Buffer size is too small", utl_user_defined_data);
    }
		
		cpHandle *buffer = (cpHandle *)calloc(1, PHY_BUFFER_BYTES);
		phy_array_push(hash->allocatedBuffers, buffer);
		
		for(int i=0; i<count; i++) phy_array_push(hash->pooledHandles, buffer + i);
	}
	
	cpHandle *hand = cpHandleInit((cpHandle *)phy_array_pop(hash->pooledHandles), obj);
	cpHandleRetain(hand);
	
	return hand;
}

//MARK: Bin Functions

struct cpSpaceHashBin {
	cpHandle *handle;
	cpSpaceHashBin *next;
};

static inline void
recycleBin(phy_space_hash *hash, cpSpaceHashBin *bin)
{
	bin->next = hash->pooledBins;
	hash->pooledBins = bin;
}

static inline void
clearTableCell(phy_space_hash *hash, int idx)
{
	cpSpaceHashBin *bin = hash->table[idx];
	while(bin){
		cpSpaceHashBin *next = bin->next;
		
		cpHandleRelease(bin->handle, hash->pooledHandles);
		recycleBin(hash, bin);
		
		bin = next;
	}
	
	hash->table[idx] = NULL;
}

static void
clearTable(phy_space_hash *hash)
{
	for(int i=0; i<hash->numcells; i++) clearTableCell(hash, i);
}

// Get a recycled or new bin.
static inline cpSpaceHashBin *
getEmptyBin(phy_space_hash *hash)
{
	cpSpaceHashBin *bin = hash->pooledBins;
	
	if(bin){
		hash->pooledBins = bin->next;
		return bin;
	} else {
		// Pool is exhausted, make more
		int count = PHY_BUFFER_BYTES/sizeof(cpSpaceHashBin);
		if (!count) {
      utl_error_func("Buffer size is too small", utl_user_defined_data);
    }
		
		cpSpaceHashBin *buffer = (cpSpaceHashBin *)calloc(1, PHY_BUFFER_BYTES);
		phy_array_push(hash->allocatedBuffers, buffer);
		
		// push all but the first one, return the first instead
		for(int i=1; i<count; i++) recycleBin(hash, buffer + i);
		return buffer;
	}
}

//MARK: Memory Management Functions

phy_space_hash *
phy_space_hash_alloc(void)
{
	return (phy_space_hash *)calloc(1, sizeof(phy_space_hash));
}

// Frees the old table, and allocate a new one.
static void
cpSpaceHashAllocTable(phy_space_hash *hash, int numcells)
{
	free(hash->table);
	
	hash->numcells = numcells;
	hash->table = (cpSpaceHashBin **)calloc(numcells, sizeof(cpSpaceHashBin *));
}

static inline phy_spatial_index_class *Klass(void);

phy_spatial_index *
phy_space_hash_init(phy_space_hash *hash, float celldim, int numcells, phy_spatial_index_BB_func bbfunc, phy_spatial_index *staticIndex)
{
	phy_spatial_index_init((phy_spatial_index *)hash, Klass(), bbfunc, staticIndex);
	
	cpSpaceHashAllocTable(hash, next_prime(numcells));
	hash->celldim = celldim;
	
	hash->handleSet = cp_hash_set_new(0, (phy_hash_set_eql_func)handleSetEql);
	
	hash->pooledHandles = phy_array_new(0);
	
	hash->pooledBins = NULL;
	hash->allocatedBuffers = phy_array_new(0);
	
	hash->stamp = 1;
	
	return (phy_spatial_index *)hash;
}

phy_spatial_index *
phy_space_hash_new(float celldim, int cells, phy_spatial_index_BB_func bbfunc, phy_spatial_index *staticIndex)
{
	return phy_space_hash_init(phy_space_hash_alloc(), celldim, cells, bbfunc, staticIndex);
}

static void
cpSpaceHashDestroy(phy_space_hash *hash)
{
	if(hash->table) clearTable(hash);
	free(hash->table);
	
	phy_hash_set_free(hash->handleSet);
	
	phy_array_free_each(hash->allocatedBuffers, free);
	phy_array_free(hash->allocatedBuffers);
	phy_array_free(hash->pooledHandles);
}

//MARK: Helper Functions

static inline bool
containsHandle(cpSpaceHashBin *bin, cpHandle *hand)
{
	while(bin){
		if(bin->handle == hand) return true;
		bin = bin->next;
	}
	
	return false;
}

// The hash function itself.
static inline phy_hash_value
hash_func(phy_hash_value x, phy_hash_value y, phy_hash_value n)
{
	return (x*1640531513ul ^ y*2654435789ul) % n;
}

// Much faster than (int)floor(f)
// Profiling showed floor() to be a sizable performance hog
static inline int
floor_int(float f)
{
	int i = (int)f;
	return (f < 0.0f && f != i ? i - 1 : i);
}

static inline void
hashHandle(phy_space_hash *hash, cpHandle *hand, phy_bb bb)
{
	// Find the dimensions in cell coordinates.
	float dim = hash->celldim;
	int l = floor_int(bb.l/dim); // Fix by ShiftZ
	int r = floor_int(bb.r/dim);
	int b = floor_int(bb.b/dim);
	int t = floor_int(bb.t/dim);
	
	int n = hash->numcells;
	for(int i=l; i<=r; i++){
		for(int j=b; j<=t; j++){
			phy_hash_value idx = hash_func(i,j,n);
			cpSpaceHashBin *bin = hash->table[idx];
			
			// Don't add an object twice to the same cell.
			if(containsHandle(bin, hand)) continue;

			cpHandleRetain(hand);
			// Insert a new bin for the handle in this cell.
			cpSpaceHashBin *newBin = getEmptyBin(hash);
			newBin->handle = hand;
			newBin->next = bin;
			hash->table[idx] = newBin;
		}
	}
}

//MARK: Basic Operations

static void
cpSpaceHashInsert(phy_space_hash *hash, void *obj, phy_hash_value hashid)
{
	cpHandle *hand = (cpHandle *)phy_hash_set_insert(hash->handleSet, hashid, obj, (phy_hash_set_trans_func)handleSetTrans, hash);
	hashHandle(hash, hand, hash->spatialIndex.bbfunc(obj));
}

static void
cpSpaceHashRehashObject(phy_space_hash *hash, void *obj, phy_hash_value hashid)
{
	cpHandle *hand = (cpHandle *)phy_hash_set_remove(hash->handleSet, hashid, obj);
	
	if(hand){
		hand->obj = NULL;
		cpHandleRelease(hand, hash->pooledHandles);
		
		cpSpaceHashInsert(hash, obj, hashid);
	}
}

static void
rehash_helper(cpHandle *hand, phy_space_hash *hash)
{
	hashHandle(hash, hand, hash->spatialIndex.bbfunc(hand->obj));
}

static void
cpSpaceHashRehash(phy_space_hash *hash)
{
	clearTable(hash);
	phy_hash_set_each(hash->handleSet, (phy_hash_set_iterator_func)rehash_helper, hash);
}

static void
cpSpaceHashRemove(phy_space_hash *hash, void *obj, phy_hash_value hashid)
{
	cpHandle *hand = (cpHandle *)phy_hash_set_remove(hash->handleSet, hashid, obj);
	
	if(hand){
		hand->obj = NULL;
		cpHandleRelease(hand, hash->pooledHandles);
	}
}

typedef struct eachContext {
	phy_spatial_index_iterator_func func;
	void *data;
} eachContext;

static void eachHelper(cpHandle *hand, eachContext *context){context->func(hand->obj, context->data);}

static void
cpSpaceHashEach(phy_space_hash *hash, phy_spatial_index_iterator_func func, void *data)
{
	eachContext context = {func, data};
	phy_hash_set_each(hash->handleSet, (phy_hash_set_iterator_func)eachHelper, &context);
}

static void
remove_orphaned_handles(phy_space_hash *hash, cpSpaceHashBin **bin_ptr)
{
	cpSpaceHashBin *bin = *bin_ptr;
	while(bin){
		cpHandle *hand = bin->handle;
		cpSpaceHashBin *next = bin->next;
		
		if(!hand->obj){
			// orphaned handle, unlink and recycle the bin
			(*bin_ptr) = bin->next;
			recycleBin(hash, bin);
			
			cpHandleRelease(hand, hash->pooledHandles);
		} else {
			bin_ptr = &bin->next;
		}
		
		bin = next;
	}
}

//MARK: Query Functions

static inline void
query_helper(phy_space_hash *hash, cpSpaceHashBin **bin_ptr, void *obj, phy_spatial_index_query_func func, void *data)
{
	restart:
	for(cpSpaceHashBin *bin = *bin_ptr; bin; bin = bin->next){
		cpHandle *hand = bin->handle;
		void *other = hand->obj;
		
		if(hand->stamp == hash->stamp || obj == other){
			continue;
		} else if(other){
			func(obj, other, 0, data);
			hand->stamp = hash->stamp;
		} else {
			// The object for this handle has been removed
			// cleanup this cell and restart the query
			remove_orphaned_handles(hash, bin_ptr);
			goto restart; // GCC not smart enough/able to tail call an inlined function.
		}
	}
}

static void
cpSpaceHashQuery(phy_space_hash *hash, void *obj, phy_bb bb, phy_spatial_index_query_func func, void *data)
{
	// Get the dimensions in cell coordinates.
	float dim = hash->celldim;
	int l = floor_int(bb.l/dim);  // Fix by ShiftZ
	int r = floor_int(bb.r/dim);
	int b = floor_int(bb.b/dim);
	int t = floor_int(bb.t/dim);
	
	int n = hash->numcells;
	cpSpaceHashBin **table = hash->table;
	
	// Iterate over the cells and query them.
	for(int i=l; i<=r; i++){
		for(int j=b; j<=t; j++){
			query_helper(hash, &table[hash_func(i,j,n)], obj, func, data);
		}
	}
	
	hash->stamp++;
}

// Similar to struct eachPair above.
typedef struct queryRehashContext {
	phy_space_hash *hash;
	phy_spatial_index_query_func func;
	void *data;
} queryRehashContext;

// Hashset iterator func used with cpSpaceHashQueryRehash().
static void
queryRehash_helper(cpHandle *hand, queryRehashContext *context)
{
	phy_space_hash *hash = context->hash;
	phy_spatial_index_query_func func = context->func;
	void *data = context->data;

	float dim = hash->celldim;
	int n = hash->numcells;

	void *obj = hand->obj;
	phy_bb bb = hash->spatialIndex.bbfunc(obj);

	int l = floor_int(bb.l/dim);
	int r = floor_int(bb.r/dim);
	int b = floor_int(bb.b/dim);
	int t = floor_int(bb.t/dim);
	
	cpSpaceHashBin **table = hash->table;

	for(int i=l; i<=r; i++){
		for(int j=b; j<=t; j++){
			phy_hash_value idx = hash_func(i,j,n);
			cpSpaceHashBin *bin = table[idx];
			
			if(containsHandle(bin, hand)) continue;
			
			cpHandleRetain(hand); // this MUST be done first in case the object is removed in func()
			query_helper(hash, &bin, obj, func, data);
			
			cpSpaceHashBin *newBin = getEmptyBin(hash);
			newBin->handle = hand;
			newBin->next = bin;
			table[idx] = newBin;
		}
	}
	
	// Increment the stamp for each object hashed.
	hash->stamp++;
}

static void
cpSpaceHashReindexQuery(phy_space_hash *hash, phy_spatial_index_query_func func, void *data)
{
	clearTable(hash);
	
	queryRehashContext context = {hash, func, data};
	phy_hash_set_each(hash->handleSet, (phy_hash_set_iterator_func)queryRehash_helper, &context);
	
	phy_spatial_index_collide_static((phy_spatial_index *)hash, hash->spatialIndex.static_index, func, data);
}

static inline float
segmentQuery_helper(phy_space_hash *hash, cpSpaceHashBin **bin_ptr, void *obj, phy_spatial_index_segment_query_func func, void *data)
{
	float t = 1.0f;
	 
	restart:
	for(cpSpaceHashBin *bin = *bin_ptr; bin; bin = bin->next){
		cpHandle *hand = bin->handle;
		void *other = hand->obj;
		
		// Skip over certain conditions
		if(hand->stamp == hash->stamp){
			continue;
		} else if(other){
			t = phy_min(t, func(obj, other, data));
			hand->stamp = hash->stamp;
		} else {
			// The object for this handle has been removed
			// cleanup this cell and restart the query
			remove_orphaned_handles(hash, bin_ptr);
			goto restart; // GCC not smart enough/able to tail call an inlined function.
		}
	}
	
	return t;
}

// modified from http://playtechs.blogspot.com/2007/03/raytracing-on-grid.html
static void
cpSpaceHashSegmentQuery(phy_space_hash *hash, void *obj, phy_vect a, phy_vect b, float t_exit, phy_spatial_index_segment_query_func func, void *data)
{
	a = phy_v_mult(a, 1.0f/hash->celldim);
	b = phy_v_mult(b, 1.0f/hash->celldim);
	
	int cell_x = floor_int(a.x), cell_y = floor_int(a.y);

	float t = 0;

	int x_inc, y_inc;
	float temp_v, temp_h;

	if (b.x > a.x){
		x_inc = 1;
		temp_h = (floorf(a.x + 1.0f) - a.x);
	} else {
		x_inc = -1;
		temp_h = (a.x - floorf(a.x));
	}

	if (b.y > a.y){
		y_inc = 1;
		temp_v = (floorf(a.y + 1.0f) - a.y);
	} else {
		y_inc = -1;
		temp_v = (a.y - floorf(a.y));
	}
	
	// Division by zero is *very* slow on ARM
	float dx = phy_abs(b.x - a.x), dy = phy_abs(b.y - a.y);
	float dt_dx = (dx ? 1.0f/dx : INFINITY), dt_dy = (dy ? 1.0f/dy : INFINITY);
	
	// fix NANs in horizontal directions
	float next_h = (temp_h ? temp_h*dt_dx : dt_dx);
	float next_v = (temp_v ? temp_v*dt_dy : dt_dy);
	
	int n = hash->numcells;
	cpSpaceHashBin **table = hash->table;

	while(t < t_exit){
		phy_hash_value idx = hash_func(cell_x, cell_y, n);
		t_exit = phy_min(t_exit, segmentQuery_helper(hash, &table[idx], obj, func, data));

		if (next_v < next_h){
			cell_y += y_inc;
			t = next_v;
			next_v += dt_dy;
		} else {
			cell_x += x_inc;
			t = next_h;
			next_h += dt_dx;
		}
	}
	
	hash->stamp++;
}

//MARK: Misc

void
phy_space_hash_resize(phy_space_hash *hash, float celldim, int numcells)
{
	if(hash->spatialIndex.class != Klass()){
		utl_error_func("Ignoring call to non-cpSpaceHash spatial index", utl_user_defined_data);
		return;
	}
	
	clearTable(hash);
	
	hash->celldim = celldim;
	cpSpaceHashAllocTable(hash, next_prime(numcells));
}

static int
cpSpaceHashCount(phy_space_hash *hash)
{
	return phy_hash_set_count(hash->handleSet);
}

static int
cpSpaceHashContains(phy_space_hash *hash, void *obj, phy_hash_value hashid)
{
	return phy_hash_set_find(hash->handleSet, hashid, obj) != NULL;
}

static phy_spatial_index_class klass = {
	(phy_spatial_index_destroy_impl)cpSpaceHashDestroy,
	
	(phy_spatial_index_count_impl)cpSpaceHashCount,
	(phy_spatial_index_each_impl)cpSpaceHashEach,
	(phy_spatial_index_contains_impl)cpSpaceHashContains,
	
	(phy_spatial_index_insert_impl)cpSpaceHashInsert,
	(phy_spatial_index_remove_impl)cpSpaceHashRemove,
	
	(phy_spatial_index_reindex_impl)cpSpaceHashRehash,
	(phy_spatial_index_reindex_object_impl)cpSpaceHashRehashObject,
	(phy_spatial_index_reindex_query_impl)cpSpaceHashReindexQuery,
	
	(phy_spatial_index_query_impl)cpSpaceHashQuery,
	(phy_spatial_index_segment_query_impl)cpSpaceHashSegmentQuery,
};

static inline phy_spatial_index_class *Klass(){return &klass;}

//MARK: Debug Drawing

//#define CP_BBTREE_DEBUG_DRAW
#ifdef CP_BBTREE_DEBUG_DRAW
#include "OpenGL/gl.h"
#include "OpenGL/glu.h"
#include <GLUT/glut.h>

void
cpSpaceHashRenderDebug(cpSpatialIndex *index)
{
	if(index->klass != &klass){
		utl_error_func("Ignoring call to non-spatial hash spatial index", utl_user_defined_data);
		return;
	}
	
	cpSpaceHash *hash = (cpSpaceHash *)index;
	cpBB bb = cpBBNew(-320, -240, 320, 240);
	
	float dim = hash->celldim;
	int n = hash->numcells;
	
	int l = (int)floor(bb.l/dim);
	int r = (int)floor(bb.r/dim);
	int b = (int)floor(bb.b/dim);
	int t = (int)floor(bb.t/dim);
	
	for(int i=l; i<=r; i++){
		for(int j=b; j<=t; j++){
			int cell_count = 0;
			
			int index = hash_func(i,j,n);
			for(cpSpaceHashBin *bin = hash->table[index]; bin; bin = bin->next)
				cell_count++;
			
			GLfloat v = 1.0f - (GLfloat)cell_count/10.0f;
			glColor3f(v,v,v);
			glRectf(i*dim, j*dim, (i + 1)*dim, (j + 1)*dim);
		}
	}
}
#endif
