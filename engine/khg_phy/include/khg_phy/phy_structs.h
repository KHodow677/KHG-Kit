#pragma once

#include "khg_phy/bb.h"
#include "khg_phy/body.h"
#include "khg_phy/constraint.h"
#include "khg_phy/damped_spring.h"
#include "khg_phy/damped_rotary_spring.h"
#include "khg_phy/shape.h"
#include "khg_phy/space.h"
#include "khg_phy/spatial_index.h"

#define PHY_POLY_SHAPE_INLINE_ALLOC 6

struct phy_array {
	int num, max;
	void **arr;
};

struct phy_body {
	phy_body_velocity_func velocity_func;
	phy_body_position_func position_func;
	float m;
	float m_inv;
	float i;
	float i_inv;
	phy_vect cog;
	phy_vect p;
	phy_vect v;
	phy_vect f;
	float a;
	float w;
	float t;
	phy_transform transform;
	phy_data_pointer user_data;
	phy_vect v_bias;
	float w_bias;
	phy_space *space;
	phy_shape *shape_list;
	phy_arbiter *arbiter_list;
	phy_constraint *constraint_list;
	struct {
		phy_body *root;
		phy_body *next;
		float idle_time;
	} sleeping;
};

enum phy_arbiter_state {
	PHY_ARBITER_STATE_FIRST_COLLISION,
	PHY_ARBITER_STATE_NORMAL,
	PHY_ARBITER_STATE_IGNORE,
	PHY_ARBITER_STATE_CACHED,
	PHY_ARBITER_STATE_INVALIDATED,
};

struct phy_arbiter_thread {
	struct phy_arbiter *next, *prev;
};

struct phy_contact {
	phy_vect r1, r2;
	float n_mass, t_mass;
	float bounce;
	float jn_acc, jt_acc, j_bias;
	float bias;
	phy_hash_value hash;
};

struct phy_collision_info {
	const phy_shape *a, *b;
	phy_collision_id id;
	phy_vect n;
	int count;
	struct phy_contact *arr;
};

struct phy_arbiter {
	float e;
	float u;
	phy_vect surface_vr;
	phy_data_pointer data;
	const phy_shape *a, *b;
	phy_body *body_a, *body_b;
	struct phy_arbiter_thread thread_A, thread_B;
	int count;
	struct phy_contact *contacts;
	phy_vect n;
	phy_collision_handler *handler, *handler_A, *handler_B;
	bool swapped;
	phy_timestamp stamp;
	enum phy_arbiter_state state;
};

struct phy_shape_mass_info {
	float m;
	float i;
	phy_vect cog;
	float area;
};

typedef enum phy_shape_type{
	PHY_CIRCLE_SHAPE,
	PHY_SEGMENT_SHAPE,
	PHY_POLY_SHAPE,
	PHY_NUM_SHAPES
} phy_shape_type;

typedef phy_bb (*phy_shape_cache_data_impl)(phy_shape *shape, phy_transform transform);
typedef void (*phy_shape_destroy_impl)(phy_shape *shape);
typedef void (*phy_shape_point_query_impl)(const phy_shape *shape, phy_vect p, phy_point_query_info *info);
typedef void (*phy_shape_segment_query_impl)(const phy_shape *shape, phy_vect a, phy_vect b, float radius, phy_segment_query_info *info);

typedef struct phy_shape_class {
	phy_shape_type type;
	phy_shape_cache_data_impl cache_data;
	phy_shape_destroy_impl destroy;
	phy_shape_point_query_impl point_query;
	phy_shape_segment_query_impl segment_query;
} phy_shape_class;

struct phy_shape {
	const phy_shape_class *class;
	phy_space *space;
	phy_body *body;
	struct phy_shape_mass_info massInfo;
	phy_bb bb;
	bool sensor;
	float e;
	float u;
	phy_vect surfaceV;
	phy_data_pointer userData;
	phy_collision_type type;
	phy_shape_filter filter;
	phy_shape *next;
	phy_shape *prev;
	phy_hash_value hashid;
};

struct phy_circle_shape {
	phy_shape shape;
	phy_vect c, tc;
	float r;
};

struct phy_segment_shape {
	phy_shape shape;
	phy_vect a, b, n;
	phy_vect ta, tb, tn;
	float r;
	phy_vect a_tangent, b_tangent;
};

struct phy_splitting_plane {
	phy_vect v0, n;
};

struct phy_poly_shape {
	phy_shape shape;
	float r;
	int count;
	struct phy_splitting_plane *planes;
	struct phy_splitting_plane fixed_planes[2 * PHY_POLY_SHAPE_INLINE_ALLOC];
};

