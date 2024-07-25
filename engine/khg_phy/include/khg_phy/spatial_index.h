#pragma once

#include "khg_phy/phy_types.h"
#include "khg_phy/bb.h"

/// Spatial index bounding box callback function type.
/// The spatial index calls this function and passes you a pointer to an object you added
/// when it needs to get the bounding box associated with that object.
typedef cpBB (*cpSpatialIndexBBFunc)(void *obj);
/// Spatial index/object iterator callback function type.
typedef void (*cpSpatialIndexIteratorFunc)(void *obj, void *data);
/// Spatial query callback function type.
typedef cpCollisionID (*cpSpatialIndexQueryFunc)(void *obj1, void *obj2, cpCollisionID id, void *data);
/// Spatial segment query callback function type.
typedef cpFloat (*cpSpatialIndexSegmentQueryFunc)(void *obj1, void *obj2, void *data);


typedef struct cpSpatialIndexClass cpSpatialIndexClass;
typedef struct cpSpatialIndex cpSpatialIndex;

/// @private
struct cpSpatialIndex {
	cpSpatialIndexClass *klass;
	
	cpSpatialIndexBBFunc bbfunc;
	
	cpSpatialIndex *staticIndex, *dynamicIndex;
};


//MARK: Spatial Hash

typedef struct cpSpaceHash cpSpaceHash;

/// Allocate a spatial hash.
CP_EXPORT cpSpaceHash* cpSpaceHashAlloc(void);
/// Initialize a spatial hash. 
CP_EXPORT cpSpatialIndex* cpSpaceHashInit(cpSpaceHash *hash, cpFloat celldim, int numcells, cpSpatialIndexBBFunc bbfunc, cpSpatialIndex *staticIndex);
/// Allocate and initialize a spatial hash.
CP_EXPORT cpSpatialIndex* cpSpaceHashNew(cpFloat celldim, int cells, cpSpatialIndexBBFunc bbfunc, cpSpatialIndex *staticIndex);

/// Change the cell dimensions and table size of the spatial hash to tune it.
/// The cell dimensions should roughly match the average size of your objects
/// and the table size should be ~10 larger than the number of objects inserted.
/// Some trial and error is required to find the optimum numbers for efficiency.
CP_EXPORT void cpSpaceHashResize(cpSpaceHash *hash, cpFloat celldim, int numcells);

//MARK: AABB Tree

typedef struct cpBBTree cpBBTree;

/// Allocate a bounding box tree.
CP_EXPORT cpBBTree* cpBBTreeAlloc(void);
/// Initialize a bounding box tree.
CP_EXPORT cpSpatialIndex* cpBBTreeInit(cpBBTree *tree, cpSpatialIndexBBFunc bbfunc, cpSpatialIndex *staticIndex);
/// Allocate and initialize a bounding box tree.
CP_EXPORT cpSpatialIndex* cpBBTreeNew(cpSpatialIndexBBFunc bbfunc, cpSpatialIndex *staticIndex);

/// Perform a static top down optimization of the tree.
CP_EXPORT void cpBBTreeOptimize(cpSpatialIndex *index);

/// Bounding box tree velocity callback function.
/// This function should return an estimate for the object's velocity.
typedef cpVect (*cpBBTreeVelocityFunc)(void *obj);
/// Set the velocity function for the bounding box tree to enable temporal coherence.
CP_EXPORT void cpBBTreeSetVelocityFunc(cpSpatialIndex *index, cpBBTreeVelocityFunc func);

//MARK: Single Axis Sweep

typedef struct cpSweep1D cpSweep1D;

/// Allocate a 1D sort and sweep broadphase.
CP_EXPORT cpSweep1D* cpSweep1DAlloc(void);
/// Initialize a 1D sort and sweep broadphase.
CP_EXPORT cpSpatialIndex* cpSweep1DInit(cpSweep1D *sweep, cpSpatialIndexBBFunc bbfunc, cpSpatialIndex *staticIndex);
/// Allocate and initialize a 1D sort and sweep broadphase.
CP_EXPORT cpSpatialIndex* cpSweep1DNew(cpSpatialIndexBBFunc bbfunc, cpSpatialIndex *staticIndex);

//MARK: Spatial Index Implementation

typedef void (*cpSpatialIndexDestroyImpl)(cpSpatialIndex *index);

typedef int (*cpSpatialIndexCountImpl)(cpSpatialIndex *index);
typedef void (*cpSpatialIndexEachImpl)(cpSpatialIndex *index, cpSpatialIndexIteratorFunc func, void *data);

typedef cpBool (*cpSpatialIndexContainsImpl)(cpSpatialIndex *index, void *obj, cpHashValue hashid);
typedef void (*cpSpatialIndexInsertImpl)(cpSpatialIndex *index, void *obj, cpHashValue hashid);
typedef void (*cpSpatialIndexRemoveImpl)(cpSpatialIndex *index, void *obj, cpHashValue hashid);

