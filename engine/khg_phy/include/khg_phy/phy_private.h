#pragma once

#include "khg_phy/phy_structs.h"
#include "khg_utl/error_func.h"

#define PHY_HASH_COEF (3344921057ul)
#define PHY_HASH_PAIR(A, B) ((phy_hash_value)(A)*PHY_HASH_COEF ^ (phy_hash_value)(B)*PHY_HASH_COEF)
#define PHY_EPSILON 1e-5

extern phy_collision_handler phy_collision_handler_do_nothing;

phy_array *phy_array_new(int size);
void phy_array_free(phy_array *arr);
void phy_array_free_each(phy_array *arr, void (free_func)(void*));

void phy_array_push(phy_array *arr, void *object);
void *phy_array_pop(phy_array *arr);
void phy_array_delete_obj(phy_array *arr, void *obj);
bool phy_array_contains(phy_array *arr, void *ptr);

typedef bool (*phy_hash_set_eql_func)(const void *ptr, const void *elt);
typedef void *(*phy_hash_set_trans_func)(const void *ptr, void *data);

phy_hash_set *cp_hash_set_new(int size, phy_hash_set_eql_func eql_func);
void phy_hash_set_free(phy_hash_set *set);
void phy_hash_set_set__default_value(phy_hash_set *set, void *default_value);

int phy_hash_set_count(phy_hash_set *set);
const void *phy_hash_set_insert(phy_hash_set *set, phy_hash_value hash, const void *ptr, phy_hash_set_trans_func trans, void *data);
const void *phy_hash_set_remove(phy_hash_set *set, phy_hash_value hash, const void *ptr);
const void *phy_hash_set_find(phy_hash_set *set, phy_hash_value hash, const void *ptr);

typedef void (*phy_hash_set_iterator_func)(void *elt, void *data);
void phy_hash_set_each(phy_hash_set *set, phy_hash_set_iterator_func func, void *data);

typedef bool (*phy_hash_set_filter_func)(void *elt, void *data);
void phy_hash_set_filter(phy_hash_set *set, phy_hash_set_filter_func func, void *data);

void phy_body_add_shape(phy_body *body, phy_shape *shape);
void phy_body_remove_shape(phy_body *body, phy_shape *shape);

void phy_body_accumulate_mass_from_shapes(phy_body *body);

void phy_body_remove_constraint(phy_body *body, phy_constraint *constraint);

cpSpatialIndex *phy_spatial_index_init(cpSpatialIndex *index, cpSpatialIndexClass *klass, cpSpatialIndexBBFunc bbfunc, cpSpatialIndex *static_index);

phy_arbiter* phy_arbiter_init(phy_arbiter *arb, phy_shape *a, phy_shape *b);

static inline struct phy_arbiter_thread *phy_arbiter_thread_for_body(phy_arbiter *arb, phy_body *body) {
	return (arb->body_a == body ? &arb->thread_A : &arb->thread_B);
}
void phy_arbiter_unthread(phy_arbiter *arb);

void phy_arbiter_update(phy_arbiter *arb, struct phy_collision_info *info, phy_space *space);
void phy_arbiter_pre_step(phy_arbiter *arb, float dt, float bias, float slop);
void phy_arbiter_apply_cached_impulse(phy_arbiter *arb, float dt_coef);
void phy_arbiter_apply_impulse(phy_arbiter *arb);

phy_shape *phy_shape_init(phy_shape *shape, const phy_shape_class *klass, phy_body *body, struct phy_shape_mass_info mass_info);

static inline bool phy_shape_active(phy_shape *shape) {
	return (shape->prev || (shape->body && shape->body->shape_list == shape));
}

struct phy_collision_info phy_collide(const phy_shape *a, const phy_shape *b, phy_collision_id id, struct phy_contact *contacts);

