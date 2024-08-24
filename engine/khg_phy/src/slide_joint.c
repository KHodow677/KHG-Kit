#include "khg_phy/phy_private.h"
#include "khg_phy/transform.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

static void
preStep(phy_slide_joint *joint, float dt)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	joint->r1 = cpTransformVect(a->transform, cpvsub(joint->anchorA, a->cog));
	joint->r2 = cpTransformVect(b->transform, cpvsub(joint->anchorB, b->cog));
	
	phy_vect delta = cpvsub(cpvadd(b->p, joint->r2), cpvadd(a->p, joint->r1));
	float dist = cpvlength(delta);
	float pdist = 0.0f;
	if(dist > joint->max) {
		pdist = dist - joint->max;
		joint->n = cpvnormalize(delta);
	} else if(dist < joint->min) {
		pdist = joint->min - dist;
		joint->n = cpvneg(cpvnormalize(delta));
	} else {
		joint->n = cpvzero;
		joint->jnAcc = 0.0f;
	}
	
	// calculate mass normal
	joint->nMass = 1.0f/k_scalar(a, b, joint->r1, joint->r2, joint->n);
	
	// calculate bias velocity
	float maxBias = joint->constraint.maxBias;
	joint->bias = phy_clamp(-bias_coef(joint->constraint.errorBias, dt)*pdist/dt, -maxBias, maxBias);
}

static void
applyCachedImpulse(phy_slide_joint *joint, float dt_coef)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	phy_vect j = cpvmult(joint->n, joint->jnAcc*dt_coef);
	apply_impulses(a, b, joint->r1, joint->r2, j);
}

static void
applyImpulse(phy_slide_joint *joint, float dt)
{
	if(cpveql(joint->n, cpvzero)) return;  // early exit

	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	phy_vect n = joint->n;
	phy_vect r1 = joint->r1;
	phy_vect r2 = joint->r2;
		
	// compute relative velocity
	phy_vect vr = relative_velocity(a, b, r1, r2);
	float vrn = cpvdot(vr, n);
	
	// compute normal impulse
	float jn = (joint->bias - vrn)*joint->nMass;
	float jnOld = joint->jnAcc;
	joint->jnAcc = phy_clamp(jnOld + jn, -joint->constraint.maxForce*dt, 0.0f);
	jn = joint->jnAcc - jnOld;
	
	// apply impulse
	apply_impulses(a, b, joint->r1, joint->r2, cpvmult(n, jn));
}

static float
getImpulse(phy_constraint *joint)
{
	return phy_abs(((phy_slide_joint *)joint)->jnAcc);
}

static const cpConstraintClass klass = {
	(cpConstraintPreStepImpl)preStep,
	(cpConstraintApplyCachedImpulseImpl)applyCachedImpulse,
	(cpConstraintApplyImpulseImpl)applyImpulse,
	(cpConstraintGetImpulseImpl)getImpulse,
};

phy_slide_joint *
cpSlideJointAlloc(void)
{
	return (phy_slide_joint *)calloc(1, sizeof(phy_slide_joint));
}

phy_slide_joint *
cpSlideJointInit(phy_slide_joint *joint, phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB, float min, float max)
{
	cpConstraintInit((phy_constraint *)joint, &klass, a, b);
	
	joint->anchorA = anchorA;
	joint->anchorB = anchorB;
	joint->min = min;
	joint->max = max;
	
	joint->jnAcc = 0.0f;
	
	return joint;
}

phy_constraint *
cpSlideJointNew(phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB, float min, float max)
{
	return (phy_constraint *)cpSlideJointInit(cpSlideJointAlloc(), a, b, anchorA, anchorB, min, max);
}

bool
cpConstraintIsSlideJoint(const phy_constraint *constraint)
{
	return (constraint->klass == &klass);
}

phy_vect
cpSlideJointGetAnchorA(const phy_constraint *constraint)
{
	if (!cpConstraintIsSlideJoint(constraint)) {
    utl_error_func("Constraint is not a slide joint", utl_user_defined_data);
  }
	return ((phy_slide_joint *)constraint)->anchorA;
}

void
cpSlideJointSetAnchorA(phy_constraint *constraint, phy_vect anchorA)
{
	if (!cpConstraintIsSlideJoint(constraint)) {
    utl_error_func("Constraint is not a slide joint", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_slide_joint *)constraint)->anchorA = anchorA;
}

phy_vect
cpSlideJointGetAnchorB(const phy_constraint *constraint)
{
	if (!cpConstraintIsSlideJoint(constraint)) {
    utl_error_func("Constraint is not a slide joint", utl_user_defined_data);
  }
	return ((phy_slide_joint *)constraint)->anchorB;
}

void
cpSlideJointSetAnchorB(phy_constraint *constraint, phy_vect anchorB)
{
	if (!cpConstraintIsSlideJoint(constraint)) {
    utl_error_func("Constraint is not a slide joint", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_slide_joint *)constraint)->anchorB = anchorB;
}

float
cpSlideJointGetMin(const phy_constraint *constraint)
{
	if (!cpConstraintIsSlideJoint(constraint)) {
    utl_error_func("Constraint is not a slide joint", utl_user_defined_data);
  }
	return ((phy_slide_joint *)constraint)->min;
}

void
cpSlideJointSetMin(phy_constraint *constraint, float min)
{
	if (!cpConstraintIsSlideJoint(constraint)) {
    utl_error_func("Constraint is not a slide joint", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_slide_joint *)constraint)->min = min;
}

float
cpSlideJointGetMax(const phy_constraint *constraint)
{
	if (!cpConstraintIsSlideJoint(constraint)) {
    utl_error_func("Constraint is not a slide joint", utl_user_defined_data);
  }
	return ((phy_slide_joint *)constraint)->max;
}

void
cpSlideJointSetMax(phy_constraint *constraint, float max)
{
	if (!cpConstraintIsSlideJoint(constraint)) {
    utl_error_func("Constraint is not a slide joint", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_slide_joint *)constraint)->max = max;
}
