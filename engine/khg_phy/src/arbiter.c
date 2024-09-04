#include "khg_phy/phy_private.h"
#include "khg_phy/arbiter.h"	
#include "khg_utl/error_func.h"
#include <stdlib.h>

static inline void phy_unthread_helper(phy_arbiter *arb, phy_body *body) {
	struct phy_arbiter_thread *thread = phy_arbiter_thread_for_body(arb, body);
	phy_arbiter *prev = thread->prev;
	phy_arbiter *next = thread->next;
  if (prev) {
		phy_arbiter_thread_for_body(prev, body)->next = next;
	} 
  else if (body->arbiter_list == arb) {
		body->arbiter_list = next;
	}
	if (next) {
    phy_arbiter_thread_for_body(next, body)->prev = prev;
  }
	thread->prev = NULL;
	thread->next = NULL;
}

void phy_arbiter_unthread(phy_arbiter *arb) {
	phy_unthread_helper(arb, arb->body_a);
	phy_unthread_helper(arb, arb->body_b);
}

bool phy_arbiter_is_first_contact(const phy_arbiter *arb) {
	return arb->state == PHY_ARBITER_STATE_FIRST_COLLISION;
}

bool phy_arbiter_is_removal(const phy_arbiter *arb) {
	return arb->state == PHY_ARBITER_STATE_INVALIDATED;
}

int phy_arbiter_get_count(const phy_arbiter *arb) {
	return (arb->state < PHY_ARBITER_STATE_CACHED ? arb->count : 0);
}

phy_vect phy_arbiter_get_normal(const phy_arbiter *arb) {
	return phy_v_mult(arb->n, arb->swapped ? -1.0f : 1.0);
}

phy_vect phy_arbiter_get_point_A(const phy_arbiter *arb, int i) {
  if (!(0 <= i && i < phy_arbiter_get_count(arb))) {
	  utl_error_func("The specified contact index is invalid for this arbiter", utl_user_defined_data);
  }
	return phy_v_add(arb->body_a->p, arb->contacts[i].r1);
}

phy_vect phy_arbiter_get_point_B(const phy_arbiter *arb, int i) {
  if (!(0 <= i && i < phy_arbiter_get_count(arb))) {
	  utl_error_func("The specified contact index is invalid for this arbiter", utl_user_defined_data);
  }
	return phy_v_add(arb->body_b->p, arb->contacts[i].r2);
}

float phy_arbiter_get_depth(const phy_arbiter *arb, int i) {
  if (!(0 <= i && i < phy_arbiter_get_count(arb))) {
	  utl_error_func("The specified contact index is invalid for this arbiter", utl_user_defined_data);
  }
	struct phy_contact *con = &arb->contacts[i];
	return phy_v_dot(phy_v_add(phy_v_sub(con->r2, con->r1), phy_v_sub(arb->body_b->p, arb->body_a->p)), arb->n);
}

phy_contact_point_set phy_arbiter_get_contact_point_set(const phy_arbiter *arb) {
	phy_contact_point_set set;
	set.count = phy_arbiter_get_count(arb);
	bool swapped = arb->swapped;
	phy_vect n = arb->n;
	set.normal = (swapped ? phy_v_neg(n) : n);
	for (int i=0; i<set.count; i++) {
		phy_vect p1 = phy_v_add(arb->body_a->p, arb->contacts[i].r1);
		phy_vect p2 = phy_v_add(arb->body_b->p, arb->contacts[i].r2);
		set.points[i].pointA = (swapped ? p2 : p1);
		set.points[i].pointB = (swapped ? p1 : p2);
		set.points[i].distance = phy_v_dot(phy_v_sub(p2, p1), n);
	}
	return set;
}

void phy_arbiter_set_contact_point_set(phy_arbiter *arb, phy_contact_point_set *set) {
	int count = set->count;
  if (count != arb->count) {
    utl_error_func("The number of contact points cannot be changed", utl_user_defined_data);
  }
	bool swapped = arb->swapped;
	arb->n = (swapped ? phy_v_neg(set->normal) : set->normal);
	for (int i=0; i<count; i++) {
		phy_vect p1 = set->points[i].pointA;
		phy_vect p2 = set->points[i].pointB;
		arb->contacts[i].r1 = phy_v_sub(swapped ? p2 : p1, arb->body_a->p);
		arb->contacts[i].r2 = phy_v_sub(swapped ? p1 : p2, arb->body_b->p);
	}
}