typedef void (*phy_constraint_pre_step_impl)(phy_constraint *constraint, float dt);
typedef void (*phy_constraint_apply_cached_impulse_impl)(phy_constraint *constraint, float dt_coef);
typedef void (*phy_constraint_apply_impulse_impl)(phy_constraint *constraint, float dt);
typedef float (*phy_constraint_get_impulse_impl)(phy_constraint *constraint);

typedef struct phy_constraint_class {
	phy_constraint_pre_step_impl pre_step;
	phy_constraint_apply_cached_impulse_impl apply_cached_impulse;
	phy_constraint_apply_impulse_impl apply_impulse;
	phy_constraint_get_impulse_impl get_impulse;
} phy_constraint_class;

struct phy_constraint {
	const phy_constraint_class *class;
	phy_space *space;
	phy_body *a, *b;
	phy_constraint *next_a, *next_b;
	float max_force;
	float error_bias;
	float max_bias;
	bool collide_bodies;
	phy_constraint_pre_solve_func pre_solve;
	phy_constraint_post_solve_func post_solve;
	phy_data_pointer user_data;
};

struct phy_pin_joint {
	phy_constraint constraint;
	phy_vect anchor_A, anchor_B;
	float dist;
	phy_vect r1, r2;
	phy_vect n;
	float n_mass;
	float jn_acc;
	float bias;
};

struct phy_slide_joint {
	phy_constraint constraint;
	phy_vect anchor_A, anchor_B;
	float min, max;
	phy_vect r1, r2;
	phy_vect n;
	float n_mass;
	float jn_acc;
	float bias;
};

struct phy_pivot_joint {
	phy_constraint constraint;
	phy_vect anchor_A, anchor_B;
	phy_vect r1, r2;
	phy_mat2x2 k;
	phy_vect j_acc;
	phy_vect bias;
};

struct phy_groove_joint {
	phy_constraint constraint;
	phy_vect grv_n, grv_a, grv_b;
	phy_vect anchor_B;
	phy_vect grv_tn;
	float clamp;
	phy_vect r1, r2;
	phy_mat2x2 k;
	phy_vect j_acc;
	phy_vect bias;
};

struct phy_damped_spring {
	phy_constraint constraint;
	phy_vect anchor_A, anchor_B;
	float rest_length;
	float stiffness;
	float damping;
	phy_damped_spring_force_func spring_force_func;
	float target_vrn;
	float v_coef;
	phy_vect r1, r2;
	float n_mass;
	phy_vect n;
	float j_acc;
};

struct phy_damped_rotary_spring {
	phy_constraint constraint;
	float rest_angle;
	float stiffness;
	float damping;
	phy_damped_rotary_spring_torque_func spring_torque_func;
	float target_wrn;
	float w_coef;
	float i_sum;
	float j_acc;
};

struct rotary_limit_joint {
	phy_constraint constraint;
	float min, max;
	float i_sum;
	float bias;
	float j_acc;
};

struct phy_ratchet_joint {
	phy_constraint constraint;
	float angle, phase, ratchet;
	float i_sum;
	float bias;
	float j_acc;
};

struct phy_gear_joint {
	phy_constraint constraint;
	float phase, ratio;
	float ratio_inv;
	float i_sum;
	float bias;
	float j_acc;
};

struct phy_simple_motor_joint {
	phy_constraint constraint;
	float rate;
	float i_sum;
	float j_acc;
};

typedef struct phy_contact_buffer_header phy_contact_buffer_header;
typedef void (*phy_space_arbiter_apply_impulse_func)(phy_arbiter *arb);

struct phy_space {
	int iterations;
	phy_vect gravity;
	float damping;
	float idle_speed_threshold;
	float sleep_time_threshold;
	float collision_slop;
	float collision_bias;
	phy_timestamp collision_persistence;
	phy_data_pointer user_data;
	phy_timestamp stamp;
	float curr_dt;
	phy_array *dynamic_bodies;
	phy_array *static_bodies;
	phy_array *roused_bodies;
	phy_array *sleeping_components;
	phy_hash_value shape_id_Counter;
	phy_spatial_index *static_shapes;
	phy_spatial_index *dynamic_shapes;
	phy_array *constraints;
	phy_array *arbiters;
	phy_contact_buffer_header *contact_buffers_head;
	phy_hash_set *cached_arbiters;
	phy_array *pooled_arbiters;
	phy_array *allocated_buffers;
	int locked;
	bool uses_wildcards;
	phy_hash_set *collision_handlers;
	phy_collision_handler default_handler;
	bool skip_post_step;
	phy_array *post_step_callbacks;
	phy_body *static_body;
	phy_body fixed_static_body;
};

typedef struct phy_post_step_callback {
	phy_post_step_func func;
	void *key;
	void *data;
} phy_post_step_callback;

