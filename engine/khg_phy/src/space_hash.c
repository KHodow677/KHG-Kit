#include "khg_phy/phy_private.h"
#include "khg_phy/prime.h"
#include "khg_utl/error_func.h"

typedef struct cpSpaceHashBin cpSpaceHashBin;
typedef struct cpHandle cpHandle;

struct cpSpaceHash {
	cpSpatialIndex spatialIndex;
	
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
handleSetTrans(void *obj, cpSpaceHash *hash)
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
recycleBin(cpSpaceHash *hash, cpSpaceHashBin *bin)
{
	bin->next = hash->pooledBins;
	hash->pooledBins = bin;
}

static inline void
clearTableCell(cpSpaceHash *hash, int idx)
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
clearTable(cpSpaceHash *hash)
{
	for(int i=0; i<hash->numcells; i++) clearTableCell(hash, i);
}

// Get a recycled or new bin.
static inline cpSpaceHashBin *
getEmptyBin(cpSpaceHash *hash)
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

cpSpaceHash *
cpSpaceHashAlloc(void)
{
	return (cpSpaceHash *)calloc(1, sizeof(cpSpaceHash));
}

// Frees the old table, and allocate a new one.
static void
cpSpaceHashAllocTable(cpSpaceHash *hash, int numcells)
{
	free(hash->table);
	
	hash->numcells = numcells;
	hash->table = (cpSpaceHashBin **)calloc(numcells, sizeof(cpSpaceHashBin *));
}

static inline cpSpatialIndexClass *Klass(void);

cpSpatialIndex *
cpSpaceHashInit(cpSpaceHash *hash, float celldim, int numcells, cpSpatialIndexBBFunc bbfunc, cpSpatialIndex *staticIndex)
{
	phy_spatial_index_init((cpSpatialIndex *)hash, Klass(), bbfunc, staticIndex);
	
	cpSpaceHashAllocTable(hash, next_prime(numcells));
	hash->celldim = celldim;
	
	hash->handleSet = cp_hash_set_new(0, (phy_hash_set_eql_func)handleSetEql);
	
	hash->pooledHandles = phy_array_new(0);
	
	hash->pooledBins = NULL;
	hash->allocatedBuffers = phy_array_new(0);
	
	hash->stamp = 1;
	
	return (cpSpatialIndex *)hash;
}

cpSpatialIndex *
cpSpaceHashNew(float celldim, int cells, cpSpatialIndexBBFunc bbfunc, cpSpatialIndex *staticIndex)
{
	return cpSpaceHashInit(cpSpaceHashAlloc(), celldim, cells, bbfunc, staticIndex);
}

static void
cpSpaceHashDestroy(cpSpaceHash *hash)
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
hashHandle(cpSpaceHash *hash, cpHandle *hand, phy_bb bb)
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
cpSpaceHashInsert(cpSpaceHash *hash, void *obj, phy_hash_value hashid)
{
	cpHandle *hand = (cpHandle *)phy_hash_set_insert(hash->handleSet, hashid, obj, (phy_hash_set_trans_func)handleSetTrans, hash);
	hashHandle(hash, hand, hash->spatialIndex.bbfunc(obj));
}

static void
cpSpaceHashRehashObject(cpSpaceHash *hash, void *obj, phy_hash_value hashid)
{
	cpHandle *hand = (cpHandle *)phy_hash_set_remove(hash->handleSet, hashid, obj);
	
	if(hand){
		hand->obj = NULL;
		cpHandleRelease(hand, hash->pooledHandles);
		
		cpSpaceHashInsert(hash, obj, hashid);
	}
}

static void
rehash_helper(cpHandle *hand, cpSpaceHash *hash)
{
	hashHandle(hash, hand, hash->spatialIndex.bbfunc(hand->obj));
}

static void
cpSpaceHashRehash(cpSpaceHash *hash)
{
	clearTable(hash);
	phy_hash_set_each(hash->handleSet, (phy_hash_set_iterator_func)rehash_helper, hash);
}

static void
cpSpaceHashRemove(cpSpaceHash *hash, void *obj, phy_hash_value hashid)
{
	cpHandle *hand = (cpHandle *)phy_hash_set_remove(hash->handleSet, hashid, obj);
	
	if(hand){
		hand->obj = NULL;
		cpHandleRelease(hand, hash->pooledHandles);
	}
}

typedef struct eachContext {
	cpSpatialIndexIteratorFunc func;
	void *data;
} eachContext;

static void eachHelper(cpHandle *hand, eachContext *context){context->func(hand->obj, context->data);}

static void
cpSpaceHashEach(cpSpaceHash *hash, cpSpatialIndexIteratorFunc func, void *data)
{
	eachContext context = {func, data};
	phy_hash_set_each(hash->handleSet, (phy_hash_set_iterator_func)eachHelper, &context);
}

static void
remove_orphaned_handles(cpSpaceHash *hash, cpSpaceHashBin **bin_ptr)
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
query_helper(cpSpaceHash *hash, cpSpaceHashBin **bin_ptr, void *obj, cpSpatialIndexQueryFunc func, void *data)
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
cpSpaceHashQuery(cpSpaceHash *hash, void *obj, phy_bb bb, cpSpatialIndexQueryFunc func, void *data)
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
	cpSpaceHash *hash;
	cpSpatialIndexQueryFunc func;
	void *data;
} queryRehashContext;

