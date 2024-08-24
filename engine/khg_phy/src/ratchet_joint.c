#include "khg_phy/phy_private.h"
#include "khg_utl/error_func.h"

static void
preStep(phy_ratchet_joint *joint, float dt)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	float angle = joint->angle;
	float phase = joint->phase;
	float ratchet = joint->ratchet;
	
	float delta = b->a - a->a;
	float diff = angle - delta;
	float pdist = 0.0f;
	
	if(diff*ratchet > 0.0f){
		pdist = diff;
	} else {
		joint->angle = floorf((delta - phase)/ratchet)*ratchet + phase;
	}
	
	// calculate moment of inertia coefficient.
	joint->iSum = 1.0f/(a->i_inv + b->i_inv);
	
	// calculate bias velocity
	float maxBias = joint->constraint.maxBias;
	joint->bias = phy_clamp(-bias_coef(joint->constraint.errorBias, dt)*pdist/dt, -maxBias, maxBias);

	// If the bias is 0, the joint is not at a limit. Reset the impulse.
	if(!joint->bias) joint->jAcc = 0.0f;
}

static void
applyCachedImpulse(phy_ratchet_joint *joint, float dt_coef)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	float j = joint->jAcc*dt_coef;
	a->w -= j*a->i_inv;
	b->w += j*b->i_inv;
}

static void
applyImpulse(phy_ratchet_joint *joint, float dt)
{
	if(!joint->bias) return; // early exit

	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	// compute relative rotational velocity
	float wr = b->w - a->w;
	float ratchet = joint->ratchet;
	
	float jMax = joint->constraint.maxForce*dt;
	
	// compute normal impulse	
	float j = -(joint->bias + wr)*joint->iSum;
	float jOld = joint->jAcc;
	joint->jAcc = phy_clamp((jOld + j)*ratchet, 0.0f, jMax*phy_abs(ratchet))/ratchet;
	j = joint->jAcc - jOld;
	
	// apply impulse
	a->w -= j*a->i_inv;
	b->w += j*b->i_inv;
}

static float
getImpulse(phy_ratchet_joint *joint)
{
	return phy_abs(joint->jAcc);
}

static const cpConstraintClass klass = {
	(cpConstraintPreStepImpl)preStep,
	(cpConstraintApplyCachedImpulseImpl)applyCachedImpulse,
	(cpConstraintApplyImpulseImpl)applyImpulse,
	(cpConstraintGetImpulseImpl)getImpulse,
};

phy_ratchet_joint *
cpRatchetJointAlloc(void)
{
	return (phy_ratchet_joint *)calloc(1, sizeof(phy_ratchet_joint));
}

phy_ratchet_joint *
cpRatchetJointInit(phy_ratchet_joint *joint, phy_body *a, phy_body *b, float phase, float ratchet)
{
	cpConstraintInit((phy_constraint *)joint, &klass, a, b);
	
	joint->angle = 0.0f;
	joint->phase = phase;
	joint->ratchet = ratchet;
	
	// STATIC_BODY_CHECK
	joint->angle = (b ? b->a : 0.0f) - (a ? a->a : 0.0f);
	
	return joint;
}

phy_constraint *
cpRatchetJointNew(phy_body *a, phy_body *b, float phase, float ratchet)
{
	return (phy_constraint *)cpRatchetJointInit(cpRatchetJointAlloc(), a, b, phase, ratchet);
}

bool
cpConstraintIsRatchetJoint(const phy_constraint *constraint)
{
	return (constraint->klass == &klass);
}

float
cpRatchetJointGetAngle(const phy_constraint *constraint)
{
	if (!cpConstraintIsRatchetJoint(constraint)) {
    utl_error_func("Constraint is not a ratchet joint", utl_user_defined_data);
  }
	return ((phy_ratchet_joint *)constraint)->angle;
}

void
cpRatchetJointSetAngle(phy_constraint *constraint, float angle)
{
	if (!cpConstraintIsRatchetJoint(constraint)) {
    utl_error_func("Constraint is not a ratchet joint", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_ratchet_joint *)constraint)->angle = angle;
}

float
cpRatchetJointGetPhase(const phy_constraint *constraint)
{
	if (!cpConstraintIsRatchetJoint(constraint)) {
    utl_error_func("Constraint is not a ratchet joint", utl_user_defined_data);
  }
	return ((phy_ratchet_joint *)constraint)->phase;
}

void
cpRatchetJointSetPhase(phy_constraint *constraint, float phase)
{
	if (!cpConstraintIsRatchetJoint(constraint)) {
    utl_error_func("Constraint is not a ratchet joint", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_ratchet_joint *)constraint)->phase = phase;
}
float
cpRatchetJointGetRatchet(const phy_constraint *constraint)
{
	if (!cpConstraintIsRatchetJoint(constraint)) {
    utl_error_func("Constraint is not a ratchet joint", utl_user_defined_data);
  }
	return ((phy_ratchet_joint *)constraint)->ratchet;
}

void
cpRatchetJointSetRatchet(phy_constraint *constraint, float ratchet)
{
	if (!cpConstraintIsRatchetJoint(constraint)) {
    utl_error_func("Constraint is not a ratchet joint", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_ratchet_joint *)constraint)->ratchet = ratchet;
}