typedef void (*cpSpatialIndexReindexImpl)(cpSpatialIndex *index);
typedef void (*cpSpatialIndexReindexObjectImpl)(cpSpatialIndex *index, void *obj, cpHashValue hashid);
typedef void (*cpSpatialIndexReindexQueryImpl)(cpSpatialIndex *index, cpSpatialIndexQueryFunc func, void *data);

typedef void (*cpSpatialIndexQueryImpl)(cpSpatialIndex *index, void *obj, cpBB bb, cpSpatialIndexQueryFunc func, void *data);
typedef void (*cpSpatialIndexSegmentQueryImpl)(cpSpatialIndex *index, void *obj, cpVect a, cpVect b, cpFloat t_exit, cpSpatialIndexSegmentQueryFunc func, void *data);

struct cpSpatialIndexClass {
	cpSpatialIndexDestroyImpl destroy;
	
	cpSpatialIndexCountImpl count;
	cpSpatialIndexEachImpl each;
	
	cpSpatialIndexContainsImpl contains;
	cpSpatialIndexInsertImpl insert;
	cpSpatialIndexRemoveImpl remove;
	
	cpSpatialIndexReindexImpl reindex;
	cpSpatialIndexReindexObjectImpl reindexObject;
	cpSpatialIndexReindexQueryImpl reindexQuery;
	
	cpSpatialIndexQueryImpl query;
	cpSpatialIndexSegmentQueryImpl segmentQuery;
};

/// Destroy and free a spatial index.
CP_EXPORT void cpSpatialIndexFree(cpSpatialIndex *index);
/// Collide the objects in @c dynamicIndex against the objects in @c staticIndex using the query callback function.
CP_EXPORT void cpSpatialIndexCollideStatic(cpSpatialIndex *dynamicIndex, cpSpatialIndex *staticIndex, cpSpatialIndexQueryFunc func, void *data);

/// Destroy a spatial index.
static inline void cpSpatialIndexDestroy(cpSpatialIndex *index)
{
	if(index->klass) index->klass->destroy(index);
}

/// Get the number of objects in the spatial index.
static inline int cpSpatialIndexCount(cpSpatialIndex *index)
{
	return index->klass->count(index);
}

/// Iterate the objects in the spatial index. @c func will be called once for each object.
static inline void cpSpatialIndexEach(cpSpatialIndex *index, cpSpatialIndexIteratorFunc func, void *data)
{
	index->klass->each(index, func, data);
}

/// Returns true if the spatial index contains the given object.
/// Most spatial indexes use hashed storage, so you must provide a hash value too.
static inline cpBool cpSpatialIndexContains(cpSpatialIndex *index, void *obj, cpHashValue hashid)
{
	return index->klass->contains(index, obj, hashid);
}

/// Add an object to a spatial index.
/// Most spatial indexes use hashed storage, so you must provide a hash value too.
static inline void cpSpatialIndexInsert(cpSpatialIndex *index, void *obj, cpHashValue hashid)
{
	index->klass->insert(index, obj, hashid);
}

/// Remove an object from a spatial index.
/// Most spatial indexes use hashed storage, so you must provide a hash value too.
static inline void cpSpatialIndexRemove(cpSpatialIndex *index, void *obj, cpHashValue hashid)
{
	index->klass->remove(index, obj, hashid);
}

/// Perform a full reindex of a spatial index.
static inline void cpSpatialIndexReindex(cpSpatialIndex *index)
{
	index->klass->reindex(index);
}

/// Reindex a single object in the spatial index.
static inline void cpSpatialIndexReindexObject(cpSpatialIndex *index, void *obj, cpHashValue hashid)
{
	index->klass->reindexObject(index, obj, hashid);
}

/// Perform a rectangle query against the spatial index, calling @c func for each potential match.
static inline void cpSpatialIndexQuery(cpSpatialIndex *index, void *obj, cpBB bb, cpSpatialIndexQueryFunc func, void *data)
{
	index->klass->query(index, obj, bb, func, data);
}

/// Perform a segment query against the spatial index, calling @c func for each potential match.
static inline void cpSpatialIndexSegmentQuery(cpSpatialIndex *index, void *obj, cpVect a, cpVect b, cpFloat t_exit, cpSpatialIndexSegmentQueryFunc func, void *data)
{
	index->klass->segmentQuery(index, obj, a, b, t_exit, func, data);
}

/// Simultaneously reindex and find all colliding objects.
/// @c func will be called once for each potentially overlapping pair of objects found.
/// If the spatial index was initialized with a static index, it will collide it's objects against that as well.
static inline void cpSpatialIndexReindexQuery(cpSpatialIndex *index, cpSpatialIndexQueryFunc func, void *data)
{
	index->klass->reindexQuery(index, func, data);
}