// Hashset iterator func used with cpSpaceHashQueryRehash().
static void
queryRehash_helper(cpHandle *hand, queryRehashContext *context)
{
	cpSpaceHash *hash = context->hash;
	cpSpatialIndexQueryFunc func = context->func;
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
cpSpaceHashReindexQuery(cpSpaceHash *hash, cpSpatialIndexQueryFunc func, void *data)
{
	clearTable(hash);
	
	queryRehashContext context = {hash, func, data};
	phy_hash_set_each(hash->handleSet, (phy_hash_set_iterator_func)queryRehash_helper, &context);
	
	cpSpatialIndexCollideStatic((cpSpatialIndex *)hash, hash->spatialIndex.staticIndex, func, data);
}

static inline float
segmentQuery_helper(cpSpaceHash *hash, cpSpaceHashBin **bin_ptr, void *obj, cpSpatialIndexSegmentQueryFunc func, void *data)
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
cpSpaceHashSegmentQuery(cpSpaceHash *hash, void *obj, phy_vect a, phy_vect b, float t_exit, cpSpatialIndexSegmentQueryFunc func, void *data)
{
	a = cpvmult(a, 1.0f/hash->celldim);
	b = cpvmult(b, 1.0f/hash->celldim);
	
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
cpSpaceHashResize(cpSpaceHash *hash, float celldim, int numcells)
{
	if(hash->spatialIndex.klass != Klass()){
		utl_error_func("Ignoring call to non-cpSpaceHash spatial index", utl_user_defined_data);
		return;
	}
	
	clearTable(hash);
	
	hash->celldim = celldim;
	cpSpaceHashAllocTable(hash, next_prime(numcells));
}

static int
cpSpaceHashCount(cpSpaceHash *hash)
{
	return phy_hash_set_count(hash->handleSet);
}

static int
cpSpaceHashContains(cpSpaceHash *hash, void *obj, phy_hash_value hashid)
{
	return phy_hash_set_find(hash->handleSet, hashid, obj) != NULL;
}

static cpSpatialIndexClass klass = {
	(cpSpatialIndexDestroyImpl)cpSpaceHashDestroy,
	
	(cpSpatialIndexCountImpl)cpSpaceHashCount,
	(cpSpatialIndexEachImpl)cpSpaceHashEach,
	(cpSpatialIndexContainsImpl)cpSpaceHashContains,
	
	(cpSpatialIndexInsertImpl)cpSpaceHashInsert,
	(cpSpatialIndexRemoveImpl)cpSpaceHashRemove,
	
	(cpSpatialIndexReindexImpl)cpSpaceHashRehash,
	(cpSpatialIndexReindexObjectImpl)cpSpaceHashRehashObject,
	(cpSpatialIndexReindexQueryImpl)cpSpaceHashReindexQuery,
	
	(cpSpatialIndexQueryImpl)cpSpaceHashQuery,
	(cpSpatialIndexSegmentQueryImpl)cpSpaceHashSegmentQuery,
};

static inline cpSpatialIndexClass *Klass(){return &klass;}

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