phy_vect phy_arbiter_total_impulse(const phy_arbiter *arb) {
	struct phy_contact *contacts = arb->contacts;
	phy_vect n = arb->n;
	phy_vect sum = phy_v_zero;
	for (int i=0, count=phy_arbiter_get_count(arb); i<count; i++) {
		struct phy_contact *con = &contacts[i];
		sum = phy_v_add(sum, phy_v_rotate(n, phy_v(con->jn_acc, con->jt_acc)));
	}
	return (arb->swapped ? sum : phy_v_neg(sum));
	return phy_v_zero;
}

float phy_arbiter_total_ke(const phy_arbiter *arb) {
	float eCoef = (1 - arb->e)/(1 + arb->e);
	float sum = 0.0;
	struct phy_contact *contacts = arb->contacts;
	for (int i=0, count=phy_arbiter_get_count(arb); i<count; i++) {
		struct phy_contact *con = &contacts[i];
		float jnAcc = con->jn_acc;
		float jtAcc = con->jt_acc;
		sum += eCoef*jnAcc*jnAcc/con->n_mass + jtAcc*jtAcc/con->t_mass;
	}
	return sum;
}

bool phy_arbiter_ignore(phy_arbiter *arb) {
	arb->state = PHY_ARBITER_STATE_IGNORE;
	return false;
}

float phy_arbiter_get_restitution(const phy_arbiter *arb) {
	return arb->e;
}

void phy_arbiter_set_restitution(phy_arbiter *arb, float restitution) {
	arb->e = restitution;
}

float phy_arbiter_get_friction(const phy_arbiter *arb) {
	return arb->u;
}

void phy_arbiter_set_friction(phy_arbiter *arb, float friction) {
	arb->u = friction;
}

phy_vect phy_arbiter_get_surface_velocity(phy_arbiter *arb) {
	return phy_v_mult(arb->surface_vr, arb->swapped ? -1.0f : 1.0);
}

void phy_arbiter_set_surface_velocity(phy_arbiter *arb, phy_vect vr) {
	arb->surface_vr = phy_v_mult(vr, arb->swapped ? -1.0f : 1.0);
}

phy_data_pointer phy_arbiter_get_user_data(const phy_arbiter *arb) {
	return arb->data;
}

void phy_arbiter_set_user_data(phy_arbiter *arb, phy_data_pointer userData) {
	arb->data = userData;
}

void phy_arbiter_get_shapes(const phy_arbiter *arb, phy_shape **a, phy_shape **b) {
  if (arb->swapped) {
		(*a) = (phy_shape *)arb->b;
		(*b) = (phy_shape *)arb->a;
	} 
  else {
		(*a) = (phy_shape *)arb->a;
		(*b) = (phy_shape *)arb->b;
	}
}

void phy_arbiter_get_bodies(const phy_arbiter *arb, phy_body **a, phy_body **b) {
	PHY_ARBITER_GET_SHAPES(arb, shape_a, shape_b);
	(*a) = shape_a->body;
	(*b) = shape_b->body;
}

bool phy_arbiter_call_wildcard_begin_A(phy_arbiter *arb, phy_space *space) {
	phy_collision_handler *handler = arb->handler_A;
	return handler->begin_func(arb, space, handler->user_data);
}

bool phy_arbiter_call_wildcard_begin_B(phy_arbiter *arb, phy_space *space) {
	phy_collision_handler *handler = arb->handler_B;
	arb->swapped = !arb->swapped;
	bool retval = handler->begin_func(arb, space, handler->user_data);
	arb->swapped = !arb->swapped;
	return retval;
}

bool phy_arbiter_call_wildcard_pre_solve_A(phy_arbiter *arb, phy_space *space) {
	phy_collision_handler *handler = arb->handler_A;
	return handler->pre_solve_func(arb, space, handler->user_data);
}