static inline void phy_circle_segment_query(phy_shape *shape, phy_vect center, float r1, phy_vect a, phy_vect b, float r2, cpSegmentQueryInfo *info) {
	phy_vect da = cpvsub(a, center);
	phy_vect db = cpvsub(b, center);
	float rsum = r1 + r2;
	float qa = cpvdot(da, da) - 2.0f*cpvdot(da, db) + cpvdot(db, db);
	float qb = cpvdot(da, db) - cpvdot(da, da);
	float det = qb*qb - qa*(cpvdot(da, da) - rsum*rsum);
	if(det >= 0.0f){
		float t = (-qb - sqrtf(det))/(qa);
		if(0.0f<= t && t <= 1.0f){
			phy_vect n = cpvnormalize(cpvlerp(da, db, t));
			info->shape = shape;
			info->point = cpvsub(cpvlerp(a, b, t), cpvmult(n, r2));
			info->normal = n;
			info->alpha = t;
		}
	}
}

static inline bool phy_shape_filter_reject(cpShapeFilter a, cpShapeFilter b) {
	return ( (a.group != 0 && a.group == b.group) || (a.categories & b.mask) == 0 || (b.categories & a.mask) == 0);
}

void cp_loop_indexes(const phy_vect *verts, int count, int *start, int *end);

void cp_constraint_init(phy_constraint *constraint, const phy_constraint_class *klass, phy_body *a, phy_body *b);

static inline void phy_constraint_activate_bodies(phy_constraint *constraint) {
	phy_body *a = constraint->a; phy_body_activate(a);
	phy_body *b = constraint->b; phy_body_activate(b);
}

static inline phy_vect phy_relative_velocity(phy_body *a, phy_body *b, phy_vect r1, phy_vect r2) {
	phy_vect v1_sum = cpvadd(a->v, cpvmult(cpvperp(r1), a->w));
	phy_vect v2_sum = cpvadd(b->v, cpvmult(cpvperp(r2), b->w));
	return cpvsub(v2_sum, v1_sum);
}

static inline float phy_normal_relative_velocity(phy_body *a, phy_body *b, phy_vect r1, phy_vect r2, phy_vect n){
	return cpvdot(phy_relative_velocity(a, b, r1, r2), n);
}

static inline void phy_apply_impulse(phy_body *body, phy_vect j, phy_vect r){
	body->v = cpvadd(body->v, cpvmult(j, body->m_inv));
	body->w += body->i_inv*cpvcross(r, j);
}

static inline void phy_apply_impulses(phy_body *a , phy_body *b, phy_vect r1, phy_vect r2, phy_vect j) {
	phy_apply_impulse(a, cpvneg(j), r1);
	phy_apply_impulse(b, j, r2);
}

static inline void phy_apply_bias_impulse(phy_body *body, phy_vect j, phy_vect r) {
	body->v_bias = cpvadd(body->v_bias, cpvmult(j, body->m_inv));
	body->w_bias += body->i_inv*cpvcross(r, j);
}

static inline void phy_apply_bias_impulses(phy_body *a , phy_body *b, phy_vect r1, phy_vect r2, phy_vect j) {
	phy_apply_bias_impulse(a, cpvneg(j), r1);
	phy_apply_bias_impulse(b, j, r2);
}

static inline float phy_k_scalar_body(phy_body *body, phy_vect r, phy_vect n) {
	float rcn = cpvcross(r, n);
	return body->m_inv + body->i_inv*rcn*rcn;
}

static inline float phy_k_scalar(phy_body *a, phy_body *b, phy_vect r1, phy_vect r2, phy_vect n) {
	float value = phy_k_scalar_body(a, r1, n) + phy_k_scalar_body(b, r2, n);
  if (value == 0.0) {
    utl_error_func("Unsolvable collision or constraint", utl_user_defined_data);
  }
	return value;
}

