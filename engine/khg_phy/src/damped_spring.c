#include "khg_phy/phy_private.h"
#include "khg_phy/transform.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

static float
defaultSpringForce(phy_damped_spring *spring, float dist){
	return (spring->rest_length - dist)*spring->stiffness;
}

static void
preStep(phy_damped_spring *spring, float dt)
{
	phy_body *a = spring->constraint.a;
	phy_body *b = spring->constraint.b;
	
	spring->r1 = phy_transform_vect(a->transform, phy_v_sub(spring->anchor_A, a->cog));
	spring->r2 = phy_transform_vect(b->transform, phy_v_sub(spring->anchor_B, b->cog));
	
	phy_vect delta = phy_v_sub(phy_v_add(b->p, spring->r2), phy_v_add(a->p, spring->r1));
	float dist = phy_v_length(delta);
	spring->n = phy_v_mult(delta, 1.0f/(dist ? dist : INFINITY));
	
	float k = phy_k_scalar(a, b, spring->r1, spring->r2, spring->n);
	if (k == 0.0) {
    utl_error_func("Unsolvable spring", utl_user_defined_data);
  }
	spring->n_mass = 1.0f/k;
	
	spring->target_vrn = 0.0f;
	spring->v_coef = 1.0f - expf(-spring->damping*dt*k);

	// apply spring force
	float f_spring = spring->spring_force_func((phy_constraint *)spring, dist);
	float j_spring = spring->j_acc = f_spring*dt;
	phy_apply_impulses(a, b, spring->r1, spring->r2, phy_v_mult(spring->n, j_spring));
}

static void applyCachedImpulse(phy_damped_spring *spring, float dt_coef){}

static void
applyImpulse(phy_damped_spring *spring, float dt)
{
	phy_body *a = spring->constraint.a;
	phy_body *b = spring->constraint.b;
	
	phy_vect n = spring->n;
	phy_vect r1 = spring->r1;
	phy_vect r2 = spring->r2;

	// compute relative velocity
	float vrn = phy_normal_relative_velocity(a, b, r1, r2, n);
	
	// compute velocity loss from drag
	float v_damp = (spring->target_vrn - vrn)*spring->v_coef;
	spring->target_vrn = vrn + v_damp;
	
	float j_damp = v_damp*spring->n_mass;
	spring->j_acc += j_damp;
	phy_apply_impulses(a, b, spring->r1, spring->r2, phy_v_mult(spring->n, j_damp));
}

static float
getImpulse(phy_damped_spring *spring)
{
	return spring->j_acc;
}

static const phy_constraint_class klass = {
	(phy_constraint_pre_step_impl)preStep,
	(phy_constraint_apply_cached_impulse_impl)applyCachedImpulse,
	(phy_constraint_apply_impulse_impl)applyImpulse,
	(phy_constraint_get_impulse_impl)getImpulse,
};

phy_damped_spring *
phy_damped_spring_alloc(void)
{
	return (phy_damped_spring *)calloc(1, sizeof(phy_damped_spring));
}

phy_damped_spring *
phy_damped_spring_init(phy_damped_spring *spring, phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB, float restLength, float stiffness, float damping)
{
	cp_constraint_init((phy_constraint *)spring, &klass, a, b);
	
	spring->anchor_A = anchorA;
	spring->anchor_B = anchorB;
	
	spring->rest_length = restLength;
	spring->stiffness = stiffness;
	spring->damping = damping;
	spring->spring_force_func = (phy_damped_spring_force_func)defaultSpringForce;
	
	spring->j_acc = 0.0f;
	
	return spring;
}

phy_constraint *
phy_damped_spring_new(phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB, float restLength, float stiffness, float damping)
{
	return (phy_constraint *)phy_damped_spring_init(phy_damped_spring_alloc(), a, b, anchorA, anchorB, restLength, stiffness, damping);
}

bool
phy_constraint_is_damped_spring(const phy_constraint *constraint)
{
	return (constraint->class == &klass);
}

phy_vect
phy_damped_spring_get_anchor_A(const phy_constraint *constraint)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	return ((phy_damped_spring *)constraint)->anchor_A;
}

void
phy_damped_spring_set_anchor_A(phy_constraint *constraint, phy_vect anchorA)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	phy_constraint_activate_bodies(constraint);
	((phy_damped_spring *)constraint)->anchor_A = anchorA;
}

phy_vect
phy_damped_spring_get_anchor_B(const phy_constraint *constraint)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	return ((phy_damped_spring *)constraint)->anchor_B;
}

void
phy_damped_spring_set_anchor_B(phy_constraint *constraint, phy_vect anchorB)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	phy_constraint_activate_bodies(constraint);
	((phy_damped_spring *)constraint)->anchor_B = anchorB;
}

float
phy_damped_spring_get_rest_length(const phy_constraint *constraint)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	return ((phy_damped_spring *)constraint)->rest_length;
}

void
phy_damped_spring_set_rest_length(phy_constraint *constraint, float restLength)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	phy_constraint_activate_bodies(constraint);
	((phy_damped_spring *)constraint)->rest_length = restLength;
}

float
phy_damped_spring_get_stiffness(const phy_constraint *constraint)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	return ((phy_damped_spring *)constraint)->stiffness;
}

void
phy_damped_spring_set_stiffness(phy_constraint *constraint, float stiffness)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	phy_constraint_activate_bodies(constraint);
	((phy_damped_spring *)constraint)->stiffness = stiffness;
}

float
phy_damped_spring_get_damping(const phy_constraint *constraint)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	return ((phy_damped_spring *)constraint)->damping;
}

void
phy_damped_spring_set_damping(phy_constraint *constraint, float damping)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	phy_constraint_activate_bodies(constraint);
	((phy_damped_spring *)constraint)->damping = damping;
}

phy_damped_spring_force_func
phy_damped_spring_get_spring_force_func(const phy_constraint *constraint)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	return ((phy_damped_spring *)constraint)->spring_force_func;
}

void
phy_damped_spring_set_spring_force_func(phy_constraint *constraint, phy_damped_spring_force_func springForceFunc)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	phy_constraint_activate_bodies(constraint);
	((phy_damped_spring *)constraint)->spring_force_func = springForceFunc;
}
