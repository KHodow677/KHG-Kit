#include "khg_phy/phy_private.h"
#include "khg_phy/transform.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

static void
preStep(phy_pivot_joint *joint, float dt)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	joint->r1 = cpTransformVect(a->transform, cpvsub(joint->anchorA, a->cog));
	joint->r2 = cpTransformVect(b->transform, cpvsub(joint->anchorB, b->cog));
	
	// Calculate mass tensor
	joint-> k = k_tensor(a, b, joint->r1, joint->r2);
	
	// calculate bias velocity
	phy_vect delta = cpvsub(cpvadd(b->p, joint->r2), cpvadd(a->p, joint->r1));
	joint->bias = cpvclamp(cpvmult(delta, -bias_coef(joint->constraint.errorBias, dt)/dt), joint->constraint.maxBias);
}

static void
applyCachedImpulse(phy_pivot_joint *joint, float dt_coef)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	apply_impulses(a, b, joint->r1, joint->r2, cpvmult(joint->jAcc, dt_coef));
}

static void
applyImpulse(phy_pivot_joint *joint, float dt)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	phy_vect r1 = joint->r1;
	phy_vect r2 = joint->r2;
		
	// compute relative velocity
	phy_vect vr = relative_velocity(a, b, r1, r2);
	
	// compute normal impulse
	phy_vect j = cpMat2x2Transform(joint->k, cpvsub(joint->bias, vr));
	phy_vect jOld = joint->jAcc;
	joint->jAcc = cpvclamp(cpvadd(joint->jAcc, j), joint->constraint.maxForce*dt);
	j = cpvsub(joint->jAcc, jOld);
	
	// apply impulse
	apply_impulses(a, b, joint->r1, joint->r2, j);
}

static float
getImpulse(phy_constraint *joint)
{
	return cpvlength(((phy_pivot_joint *)joint)->jAcc);
}

static const cpConstraintClass klass = {
	(cpConstraintPreStepImpl)preStep,
	(cpConstraintApplyCachedImpulseImpl)applyCachedImpulse,
	(cpConstraintApplyImpulseImpl)applyImpulse,
	(cpConstraintGetImpulseImpl)getImpulse,
};

phy_pivot_joint *
cpPivotJointAlloc(void)
{
	return (phy_pivot_joint *)calloc(1, sizeof(phy_pivot_joint));
}

phy_pivot_joint *
cpPivotJointInit(phy_pivot_joint *joint, phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB)
{
	cpConstraintInit((phy_constraint *)joint, &klass, a, b);
	
	joint->anchorA = anchorA;
	joint->anchorB = anchorB;
	
	joint->jAcc = cpvzero;
	
	return joint;
}

phy_constraint *
cpPivotJointNew2(phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB)
{
	return (phy_constraint *)cpPivotJointInit(cpPivotJointAlloc(), a, b, anchorA, anchorB);
}

phy_constraint *
cpPivotJointNew(phy_body *a, phy_body *b, phy_vect pivot)
{
	phy_vect anchorA = (a ? phy_body_world_to_local(a, pivot) : pivot);
	phy_vect anchorB = (b ? phy_body_world_to_local(b, pivot) : pivot);
	return cpPivotJointNew2(a, b, anchorA, anchorB);
}

bool
cpConstraintIsPivotJoint(const phy_constraint *constraint)
{
	return (constraint->klass == &klass);
}

phy_vect
cpPivotJointGetAnchorA(const phy_constraint *constraint)
{
	if (!cpConstraintIsPivotJoint(constraint)) {
    utl_error_func("Constraint is not a pivot joint", utl_user_defined_data);
  }
	return ((phy_pivot_joint *)constraint)->anchorA;
}

void
cpPivotJointSetAnchorA(phy_constraint *constraint, phy_vect anchorA)
{
	if (!cpConstraintIsPivotJoint(constraint)) {
    utl_error_func("Constraint is not a pivot joint", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_pivot_joint *)constraint)->anchorA = anchorA;
}

phy_vect
cpPivotJointGetAnchorB(const phy_constraint *constraint)
{
	if (!cpConstraintIsPivotJoint(constraint)) {
    utl_error_func("Constraint is not a pivot joint", utl_user_defined_data);
  }
	return ((phy_pivot_joint *)constraint)->anchorB;
}

void
cpPivotJointSetAnchorB(phy_constraint *constraint, phy_vect anchorB)
{
	if (!cpConstraintIsPivotJoint(constraint)) {
    utl_error_func("Constraint is not a pivot joint", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_pivot_joint *)constraint)->anchorB = anchorB;
}