static inline phy_mat2x2 phy_k_tensor(phy_body *a, phy_body *b, phy_vect r1, phy_vect r2) {
	float m_sum = a->m_inv + b->m_inv;
	float k11 = m_sum, k12 = 0.0f;
	float k21 = 0.0f,  k22 = m_sum;
	float a_i_inv = a->i_inv;
	float r1xsq =  r1.x * r1.x * a_i_inv;
	float r1ysq =  r1.y * r1.y * a_i_inv;
	float r1nxy = -r1.x * r1.y * a_i_inv;
	k11 += r1ysq; k12 += r1nxy;
	k21 += r1nxy; k22 += r1xsq;
	float b_i_inv = b->i_inv;
	float r2xsq =  r2.x * r2.x * b_i_inv;
	float r2ysq =  r2.y * r2.y * b_i_inv;
	float r2nxy = -r2.x * r2.y * b_i_inv;
	k11 += r2ysq; k12 += r2nxy;
	k21 += r2nxy; k22 += r2xsq;
	float det = k11*k22 - k12*k21;
  if (det == 0.0) {
    utl_error_func("Unsolvable constraint", utl_user_defined_data);  
  }
	float det_inv = 1.0f/det;
	return cpMat2x2New(k22*det_inv, -k12*det_inv, -k21*det_inv,  k11*det_inv);
}

static inline float phy_bias_coef(float error_bias, float dt) {
	return 1.0f - powf(error_bias, dt);
}

void phy_space_set_static_body(phy_space *space, phy_body *body);
void phy_space_process_components(phy_space *space, float dt);

void phy_space_push_fresh_contact_buffer(phy_space *space);
struct phy_contact *phy_contact_buffer_get_array(phy_space *space);
void phy_space_push_contacts(phy_space *space, int count);

phy_post_step_callback *phy_space_get_post_step_callback(phy_space *space, void *key);

bool phy_space_arbiter_set_filter(phy_arbiter *arb, phy_space *space);
void phy_space_filter_arbiters(phy_space *space, phy_body *body, phy_shape *filter);

void phy_space_activate_body(phy_space *space, phy_body *body);
void phy_space_lock(phy_space *space);
void phy_space_unlock(phy_space *space, bool runPostStep);

static inline void phy_space_uncache_arbiter(phy_space *space, phy_arbiter *arb) {
	const phy_shape *a = arb->a, *b = arb->b;
	const phy_shape *shape_pair[] = {a, b};
	phy_hash_value arbHashID = PHY_HASH_PAIR((phy_hash_value)a, (phy_hash_value)b);
	phy_hash_set_remove(space->cached_arbiters, arbHashID, shape_pair);
	phy_array_delete_obj(space->arbiters, arb);
}

static inline phy_array *phy_space_array_for_body_type(phy_space *space, phy_body_type type) {
	return (type == PHY_BODY_TYPE_STATIC ? space->static_bodies : space->dynamic_bodies);
}

void phy_shape_update_func(phy_shape *shape, void *unused);
phy_collision_id phy_space_collide_shapes(phy_shape *a, phy_shape *b, phy_collision_id id, phy_space *space);

static inline phy_constraint *phy_constraint_next(phy_constraint *node, phy_body *body) {
	return (node->a == body ? node->next_a : node->next_b);
}

static inline phy_arbiter *phy_arbiter_next(phy_arbiter *node, phy_body *body) {
	return (node->body_a == body ? node->thread_A.next : node->thread_B.next);
}

#define PHY_BODY_FOREACH_CONSTRAINT(bdy, var) for(phy_constraint *var = bdy->constraint_list; var; var = phy_constraint_next(var, bdy))
#define PHY_BODY_FOREACH_ARBITER(bdy, var)	for(phy_arbiter *var = bdy->arbiter_list; var; var = phy_arbiter_next(var, bdy))
#define PHY_BODY_FOREACH_SHAPE(body, var) for(phy_shape *var = body->shape_list; var; var = var->next)
#define PHY_BODY_FOREACH_COMPONENT(root, var) for(phy_body *var = root; var; var = var->sleeping.next)

