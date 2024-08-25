#include "khg_phy/phy_private.h"
#include "khg_phy/phy_structs.h"
#include "khg_phy/transform.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

static void
preStep(phy_pin_joint *joint, float dt)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	joint->r1 = cpTransformVect(a->transform, cpvsub(joint->anchor_A, a->cog));
	joint->r2 = cpTransformVect(b->transform, cpvsub(joint->anchor_B, b->cog));
	
	phy_vect delta = cpvsub(cpvadd(b->p, joint->r2), cpvadd(a->p, joint->r1));
	float dist = cpvlength(delta);
	joint->n = cpvmult(delta, 1.0f/(dist ? dist : (float)INFINITY));
	
	// calculate mass normal
	joint->n_mass = 1.0f/phy_k_scalar(a, b, joint->r1, joint->r2, joint->n);
	
	// calculate bias velocity
	float maxBias = joint->constraint.max_bias;
	joint->bias = phy_clamp(-phy_bias_coef(joint->constraint.error_bias, dt)*(dist - joint->dist)/dt, -maxBias, maxBias);
}

static void
applyCachedImpulse(phy_pin_joint *joint, float dt_coef)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	phy_vect j = cpvmult(joint->n, joint->jn_acc*dt_coef);
	phy_apply_impulses(a, b, joint->r1, joint->r2, j);
}

static void
applyImpulse(phy_pin_joint *joint, float dt)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	phy_vect n = joint->n;

	// compute relative velocity
	float vrn = phy_normal_relative_velocity(a, b, joint->r1, joint->r2, n);
	
	float jnMax = joint->constraint.max_force*dt;
	
	// compute normal impulse
	float jn = (joint->bias - vrn)*joint->n_mass;
	float jnOld = joint->jn_acc;
	joint->jn_acc = phy_clamp(jnOld + jn, -jnMax, jnMax);
	jn = joint->jn_acc - jnOld;
	
	// apply impulse
	phy_apply_impulses(a, b, joint->r1, joint->r2, cpvmult(n, jn));
}

static float
getImpulse(phy_pin_joint *joint)
{
	return phy_abs(joint->jn_acc);
}

static const phy_constraint_class klass = {
	(phy_constraint_pre_step_impl)preStep,
	(phy_constraint_apply_cached_impulse_impl)applyCachedImpulse,
	(phy_constraint_apply_impulse_impl)applyImpulse,
	(phy_constraint_get_impulse_impl)getImpulse,
};


phy_pin_joint *
cpPinJointAlloc(void)
{
	return (phy_pin_joint *)calloc(1, sizeof(phy_pin_joint));
}

phy_pin_joint *
cpPinJointInit(phy_pin_joint *joint, phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB)
{
	cp_constraint_init((phy_constraint *)joint, &klass, a, b);
	
	joint->anchor_A = anchorA;
	joint->anchor_B = anchorB;
	
	// STATIC_BODY_CHECK
	phy_vect p1 = (a ? cpTransformPoint(a->transform, anchorA) : anchorA);
	phy_vect p2 = (b ? cpTransformPoint(b->transform, anchorB) : anchorB);
	joint->dist = cpvlength(cpvsub(p2, p1));
	
	if (joint->dist <= 0.0) {
    utl_error_func("You created a 0 length pin joint, a pivot joint will be much more stable", utl_user_defined_data);
  }

	joint->jn_acc = 0.0f;
	
	return joint;
}

phy_constraint *
cpPinJointNew(phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB)
{
	return (phy_constraint *)cpPinJointInit(cpPinJointAlloc(), a, b, anchorA, anchorB);
}

bool
cpConstraintIsPinJoint(const phy_constraint *constraint)
{
	return (constraint->class == &klass);
}

phy_vect
cpPinJointGetAnchorA(const phy_constraint *constraint)
{
	if (!cpConstraintIsPinJoint(constraint)) {
    utl_error_func("Constraint is not a pin joint", utl_user_defined_data);
  }
	return ((phy_pin_joint *)constraint)->anchor_A;
}

void
cpPinJointSetAnchorA(phy_constraint *constraint, phy_vect anchorA)
{
	if (!cpConstraintIsPinJoint(constraint)) {
    utl_error_func("Constraint is not a pin joint", utl_user_defined_data);
  }
	phy_constraint_activate_bodies(constraint);
	((phy_pin_joint *)constraint)->anchor_A = anchorA;
}

phy_vect
cpPinJointGetAnchorB(const phy_constraint *constraint)
{
	if (!cpConstraintIsPinJoint(constraint)) {
    utl_error_func("Constraint is not a pin joint", utl_user_defined_data);
  }
	return ((phy_pin_joint *)constraint)->anchor_B;
}

void
cpPinJointSetAnchorB(phy_constraint *constraint, phy_vect anchorB)
{
	if (!cpConstraintIsPinJoint(constraint)) {
    utl_error_func("Constraint is not a pin joint", utl_user_defined_data);
  }
	phy_constraint_activate_bodies(constraint);
	((phy_pin_joint *)constraint)->anchor_B = anchorB;
}

float
cpPinJointGetDist(const phy_constraint *constraint)
{
	if (!cpConstraintIsPinJoint(constraint)) {
    utl_error_func("Constraint is not a pin joint", utl_user_defined_data);
  }
	return ((phy_pin_joint *)constraint)->dist;
}

void
cpPinJointSetDist(phy_constraint *constraint, float dist)
{
	if (!cpConstraintIsPinJoint(constraint)) {
    utl_error_func("Constraint is not a pin joint", utl_user_defined_data);
  }
	phy_constraint_activate_bodies(constraint);
	((phy_pin_joint *)constraint)->dist = dist;
}
