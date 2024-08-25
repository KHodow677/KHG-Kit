#pragma once

#include "khg_phy/phy_types.h"
#include "khg_phy/bb.h"
#include "khg_phy/shape.h"

typedef bool (*phy_collision_begin_func)(phy_arbiter *arb, phy_space *space, phy_data_pointer user_data);
typedef bool (*phy_collision_pre_solve_func)(phy_arbiter *arb, phy_space *space, phy_data_pointer user_data);
typedef void (*phy_collision_post_solve_func)(phy_arbiter *arb, phy_space *space, phy_data_pointer user_data);
typedef void (*phy_collision_separate_func)(phy_arbiter *arb, phy_space *space, phy_data_pointer user_data);

struct phy_collision_handler {
	const phy_collision_type type_A;
	const phy_collision_type type_B;
	phy_collision_begin_func begin_func;
	phy_collision_pre_solve_func pre_solve_func;
	phy_collision_post_solve_func post_solve_func;
	phy_collision_separate_func separate_func;
	phy_data_pointer user_data;
};

phy_space *phy_space_alloc(void);
phy_space *phy_space_new(void);
phy_space *phy_space_init(phy_space *space);

void phy_space_destroy(phy_space *space);
void phy_space_free(phy_space *space);

int phy_space_get_iterations(const phy_space *space);
void phy_space_set_iterations(phy_space *space, int iterations);

phy_vect phy_space_get_gravity(const phy_space *space);
void phy_space_set_gravity(phy_space *space, phy_vect gravity);

float phy_space_get_damping(const phy_space *space);
void phy_space_set_damping(phy_space *space, float damping);

float phy_space_get_idle_speed_threshold(const phy_space *space);
void phy_space_set_idle_speed_threshold(phy_space *space, float idle_speed_threshold);

float phy_space_get_sleep_time_threshold(const phy_space *space);
void phy_space_set_sleep_time_threshold(phy_space *space, float sleep_time_threshold);

float phy_space_get_collision_slop(const phy_space *space);
void phy_space_set_collision_slop(phy_space *space, float collision_slop);

float phy_space_get_collision_bias(const phy_space *space);
void phy_space_set_collision_bias(phy_space *space, float collision_bias);

phy_timestamp phy_space_get_collision_persistence(const phy_space *space);
void phy_space_set_collision_persistence(phy_space *space, phy_timestamp collision_persistence);

phy_data_pointer phy_space_get_user_data(const phy_space *space);
void phy_space_set_user_data(phy_space *space, phy_data_pointer user_data);

phy_body* phy_space_get_static_body(const phy_space *space);
float phy_space_get_current_time_step(const phy_space *space);

bool phy_space_is_locked(phy_space *space);

phy_collision_handler *phy_space_add_default_collision_handler(phy_space *space);
phy_collision_handler *phy_space_add_collision_handler(phy_space *space, phy_collision_type a, phy_collision_type b);
phy_collision_handler *phy_space_add_wildcard_handler(phy_space *space, phy_collision_type type);

phy_shape *phy_space_add_shape(phy_space *space, phy_shape *shape);
phy_body *phy_space_add_body(phy_space *space, phy_body *body);
phy_constraint *phy_space_add_constraint(phy_space *space, phy_constraint *constraint);

void phy_space_remove_shape(phy_space *space, phy_shape *shape);
void phy_space_remove_body(phy_space *space, phy_body *body);
void phy_space_remove_constraint(phy_space *space, phy_constraint *constraint);

bool phy_space_contains_shape(phy_space *space, phy_shape *shape);
bool phy_space_contains_body(phy_space *space, phy_body *body);
bool phy_space_contains_constraint(phy_space *space, phy_constraint *constraint);

typedef void (*phy_post_step_func)(phy_space *space, void *key, void *data);
bool phy_space_add_post_step_callback(phy_space *space, phy_post_step_func func, void *key, void *data);

typedef void (*phy_space_point_query_func)(phy_shape *shape, phy_vect point, float distance, phy_vect gradient, void *data);
void phy_space_point_query(phy_space *space, phy_vect point, float max_distance, phy_shape_filter filter, phy_space_point_query_func func, void *data);
phy_shape *phy_space_point_query_nearest(phy_space *space, phy_vect point, float max_distance, phy_shape_filter filter, phy_point_query_info *out);

typedef void (*phy_space_segment_query_func)(phy_shape *shape, phy_vect point, phy_vect normal, float alpha, void *data);
void phy_space_segment_query(phy_space *space, phy_vect start, phy_vect end, float radius, phy_shape_filter filter, phy_space_segment_query_func func, void *data);
phy_shape *phy_space_segment_query_first(phy_space *space, phy_vect start, phy_vect end, float radius, phy_shape_filter filter, phy_segment_query_info *out);

typedef void (*phy_space_BB_query_func)(phy_shape *shape, void *data);
void phy_space_BB_query(phy_space *space, phy_bb bb, phy_shape_filter filter, phy_space_BB_query_func func, void *data);

typedef void (*phy_space_shape_query_func)(phy_shape *shape, phy_contact_point_set *points, void *data);
bool phy_space_shape_query(phy_space *space, phy_shape *shape, phy_space_shape_query_func func, void *data);

typedef void (*phy_space_body_iterator_func)(phy_body *body, void *data);
void phy_space_each_body(phy_space *space, phy_space_body_iterator_func func, void *data);

typedef void (*phy_space_shape_iterator_func)(phy_shape *shape, void *data);
void phy_space_each_shape(phy_space *space, phy_space_shape_iterator_func func, void *data);

typedef void (*phy_space_constraint_iterator_func)(phy_constraint *constraint, void *data);
void phy_space_each_constraint(phy_space *space, phy_space_constraint_iterator_func func, void *data);

void phy_space_reindex_static(phy_space *space);
void phy_space_reindex_shape(phy_space *space, phy_shape *shape);
void phy_space_reindex_shapes_for_body(phy_space *space, phy_body *body);

void phy_space_use_spatial_hash(phy_space *space, float dim, int count);

void phy_space_step(phy_space *space, float dt);

