#include "khg_phy/phy_private.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

static void
preStep(phy_gear_joint *joint, float dt)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	// calculate moment of inertia coefficient.
	joint->iSum = 1.0f/(a->i_inv*joint->ratio_inv + joint->ratio*b->i_inv);
	
	// calculate bias velocity
	float maxBias = joint->constraint.maxBias;
	joint->bias = phy_clamp(-bias_coef(joint->constraint.errorBias, dt)*(b->a*joint->ratio - a->a - joint->phase)/dt, -maxBias, maxBias);
}

static void
applyCachedImpulse(phy_gear_joint *joint, float dt_coef)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	float j = joint->jAcc*dt_coef;
	a->w -= j*a->i_inv*joint->ratio_inv;
	b->w += j*b->i_inv;
}

static void
applyImpulse(phy_gear_joint *joint, float dt)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	// compute relative rotational velocity
	float wr = b->w*joint->ratio - a->w;
	
	float jMax = joint->constraint.maxForce*dt;
	
	// compute normal impulse	
	float j = (joint->bias - wr)*joint->iSum;
	float jOld = joint->jAcc;
	joint->jAcc = phy_clamp(jOld + j, -jMax, jMax);
	j = joint->jAcc - jOld;
	
	// apply impulse
	a->w -= j*a->i_inv*joint->ratio_inv;
	b->w += j*b->i_inv;
}

static float
getImpulse(phy_gear_joint *joint)
{
	return phy_abs(joint->jAcc);
}

static const cpConstraintClass klass = {
	(cpConstraintPreStepImpl)preStep,
	(cpConstraintApplyCachedImpulseImpl)applyCachedImpulse,
	(cpConstraintApplyImpulseImpl)applyImpulse,
	(cpConstraintGetImpulseImpl)getImpulse,
};

phy_gear_joint *
phy_gear_joint_alloc(void)
{
	return (phy_gear_joint *)calloc(1, sizeof(phy_gear_joint));
}

phy_gear_joint *
phy_gear_joint_init(phy_gear_joint *joint, phy_body *a, phy_body *b, float phase, float ratio)
{
	cpConstraintInit((phy_constraint *)joint, &klass, a, b);
	
	joint->phase = phase;
	joint->ratio = ratio;
	joint->ratio_inv = 1.0f/ratio;
	
	joint->jAcc = 0.0f;
	
	return joint;
}

phy_constraint *
phy_gear_joint_new(phy_body *a, phy_body *b, float phase, float ratio)
{
	return (phy_constraint *)phy_gear_joint_init(phy_gear_joint_alloc(), a, b, phase, ratio);
}

bool
phy_constraint_is_gear_joint(const phy_constraint *constraint)
{
	return (constraint->klass == &klass);
}

float
phy_gear_joint_get_phase(const phy_constraint *constraint)
{
	if (!phy_constraint_is_gear_joint(constraint)) {
    utl_error_func("Constraint is not a ratchet joint", utl_user_defined_data);
  }
	return ((phy_gear_joint *)constraint)->phase;
}

void
phy_gear_joint_set_phase(phy_constraint *constraint, float phase)
{
	if (!phy_constraint_is_gear_joint(constraint)) {
    utl_error_func("Constraint is not a ratchet joint", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_gear_joint *)constraint)->phase = phase;
}

float
phy_gear_joint_get_ratio(const phy_constraint *constraint)
{
	if (!phy_constraint_is_gear_joint(constraint)) {
    utl_error_func("Constraint is not a ratchet joint", utl_user_defined_data);
  }
	return ((phy_gear_joint *)constraint)->ratio;
}

void
phy_gear_joint_set_ratio(phy_constraint *constraint, float ratio)
{
	if (!phy_constraint_is_gear_joint(constraint)) {
    utl_error_func("Constraint is not a ratchet joint", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_gear_joint *)constraint)->ratio = ratio;
	((phy_gear_joint *)constraint)->ratio_inv = 1.0f/ratio;
}
