#include "khg_phy/phy_private.h"
#include "khg_phy/phy_structs.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

static void
preStep(rotary_limit_joint *joint, float dt)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	float dist = b->a - a->a;
	float pdist = 0.0f;
	if(dist > joint->max) {
		pdist = joint->max - dist;
	} else if(dist < joint->min) {
		pdist = joint->min - dist;
	}
	
	// calculate moment of inertia coefficient.
	joint->i_sum = 1.0f/(a->i_inv + b->i_inv);
	
	// calculate bias velocity
	float maxBias = joint->constraint.max_bias;
	joint->bias = phy_clamp(-phy_bias_coef(joint->constraint.error_bias, dt)*pdist/dt, -maxBias, maxBias);

	// If the bias is 0, the joint is not at a limit. Reset the impulse.
	if(!joint->bias) joint->j_acc = 0.0f;
}

static void
applyCachedImpulse(rotary_limit_joint *joint, float dt_coef)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	float j = joint->j_acc*dt_coef;
	a->w -= j*a->i_inv;
	b->w += j*b->i_inv;
}

static void
applyImpulse(rotary_limit_joint *joint, float dt)
{
	if(!joint->bias) return; // early exit

	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	// compute relative rotational velocity
	float wr = b->w - a->w;
	
	float jMax = joint->constraint.max_force*dt;
	
	// compute normal impulse	
	float j = -(joint->bias + wr)*joint->i_sum;
	float jOld = joint->j_acc;
	if(joint->bias < 0.0f){
		joint->j_acc = phy_clamp(jOld + j, 0.0f, jMax);
	} else {
		joint->j_acc = phy_clamp(jOld + j, -jMax, 0.0f);
	}
	j = joint->j_acc - jOld;
	
	// apply impulse
	a->w -= j*a->i_inv;
	b->w += j*b->i_inv;
}

static float
getImpulse(rotary_limit_joint *joint)
{
	return phy_abs(joint->j_acc);
}

static const phy_constraint_class klass = {
	(phy_constraint_pre_step_impl)preStep,
	(phy_constraint_apply_cached_impulse_impl)applyCachedImpulse,
	(phy_constraint_apply_impulse_impl)applyImpulse,
	(phy_constraint_get_impulse_impl)getImpulse,
};

rotary_limit_joint *
phy_rotary_limit_joint_alloc(void)
{
	return (rotary_limit_joint *)calloc(1, sizeof(rotary_limit_joint));
}

rotary_limit_joint *
phy_rotary_limit_joint_init(rotary_limit_joint *joint, phy_body *a, phy_body *b, float min, float max)
{
	cp_constraint_init((phy_constraint *)joint, &klass, a, b);
	
	joint->min = min;
	joint->max  = max;
	
	joint->j_acc = 0.0f;
	
	return joint;
}

phy_constraint *
phy_rotary_limit_joint_new(phy_body *a, phy_body *b, float min, float max)
{
	return (phy_constraint *)phy_rotary_limit_joint_init(phy_rotary_limit_joint_alloc(), a, b, min, max);
}

bool
phy_constraint_is_rotary_limit_joint(const phy_constraint *constraint)
{
	return (constraint->class == &klass);
}

float
phy_rotary_limit_joint_get_min(const phy_constraint *constraint)
{
	if (!phy_constraint_is_rotary_limit_joint(constraint)) {
    utl_error_func("Constraint is not a rotary limit joint", utl_user_defined_data);
  }
	return ((rotary_limit_joint *)constraint)->min;
}

void
phy_rotary_limit_joint_set_min(phy_constraint *constraint, float min)
{
	if (!phy_constraint_is_rotary_limit_joint(constraint)) {
    utl_error_func("Constraint is not a rotary limit joint", utl_user_defined_data);
  }
	phy_constraint_activate_bodies(constraint);
	((rotary_limit_joint *)constraint)->min = min;
}

float
phy_rotary_limit_joint_get_max(const phy_constraint *constraint)
{
	if (!phy_constraint_is_rotary_limit_joint(constraint)) {
    utl_error_func("Constraint is not a rotary limit joint", utl_user_defined_data);
  }
	return ((rotary_limit_joint *)constraint)->max;
}

void
phy_rotary_limit_joint_set_max(phy_constraint *constraint, float max)
{
	if (!phy_constraint_is_rotary_limit_joint(constraint)) {
    utl_error_func("Constraint is not a rotary limit joint", utl_user_defined_data);
  }
	phy_constraint_activate_bodies(constraint);
	((rotary_limit_joint *)constraint)->max = max;
}
