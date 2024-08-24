#include "khg_phy/phy_private.h"
#include "khg_utl/error_func.h"

static void
preStep(phy_pin_joint *joint, float dt)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	joint->r1 = cpTransformVect(a->transform, cpvsub(joint->anchorA, a->cog));
	joint->r2 = cpTransformVect(b->transform, cpvsub(joint->anchorB, b->cog));
	
	phy_vect delta = cpvsub(cpvadd(b->p, joint->r2), cpvadd(a->p, joint->r1));
	float dist = cpvlength(delta);
	joint->n = cpvmult(delta, 1.0f/(dist ? dist : (float)INFINITY));
	
	// calculate mass normal
	joint->nMass = 1.0f/k_scalar(a, b, joint->r1, joint->r2, joint->n);
	
	// calculate bias velocity
	float maxBias = joint->constraint.maxBias;
	joint->bias = phy_clamp(-bias_coef(joint->constraint.errorBias, dt)*(dist - joint->dist)/dt, -maxBias, maxBias);
}

static void
applyCachedImpulse(phy_pin_joint *joint, float dt_coef)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	phy_vect j = cpvmult(joint->n, joint->jnAcc*dt_coef);
	apply_impulses(a, b, joint->r1, joint->r2, j);
}

static void
applyImpulse(phy_pin_joint *joint, float dt)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	phy_vect n = joint->n;

	// compute relative velocity
	float vrn = normal_relative_velocity(a, b, joint->r1, joint->r2, n);
	
	float jnMax = joint->constraint.maxForce*dt;
	
	// compute normal impulse
	float jn = (joint->bias - vrn)*joint->nMass;
	float jnOld = joint->jnAcc;
	joint->jnAcc = phy_clamp(jnOld + jn, -jnMax, jnMax);
	jn = joint->jnAcc - jnOld;
	
	// apply impulse
	apply_impulses(a, b, joint->r1, joint->r2, cpvmult(n, jn));
}

static float
getImpulse(phy_pin_joint *joint)
{
	return phy_abs(joint->jnAcc);
}

static const cpConstraintClass klass = {
	(cpConstraintPreStepImpl)preStep,
	(cpConstraintApplyCachedImpulseImpl)applyCachedImpulse,
	(cpConstraintApplyImpulseImpl)applyImpulse,
	(cpConstraintGetImpulseImpl)getImpulse,
};


phy_pin_joint *
cpPinJointAlloc(void)
{
	return (phy_pin_joint *)calloc(1, sizeof(phy_pin_joint));
}

phy_pin_joint *
cpPinJointInit(phy_pin_joint *joint, phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB)
{
	cpConstraintInit((phy_constraint *)joint, &klass, a, b);
	
	joint->anchorA = anchorA;
	joint->anchorB = anchorB;
	
	// STATIC_BODY_CHECK
	phy_vect p1 = (a ? cpTransformPoint(a->transform, anchorA) : anchorA);
	phy_vect p2 = (b ? cpTransformPoint(b->transform, anchorB) : anchorB);
	joint->dist = cpvlength(cpvsub(p2, p1));
	
	if (joint->dist <= 0.0) {
    utl_error_func("You created a 0 length pin joint, a pivot joint will be much more stable", utl_user_defined_data);
  }

	joint->jnAcc = 0.0f;
	
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
	return (constraint->klass == &klass);
}

phy_vect
cpPinJointGetAnchorA(const phy_constraint *constraint)
{
	if (!cpConstraintIsPinJoint(constraint)) {
    utl_error_func("Constraint is not a pin joint", utl_user_defined_data);
  }
	return ((phy_pin_joint *)constraint)->anchorA;
}

void
cpPinJointSetAnchorA(phy_constraint *constraint, phy_vect anchorA)
{
	if (!cpConstraintIsPinJoint(constraint)) {
    utl_error_func("Constraint is not a pin joint", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_pin_joint *)constraint)->anchorA = anchorA;
}

phy_vect
cpPinJointGetAnchorB(const phy_constraint *constraint)
{
	if (!cpConstraintIsPinJoint(constraint)) {
    utl_error_func("Constraint is not a pin joint", utl_user_defined_data);
  }
	return ((phy_pin_joint *)constraint)->anchorB;
}

void
cpPinJointSetAnchorB(phy_constraint *constraint, phy_vect anchorB)
{
	if (!cpConstraintIsPinJoint(constraint)) {
    utl_error_func("Constraint is not a pin joint", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_pin_joint *)constraint)->anchorB = anchorB;
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
	cpConstraintActivateBodies(constraint);
	((phy_pin_joint *)constraint)->dist = dist;
}
