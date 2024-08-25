#include "khg_phy/phy_private.h"
#include "khg_phy/phy_structs.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

static void
preStep(phy_simple_motor_joint *joint, float dt)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	// calculate moment of inertia coefficient.
	joint->i_sum = 1.0f/(a->i_inv + b->i_inv);
}

static void
applyCachedImpulse(phy_simple_motor_joint *joint, float dt_coef)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	float j = joint->j_acc*dt_coef;
	a->w -= j*a->i_inv;
	b->w += j*b->i_inv;
}

static void
applyImpulse(phy_simple_motor_joint *joint, float dt)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	// compute relative rotational velocity
	float wr = b->w - a->w + joint->rate;
	
	float jMax = joint->constraint.max_force*dt;
	
	// compute normal impulse	
	float j = -wr*joint->i_sum;
	float jOld = joint->j_acc;
	joint->j_acc = phy_clamp(jOld + j, -jMax, jMax);
	j = joint->j_acc - jOld;
	
	// apply impulse
	a->w -= j*a->i_inv;
	b->w += j*b->i_inv;
}

static float
getImpulse(phy_simple_motor_joint *joint)
{
	return phy_abs(joint->j_acc);
}

static const phy_constraint_class klass = {
	(phy_constraint_pre_step_impl)preStep,
	(phy_constraint_apply_cached_impulse_impl)applyCachedImpulse,
	(phy_constraint_apply_impulse_impl)applyImpulse,
	(phy_constraint_get_impulse_impl)getImpulse,
};

phy_simple_motor_joint *
cpSimpleMotorAlloc(void)
{
	return (phy_simple_motor_joint *)calloc(1, sizeof(phy_simple_motor_joint));
}

phy_simple_motor_joint *
cpSimpleMotorInit(phy_simple_motor_joint *joint, phy_body *a, phy_body *b, float rate)
{
	cp_constraint_init((phy_constraint *)joint, &klass, a, b);
	
	joint->rate = rate;
	
	joint->j_acc = 0.0f;
	
	return joint;
}

phy_constraint *
cpSimpleMotorNew(phy_body *a, phy_body *b, float rate)
{
	return (phy_constraint *)cpSimpleMotorInit(cpSimpleMotorAlloc(), a, b, rate);
}

bool
cpConstraintIsSimpleMotor(const phy_constraint *constraint)
{
	return (constraint->class == &klass);
}

float
cpSimpleMotorGetRate(const phy_constraint *constraint)
{
	if (!cpConstraintIsSimpleMotor(constraint)) {
    utl_error_func("Constraint is not a SimpleMotor", utl_user_defined_data);
  }
	return ((phy_simple_motor_joint *)constraint)->rate;
}

void
cpSimpleMotorSetRate(phy_constraint *constraint, float rate)
{
	if (!cpConstraintIsSimpleMotor(constraint)) {
    utl_error_func("Constraint is not a SimpleMotor", utl_user_defined_data);
  }
	phy_constraint_activate_bodies(constraint);
	((phy_simple_motor_joint *)constraint)->rate = rate;
}