bool phy_arbiter_call_wildcard_pre_solve_B(phy_arbiter *arb, phy_space *space) {
	phy_collision_handler *handler = arb->handler_B;
	arb->swapped = !arb->swapped;
	bool retval = handler->pre_solve_func(arb, space, handler->user_data);
	arb->swapped = !arb->swapped;
	return retval;
}

void phy_arbiter_call_wildcard_post_solve_A(phy_arbiter *arb, phy_space *space) {
	phy_collision_handler *handler = arb->handler_A;
	handler->post_solve_func(arb, space, handler->user_data);
}

void phy_arbiter_call_wildcard_post_solve_B(phy_arbiter *arb, phy_space *space) {
	phy_collision_handler *handler = arb->handler_B;
	arb->swapped = !arb->swapped;
	handler->post_solve_func(arb, space, handler->user_data);
	arb->swapped = !arb->swapped;
}

void phy_arbiter_call_wildcard_separate_A(phy_arbiter *arb, phy_space *space) {
	phy_collision_handler *handler = arb->handler_A;
	handler->separate_func(arb, space, handler->user_data);
}

void phy_arbiter_call_wildcard_separate_B(phy_arbiter *arb, phy_space *space) {
	phy_collision_handler *handler = arb->handler_B;
	arb->swapped = !arb->swapped;
	handler->separate_func(arb, space, handler->user_data);
	arb->swapped = !arb->swapped;
}

phy_arbiter *phy_arbiter_init(phy_arbiter *arb, phy_shape *a, phy_shape *b) {
	arb->handler = NULL;
	arb->swapped = false;
	arb->handler = NULL;
	arb->handler_A = NULL;
	arb->handler_B = NULL;
	arb->e = 0.0f;
	arb->u = 0.0f;
	arb->surface_vr = phy_v_zero;
	arb->count = 0;
	arb->contacts = NULL;
	arb->a = a; arb->body_a = a->body;
	arb->b = b; arb->body_b = b->body;
	arb->thread_A.next = NULL;
	arb->thread_B.next = NULL;
	arb->thread_A.prev = NULL;
	arb->thread_B.prev = NULL;
	arb->stamp = 0;
	arb->state = PHY_ARBITER_STATE_FIRST_COLLISION;
	arb->data = NULL;
	return arb;
}

static inline phy_collision_handler *phy_space_lookup_handler(phy_space *space, phy_collision_type a, phy_collision_type b, phy_collision_handler *defaultValue) {
	phy_collision_type types[] = {a, b};
	phy_collision_handler *handler = (phy_collision_handler *)phy_hash_set_find(space->collision_handlers, PHY_HASH_PAIR(a, b), types);
	return (handler ? handler : defaultValue);
}

void phy_arbiter_update(phy_arbiter *arb, struct phy_collision_info *info, phy_space *space) {
	const phy_shape *a = info->a, *b = info->b;
	arb->a = a; arb->body_a = a->body;
	arb->b = b; arb->body_b = b->body;
	for(int i=0; i<info->count; i++) {
		struct phy_contact *con = &info->arr[i];
		con->r1 = phy_v_sub(con->r1, a->body->p);
		con->r2 = phy_v_sub(con->r2, b->body->p);
		con->jn_acc = con->jt_acc = 0.0f;
		for(int j=0; j<arb->count; j++){
			struct phy_contact *old = &arb->contacts[j];
			if(con->hash == old->hash){
				con->jn_acc = old->jn_acc;
				con->jt_acc = old->jt_acc;
			}
		}
	}
	arb->contacts = info->arr;
	arb->count = info->count;
	arb->n = info->n;
	arb->e = a->e * b->e;
	arb->u = a->u * b->u;
	phy_vect surface_vr = phy_v_sub(b->surfaceV, a->surfaceV);
	arb->surface_vr = phy_v_sub(surface_vr, phy_v_mult(info->n, phy_v_dot(surface_vr, info->n)));
	phy_collision_type typeA = info->a->type, typeB = info->b->type;
	phy_collision_handler *defaultHandler = &space->default_handler;
	phy_collision_handler *handler = arb->handler = phy_space_lookup_handler(space, typeA, typeB, defaultHandler);
	bool swapped = arb->swapped = (typeA != handler->type_A && handler->type_A != PHY_WILDCARD_COLLISION_TYPE);
	if(handler != defaultHandler || space->uses_wildcards){
		arb->handler_A = phy_space_lookup_handler(space, (swapped ? typeB : typeA), PHY_WILDCARD_COLLISION_TYPE, &phy_collision_handler_do_nothing);
		arb->handler_B = phy_space_lookup_handler(space, (swapped ? typeA : typeB), PHY_WILDCARD_COLLISION_TYPE, &phy_collision_handler_do_nothing);
	}
	if(arb->state == PHY_ARBITER_STATE_CACHED) arb->state = PHY_ARBITER_STATE_FIRST_COLLISION;
}

