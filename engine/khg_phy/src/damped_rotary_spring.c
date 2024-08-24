#include "khg_phy/phy_private.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

static float
defaultSpringTorque(phy_damped_rotary_spring *spring, float relativeAngle){
	return (relativeAngle - spring->restAngle)*spring->stiffness;
}

static void
preStep(phy_damped_rotary_spring *spring, float dt)
{
	phy_body *a = spring->constraint.a;
	phy_body *b = spring->constraint.b;
	
	float moment = a->i_inv + b->i_inv;
	if (moment == 0.0) {
    utl_error_func("Unsolvable spring", utl_user_defined_data);
  }
	spring->iSum = 1.0f/moment;

	spring->w_coef = 1.0f - expf(-spring->damping*dt*moment);
	spring->target_wrn = 0.0f;

	// apply spring torque
	float j_spring = spring->springTorqueFunc((phy_constraint *)spring, a->a - b->a)*dt;
	spring->jAcc = j_spring;
	
	a->w -= j_spring*a->i_inv;
	b->w += j_spring*b->i_inv;
}

static void applyCachedImpulse(phy_damped_rotary_spring *spring, float dt_coef){}

static void
applyImpulse(phy_damped_rotary_spring *spring, float dt)
{
	phy_body *a = spring->constraint.a;
	phy_body *b = spring->constraint.b;
	
	// compute relative velocity
	float wrn = a->w - b->w;//normal_relative_velocity(a, b, r1, r2, n) - spring->target_vrn;
	
	// compute velocity loss from drag
	// not 100% certain this is derived correctly, though it makes sense
	float w_damp = (spring->target_wrn - wrn)*spring->w_coef;
	spring->target_wrn = wrn + w_damp;
	
	//apply_impulses(a, b, spring->r1, spring->r2, cpvmult(spring->n, v_damp*spring->nMass));
	float j_damp = w_damp*spring->iSum;
	spring->jAcc += j_damp;
	
	a->w += j_damp*a->i_inv;
	b->w -= j_damp*b->i_inv;
}

static float
getImpulse(phy_damped_rotary_spring *spring)
{
	return spring->jAcc;
}

static const cpConstraintClass klass = {
	(cpConstraintPreStepImpl)preStep,
	(cpConstraintApplyCachedImpulseImpl)applyCachedImpulse,
	(cpConstraintApplyImpulseImpl)applyImpulse,
	(cpConstraintGetImpulseImpl)getImpulse,
};

phy_damped_rotary_spring *
phy_damped_rotary_spring_alloc(void)
{
	return (phy_damped_rotary_spring *)calloc(1, sizeof(phy_damped_rotary_spring));
}

phy_damped_rotary_spring *
phy_damped_rotary_spring_init(phy_damped_rotary_spring *spring, phy_body *a, phy_body *b, float restAngle, float stiffness, float damping)
{
	cpConstraintInit((phy_constraint *)spring, &klass, a, b);
	
	spring->restAngle = restAngle;
	spring->stiffness = stiffness;
	spring->damping = damping;
	spring->springTorqueFunc = (phy_damped_rotary_spring_torque_func)defaultSpringTorque;
	
	spring->jAcc = 0.0f;
	
	return spring;
}

phy_constraint *
phy_damped_rotary_spring_new(phy_body *a, phy_body *b, float restAngle, float stiffness, float damping)
{
	return (phy_constraint *)phy_damped_rotary_spring_init(phy_damped_rotary_spring_alloc(), a, b, restAngle, stiffness, damping);
}

bool
phy_constraint_is_damped_rotary_spring(const phy_constraint *constraint)
{
	return (constraint->klass == &klass);
}

float
phy_damped_rotary_spring_get_rest_angle(const phy_constraint *constraint)
{
	if (!phy_constraint_is_damped_rotary_spring(constraint)) {
    utl_error_func("Constraint is not a damped rotary spring", utl_user_defined_data);
  }
	return ((phy_damped_rotary_spring *)constraint)->restAngle;
}

void
phy_damped_rotary_spring_set_rest_angle(phy_constraint *constraint, float restAngle)
{
	if (!phy_constraint_is_damped_rotary_spring(constraint)) {
    utl_error_func("Constraint is not a damped rotary spring", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_damped_rotary_spring *)constraint)->restAngle = restAngle;
}

float
phy_damped_rotary_spring_get_stiffness(const phy_constraint *constraint)
{
	if (!phy_constraint_is_damped_rotary_spring(constraint)) {
    utl_error_func("Constraint is not a damped rotary spring", utl_user_defined_data);
  }
	return ((phy_damped_rotary_spring *)constraint)->stiffness;
}

void
phy_damped_rotary_spring_set_stiffness(phy_constraint *constraint, float stiffness)
{
	if (!phy_constraint_is_damped_rotary_spring(constraint)) {
    utl_error_func("Constraint is not a damped rotary spring", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_damped_rotary_spring *)constraint)->stiffness = stiffness;
}

float
phy_damped_rotary_spring_get_damping(const phy_constraint *constraint)
{
	if (!phy_constraint_is_damped_rotary_spring(constraint)) {
    utl_error_func("Constraint is not a damped rotary spring", utl_user_defined_data);
  }
	return ((phy_damped_rotary_spring *)constraint)->damping;
}

void
phy_damped_rotary_spring_set_damping(phy_constraint *constraint, float damping)
{
	if (!phy_constraint_is_damped_rotary_spring(constraint)) {
    utl_error_func("Constraint is not a damped rotary spring", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_damped_rotary_spring *)constraint)->damping = damping;
}

phy_damped_rotary_spring_torque_func
phy_damped_rotary_spring_get_spring_torque_func(const phy_constraint *constraint)
{
	if (!phy_constraint_is_damped_rotary_spring(constraint)) {
    utl_error_func("Constraint is not a damped rotary spring", utl_user_defined_data);
  }
	return ((phy_damped_rotary_spring *)constraint)->springTorqueFunc;
}

void
phy_damped_rotary_spring_set_spring_torque_func(phy_constraint *constraint, phy_damped_rotary_spring_torque_func springTorqueFunc)
{
	if (!phy_constraint_is_damped_rotary_spring(constraint)) {
    utl_error_func("Constraint is not a damped rotary spring", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_damped_rotary_spring *)constraint)->springTorqueFunc = springTorqueFunc;
}

