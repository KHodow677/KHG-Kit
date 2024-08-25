#include "khg_phy/phy_private.h"
#include "khg_phy/phy_structs.h"
#include "khg_phy/transform.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

static void
preStep(phy_pivot_joint *joint, float dt)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	joint->r1 = phy_transform_vect(a->transform, phy_v_sub(joint->anchor_A, a->cog));
	joint->r2 = phy_transform_vect(b->transform, phy_v_sub(joint->anchor_B, b->cog));
	
	// Calculate mass tensor
	joint-> k = phy_k_tensor(a, b, joint->r1, joint->r2);
	
	// calculate bias velocity
	phy_vect delta = phy_v_sub(phy_v_add(b->p, joint->r2), phy_v_add(a->p, joint->r1));
	joint->bias = phy_v_clamp(phy_v_mult(delta, -phy_bias_coef(joint->constraint.error_bias, dt)/dt), joint->constraint.max_bias);
}

static void
applyCachedImpulse(phy_pivot_joint *joint, float dt_coef)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	phy_apply_impulses(a, b, joint->r1, joint->r2, phy_v_mult(joint->j_acc, dt_coef));
}

static void
applyImpulse(phy_pivot_joint *joint, float dt)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	phy_vect r1 = joint->r1;
	phy_vect r2 = joint->r2;
		
	// compute relative velocity
	phy_vect vr = phy_relative_velocity(a, b, r1, r2);
	
	// compute normal impulse
	phy_vect j = phy_mat2x2_transform(joint->k, phy_v_sub(joint->bias, vr));
	phy_vect jOld = joint->j_acc;
	joint->j_acc = phy_v_clamp(phy_v_add(joint->j_acc, j), joint->constraint.max_force*dt);
	j = phy_v_sub(joint->j_acc, jOld);
	
	// apply impulse
	phy_apply_impulses(a, b, joint->r1, joint->r2, j);
}

static float
getImpulse(phy_constraint *joint)
{
	return phy_v_length(((phy_pivot_joint *)joint)->j_acc);
}

static const phy_constraint_class klass = {
	(phy_constraint_pre_step_impl)preStep,
	(phy_constraint_apply_cached_impulse_impl)applyCachedImpulse,
	(phy_constraint_apply_impulse_impl)applyImpulse,
	(phy_constraint_get_impulse_impl)getImpulse,
};

phy_pivot_joint *
phy_pivot_joint_alloc(void)
{
	return (phy_pivot_joint *)calloc(1, sizeof(phy_pivot_joint));
}

phy_pivot_joint *
phy_pivot_joint_init(phy_pivot_joint *joint, phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB)
{
	cp_constraint_init((phy_constraint *)joint, &klass, a, b);
	
	joint->anchor_A = anchorA;
	joint->anchor_B = anchorB;
	
	joint->j_acc = phy_v_zero;
	
	return joint;
}

phy_constraint *
phy_pivot_joint_new_2(phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB)
{
	return (phy_constraint *)phy_pivot_joint_init(phy_pivot_joint_alloc(), a, b, anchorA, anchorB);
}

phy_constraint *
phy_pivot_joint_new(phy_body *a, phy_body *b, phy_vect pivot)
{
	phy_vect anchorA = (a ? phy_body_world_to_local(a, pivot) : pivot);
	phy_vect anchorB = (b ? phy_body_world_to_local(b, pivot) : pivot);
	return phy_pivot_joint_new_2(a, b, anchorA, anchorB);
}

bool
phy_constraint_is_pivot_joint(const phy_constraint *constraint)
{
	return (constraint->class == &klass);
}

phy_vect
phy_pivot_joint_get_anchor_A(const phy_constraint *constraint)
{
	if (!phy_constraint_is_pivot_joint(constraint)) {
    utl_error_func("Constraint is not a pivot joint", utl_user_defined_data);
  }
	return ((phy_pivot_joint *)constraint)->anchor_A;
}

void
phy_pivot_joint_set_anchor_A(phy_constraint *constraint, phy_vect anchorA)
{
	if (!phy_constraint_is_pivot_joint(constraint)) {
    utl_error_func("Constraint is not a pivot joint", utl_user_defined_data);
  }
	phy_constraint_activate_bodies(constraint);
	((phy_pivot_joint *)constraint)->anchor_A = anchorA;
}

phy_vect
phy_pivot_joint_get_anchor_B(const phy_constraint *constraint)
{
	if (!phy_constraint_is_pivot_joint(constraint)) {
    utl_error_func("Constraint is not a pivot joint", utl_user_defined_data);
  }
	return ((phy_pivot_joint *)constraint)->anchor_B;
}

void
phy_pivot_joint_set_anchor_B(phy_constraint *constraint, phy_vect anchorB)
{
	if (!phy_constraint_is_pivot_joint(constraint)) {
    utl_error_func("Constraint is not a pivot joint", utl_user_defined_data);
  }
	phy_constraint_activate_bodies(constraint);
	((phy_pivot_joint *)constraint)->anchor_B = anchorB;
}