void phy_arbiter_pre_step(phy_arbiter *arb, float dt, float slop, float bias) {
	phy_body *a = arb->body_a;
	phy_body *b = arb->body_b;
	phy_vect n = arb->n;
	phy_vect body_delta = phy_v_sub(b->p, a->p);
	for(int i=0; i<arb->count; i++){
		struct phy_contact *con = &arb->contacts[i];
		con->n_mass = 1.0f/phy_k_scalar(a, b, con->r1, con->r2, n);
		con->t_mass = 1.0f/phy_k_scalar(a, b, con->r1, con->r2, phy_v_perp(n));
		float dist = phy_v_dot(phy_v_add(phy_v_sub(con->r2, con->r1), body_delta), n);
		con->bias = -bias*phy_min(0.0f, dist + slop)/dt;
		con->j_bias = 0.0f;
		con->bounce = phy_normal_relative_velocity(a, b, con->r1, con->r2, n)*arb->e;
	}
}

void phy_arbiter_apply_cached_impulse(phy_arbiter *arb, float dt_coef) {
	if(phy_arbiter_is_first_contact(arb)) {
    return;
  }
	phy_body *a = arb->body_a;
	phy_body *b = arb->body_b;
	phy_vect n = arb->n;
	for(int i=0; i<arb->count; i++){
		struct phy_contact *con = &arb->contacts[i];
		phy_vect j = phy_v_rotate(n, phy_v(con->jn_acc, con->jt_acc));
		phy_apply_impulses(a, b, con->r1, con->r2, phy_v_mult(j, dt_coef));
	}
}

void phy_arbiter_apply_impulse(phy_arbiter *arb) {
	phy_body *a = arb->body_a;
	phy_body *b = arb->body_b;
	phy_vect n = arb->n;
	phy_vect surface_vr = arb->surface_vr;
	float friction = arb->u;
	for(int i=0; i<arb->count; i++){
		struct phy_contact *con = &arb->contacts[i];
		float nMass = con->n_mass;
		phy_vect r1 = con->r1;
		phy_vect r2 = con->r2;
		phy_vect vb1 = phy_v_add(a->v_bias, phy_v_mult(phy_v_perp(r1), a->w_bias));
		phy_vect vb2 = phy_v_add(b->v_bias, phy_v_mult(phy_v_perp(r2), b->w_bias));
		phy_vect vr = phy_v_add(phy_relative_velocity(a, b, r1, r2), surface_vr);
		float vbn = phy_v_dot(phy_v_sub(vb2, vb1), n);
		float vrn = phy_v_dot(vr, n);
		float vrt = phy_v_dot(vr, phy_v_perp(n));
		float jbn = (con->bias - vbn)*nMass;
		float jbnOld = con->j_bias;
		con->j_bias = phy_max(jbnOld + jbn, 0.0f);
		float jn = -(con->bounce + vrn)*nMass;
		float jnOld = con->jn_acc;
		con->jn_acc = phy_max(jnOld + jn, 0.0f);
		float jtMax = friction*con->jn_acc;
		float jt = -vrt*con->t_mass;
		float jtOld = con->jt_acc;
		con->jt_acc = phy_clamp(jtOld + jt, -jtMax, jtMax);
		phy_apply_bias_impulses(a, b, r1, r2, phy_v_mult(n, con->j_bias - jbnOld));
		phy_apply_impulses(a, b, r1, r2, phy_v_rotate(n, phy_v(con->jn_acc - jnOld, con->jt_acc - jtOld)));
	}
}

