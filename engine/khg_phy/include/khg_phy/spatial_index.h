#pragma once

#include "khg_phy/phy_types.h"
#include "khg_phy/bb.h"

typedef phy_bb (*phy_spatial_index_BB_func)(void *obj);
typedef void (*phy_spatial_index_iterator_func)(void *obj, void *data);
typedef phy_collision_id (*phy_spatial_index_query_func)(void *obj_1, void *obj_2, phy_collision_id id, void *data);
typedef float (*phy_spatial_index_segment_query_func)(void *obj_1, void *obj_2, void *data);

typedef struct phy_spatial_index_class phy_spatial_index_class;
typedef struct phy_spatial_index phy_spatial_index;

struct phy_spatial_index {
	phy_spatial_index_class *class;
	phy_spatial_index_BB_func bbfunc;
	phy_spatial_index *static_index, *dynamic_index;
};

typedef struct phy_space_hash phy_space_hash;

phy_space_hash *phy_space_hash_alloc(void);
phy_spatial_index *phy_space_hash_new(float celldim, int cells, phy_spatial_index_BB_func bbfunc, phy_spatial_index *static_index);
phy_spatial_index *phy_space_hash_init(phy_space_hash *hash, float celldim, int numcells, phy_spatial_index_BB_func bbfunc, phy_spatial_index *static_index);

void phy_space_hash_resize(phy_space_hash *hash, float celldim, int numcells);

typedef struct phy_BB_tree phy_BB_tree;

phy_BB_tree *phy_BB_tree_alloc(void);
phy_spatial_index *phy_BB_tree_new(phy_spatial_index_BB_func bbfunc, phy_spatial_index *static_index);
phy_spatial_index *phy_BB_tree_init(phy_BB_tree *tree, phy_spatial_index_BB_func bbfunc, phy_spatial_index *static_index);

void phy_BB_tree_optimize(phy_spatial_index *index);

typedef phy_vect (*phy_BB_tree_velocity_func)(void *obj);
void phy_BB_tree_set_velocity_func(phy_spatial_index *index, phy_BB_tree_velocity_func func);

typedef struct phy_sweep_1d phy_sweep_1d;

phy_sweep_1d *phy_sweep_1d_alloc(void);
phy_spatial_index *phy_sweep_1d_init(phy_sweep_1d *sweep, phy_spatial_index_BB_func bbfunc, phy_spatial_index *static_index);
phy_spatial_index *phy_sweep_1D_new(phy_spatial_index_BB_func bbfunc, phy_spatial_index *static_index);

typedef void (*phy_spatial_index_destroy_impl)(phy_spatial_index *index);
typedef int (*phy_spatial_index_count_impl)(phy_spatial_index *index);
typedef void (*phy_spatial_index_each_impl)(phy_spatial_index *index, phy_spatial_index_iterator_func func, void *data);

typedef bool (*phy_spatial_index_contains_impl)(phy_spatial_index *index, void *obj, phy_hash_value hashid);
typedef void (*phy_spatial_index_insert_impl)(phy_spatial_index *index, void *obj, phy_hash_value hashid);
typedef void (*phy_spatial_index_remove_impl)(phy_spatial_index *index, void *obj, phy_hash_value hashid);

typedef void (*phy_spatial_index_reindex_impl)(phy_spatial_index *index);
typedef void (*phy_spatial_index_reindex_object_impl)(phy_spatial_index *index, void *obj, phy_hash_value hashid);
typedef void (*phy_spatial_index_reindex_query_impl)(phy_spatial_index *index, phy_spatial_index_query_func func, void *data);

typedef void (*phy_spatial_index_query_impl)(phy_spatial_index *index, void *obj, phy_bb bb, phy_spatial_index_query_func func, void *data);
typedef void (*phy_spatial_index_segment_query_impl)(phy_spatial_index *index, void *obj, phy_vect a, phy_vect b, float t_exit, phy_spatial_index_segment_query_func func, void *data);

struct phy_spatial_index_class {
	phy_spatial_index_destroy_impl destroy;
	phy_spatial_index_count_impl count;
	phy_spatial_index_each_impl each;
	phy_spatial_index_contains_impl contains;
	phy_spatial_index_insert_impl insert;
	phy_spatial_index_remove_impl remove;
	phy_spatial_index_reindex_impl reindex;
	phy_spatial_index_reindex_object_impl reindexObject;
	phy_spatial_index_reindex_query_impl reindexQuery;
	phy_spatial_index_query_impl query;
	phy_spatial_index_segment_query_impl segmentQuery;
};

void phy_spatial_index_free(phy_spatial_index *index);
void phy_spatial_index_collide_static(phy_spatial_index *dynamic_index, phy_spatial_index *static_index, phy_spatial_index_query_func func, void *data);

static inline void phy_spatial_index_destroy(phy_spatial_index *index) {
	if(index->class) index->class->destroy(index);
}

static inline int phy_spatial_index_count(phy_spatial_index *index) {
	return index->class->count(index);
}

static inline void phy_spatial_index_each(phy_spatial_index *index, phy_spatial_index_iterator_func func, void *data) {
	index->class->each(index, func, data);
}

static inline bool phy_spatial_index_contains(phy_spatial_index *index, void *obj, phy_hash_value hashid) {
	return index->class->contains(index, obj, hashid);
}

static inline void phy_spatial_index_insert(phy_spatial_index *index, void *obj, phy_hash_value hashid) {
	index->class->insert(index, obj, hashid);
}

static inline void phy_spatial_index_remove(phy_spatial_index *index, void *obj, phy_hash_value hashid) {
	index->class->remove(index, obj, hashid);
}

static inline void phy_spatial_index_reindex(phy_spatial_index *index) {
	index->class->reindex(index);
}

static inline void phy_spatial_index_reindex_object(phy_spatial_index *index, void *obj, phy_hash_value hashid) {
	index->class->reindexObject(index, obj, hashid);
}

static inline void phy_spatial_index_query(phy_spatial_index *index, void *obj, phy_bb bb, phy_spatial_index_query_func func, void *data) {
	index->class->query(index, obj, bb, func, data);
}

static inline void phy_spatial_index_segment_query(phy_spatial_index *index, void *obj, phy_vect a, phy_vect b, float t_exit, phy_spatial_index_segment_query_func func, void *data) {
	index->class->segmentQuery(index, obj, a, b, t_exit, func, data);
}

static inline void phy_spatial_index_reindex_query(phy_spatial_index *index, phy_spatial_index_query_func func, void *data) {
	index->class->reindexQuery(index, func, data);
}

