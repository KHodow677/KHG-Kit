#include "khg_phy/phy_private.h"
#include "khg_phy/transform.h"
#include "khg_utl/error_func.h"

static float
defaultSpringForce(phy_damped_spring *spring, float dist){
	return (spring->restLength - dist)*spring->stiffness;
}

static void
preStep(phy_damped_spring *spring, float dt)
{
	phy_body *a = spring->constraint.a;
	phy_body *b = spring->constraint.b;
	
	spring->r1 = cpTransformVect(a->transform, cpvsub(spring->anchorA, a->cog));
	spring->r2 = cpTransformVect(b->transform, cpvsub(spring->anchorB, b->cog));
	
	phy_vect delta = cpvsub(cpvadd(b->p, spring->r2), cpvadd(a->p, spring->r1));
	float dist = cpvlength(delta);
	spring->n = cpvmult(delta, 1.0f/(dist ? dist : INFINITY));
	
	float k = k_scalar(a, b, spring->r1, spring->r2, spring->n);
	if (k == 0.0) {
    utl_error_func("Unsolvable spring", utl_user_defined_data);
  }
	spring->nMass = 1.0f/k;
	
	spring->target_vrn = 0.0f;
	spring->v_coef = 1.0f - expf(-spring->damping*dt*k);

	// apply spring force
	float f_spring = spring->springForceFunc((phy_constraint *)spring, dist);
	float j_spring = spring->jAcc = f_spring*dt;
	apply_impulses(a, b, spring->r1, spring->r2, cpvmult(spring->n, j_spring));
}

static void applyCachedImpulse(phy_damped_spring *spring, float dt_coef){}

static void
applyImpulse(phy_damped_spring *spring, float dt)
{
	phy_body *a = spring->constraint.a;
	phy_body *b = spring->constraint.b;
	
	phy_vect n = spring->n;
	phy_vect r1 = spring->r1;
	phy_vect r2 = spring->r2;

	// compute relative velocity
	float vrn = normal_relative_velocity(a, b, r1, r2, n);
	
	// compute velocity loss from drag
	float v_damp = (spring->target_vrn - vrn)*spring->v_coef;
	spring->target_vrn = vrn + v_damp;
	
	float j_damp = v_damp*spring->nMass;
	spring->jAcc += j_damp;
	apply_impulses(a, b, spring->r1, spring->r2, cpvmult(spring->n, j_damp));
}

static float
getImpulse(phy_damped_spring *spring)
{
	return spring->jAcc;
}

static const cpConstraintClass klass = {
	(cpConstraintPreStepImpl)preStep,
	(cpConstraintApplyCachedImpulseImpl)applyCachedImpulse,
	(cpConstraintApplyImpulseImpl)applyImpulse,
	(cpConstraintGetImpulseImpl)getImpulse,
};

phy_damped_spring *
phy_damped_spring_alloc(void)
{
	return (phy_damped_spring *)calloc(1, sizeof(phy_damped_spring));
}

phy_damped_spring *
phy_damped_spring_init(phy_damped_spring *spring, phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB, float restLength, float stiffness, float damping)
{
	cpConstraintInit((phy_constraint *)spring, &klass, a, b);
	
	spring->anchorA = anchorA;
	spring->anchorB = anchorB;
	
	spring->restLength = restLength;
	spring->stiffness = stiffness;
	spring->damping = damping;
	spring->springForceFunc = (phy_damped_spring_force_func)defaultSpringForce;
	
	spring->jAcc = 0.0f;
	
	return spring;
}

phy_constraint *
phy_damped_spring_new(phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB, float restLength, float stiffness, float damping)
{
	return (phy_constraint *)phy_damped_spring_init(phy_damped_spring_alloc(), a, b, anchorA, anchorB, restLength, stiffness, damping);
}

bool
phy_constraint_is_damped_spring(const phy_constraint *constraint)
{
	return (constraint->klass == &klass);
}

phy_vect
phy_damped_spring_get_anchor_A(const phy_constraint *constraint)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	return ((phy_damped_spring *)constraint)->anchorA;
}

void
phy_damped_spring_set_anchor_A(phy_constraint *constraint, phy_vect anchorA)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_damped_spring *)constraint)->anchorA = anchorA;
}

phy_vect
phy_damped_spring_get_anchor_B(const phy_constraint *constraint)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	return ((phy_damped_spring *)constraint)->anchorB;
}

void
phy_damped_spring_set_anchor_B(phy_constraint *constraint, phy_vect anchorB)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_damped_spring *)constraint)->anchorB = anchorB;
}

float
phy_damped_spring_get_rest_length(const phy_constraint *constraint)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	return ((phy_damped_spring *)constraint)->restLength;
}

void
phy_damped_spring_set_rest_length(phy_constraint *constraint, float restLength)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_damped_spring *)constraint)->restLength = restLength;
}

float
phy_damped_spring_get_stiffness(const phy_constraint *constraint)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	return ((phy_damped_spring *)constraint)->stiffness;
}

void
phy_damped_spring_set_stiffness(phy_constraint *constraint, float stiffness)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_damped_spring *)constraint)->stiffness = stiffness;
}

float
phy_damped_spring_get_damping(const phy_constraint *constraint)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	return ((phy_damped_spring *)constraint)->damping;
}

void
phy_damped_spring_set_damping(phy_constraint *constraint, float damping)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_damped_spring *)constraint)->damping = damping;
}

phy_damped_spring_force_func
phy_damped_spring_get_spring_force_func(const phy_constraint *constraint)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	return ((phy_damped_spring *)constraint)->springForceFunc;
}

void
phy_damped_spring_set_spring_force_func(phy_constraint *constraint, phy_damped_spring_force_func springForceFunc)
{
	if (!phy_constraint_is_damped_spring(constraint)) {
    utl_error_func("Constraint is not a damped spring", utl_user_defined_data);
  }
	cpConstraintActivateBodies(constraint);
	((phy_damped_spring *)constraint)->springForceFunc = springForceFunc;
}
