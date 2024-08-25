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
	
	joint->r1 = cpTransformVect(a->transform, cpvsub(joint->anchor_A, a->cog));
	joint->r2 = cpTransformVect(b->transform, cpvsub(joint->anchor_B, b->cog));
	
	// Calculate mass tensor
	joint-> k = phy_k_tensor(a, b, joint->r1, joint->r2);
	
	// calculate bias velocity
	phy_vect delta = cpvsub(cpvadd(b->p, joint->r2), cpvadd(a->p, joint->r1));
	joint->bias = cpvclamp(cpvmult(delta, -phy_bias_coef(joint->constraint.error_bias, dt)/dt), joint->constraint.max_bias);
}

static void
applyCachedImpulse(phy_pivot_joint *joint, float dt_coef)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	phy_apply_impulses(a, b, joint->r1, joint->r2, cpvmult(joint->j_acc, dt_coef));
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
	phy_vect j = cpMat2x2Transform(joint->k, cpvsub(joint->bias, vr));
	phy_vect jOld = joint->j_acc;
	joint->j_acc = cpvclamp(cpvadd(joint->j_acc, j), joint->constraint.max_force*dt);
	j = cpvsub(joint->j_acc, jOld);
	
	// apply impulse
	phy_apply_impulses(a, b, joint->r1, joint->r2, j);
}

static float
getImpulse(phy_constraint *joint)
{
	return cpvlength(((phy_pivot_joint *)joint)->j_acc);
}

static const phy_constraint_class klass = {
	(phy_constraint_pre_step_impl)preStep,
	(phy_constraint_apply_cached_impulse_impl)applyCachedImpulse,
	(phy_constraint_apply_impulse_impl)applyImpulse,
	(phy_constraint_get_impulse_impl)getImpulse,
};

phy_pivot_joint *
cpPivotJointAlloc(void)
{
	return (phy_pivot_joint *)calloc(1, sizeof(phy_pivot_joint));
}

phy_pivot_joint *
cpPivotJointInit(phy_pivot_joint *joint, phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB)
{
	cp_constraint_init((phy_constraint *)joint, &klass, a, b);
	
	joint->anchor_A = anchorA;
	joint->anchor_B = anchorB;
	
	joint->j_acc = cpvzero;
	
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
	return (constraint->class == &klass);
}

phy_vect
cpPivotJointGetAnchorA(const phy_constraint *constraint)
{
	if (!cpConstraintIsPivotJoint(constraint)) {
    utl_error_func("Constraint is not a pivot joint", utl_user_defined_data);
  }
	return ((phy_pivot_joint *)constraint)->anchor_A;
}

void
cpPivotJointSetAnchorA(phy_constraint *constraint, phy_vect anchorA)
{
	if (!cpConstraintIsPivotJoint(constraint)) {
    utl_error_func("Constraint is not a pivot joint", utl_user_defined_data);
  }
	phy_constraint_activate_bodies(constraint);
	((phy_pivot_joint *)constraint)->anchor_A = anchorA;
}

phy_vect
cpPivotJointGetAnchorB(const phy_constraint *constraint)
{
	if (!cpConstraintIsPivotJoint(constraint)) {
    utl_error_func("Constraint is not a pivot joint", utl_user_defined_data);
  }
	return ((phy_pivot_joint *)constraint)->anchor_B;
}

void
cpPivotJointSetAnchorB(phy_constraint *constraint, phy_vect anchorB)
{
	if (!cpConstraintIsPivotJoint(constraint)) {
    utl_error_func("Constraint is not a pivot joint", utl_user_defined_data);
  }
	phy_constraint_activate_bodies(constraint);
	((phy_pivot_joint *)constraint)->anchor_B = anchorB;
}
