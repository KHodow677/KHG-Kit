#include "khg_phy/phy_private.h"
#include "khg_phy/transform.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

static void
preStep(phy_groove_joint *joint, float dt)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	// calculate endpoints in worldspace
	phy_vect ta = cpTransformPoint(a->transform, joint->grv_a);
	phy_vect tb = cpTransformPoint(a->transform, joint->grv_b);

	// calculate axis
	phy_vect n = cpTransformVect(a->transform, joint->grv_n);
	float d = cpvdot(ta, n);
	
	joint->grv_tn = n;
	joint->r2 = cpTransformVect(b->transform, cpvsub(joint->anchor_B, b->cog));
	
	// calculate tangential distance along the axis of r2
	float td = cpvcross(cpvadd(b->p, joint->r2), n);
	// calculate clamping factor and r2
	if(td <= cpvcross(ta, n)){
		joint->clamp = 1.0f;
		joint->r1 = cpvsub(ta, a->p);
	} else if(td >= cpvcross(tb, n)){
		joint->clamp = -1.0f;
		joint->r1 = cpvsub(tb, a->p);
	} else {
		joint->clamp = 0.0f;
		joint->r1 = cpvsub(cpvadd(cpvmult(cpvperp(n), -td), cpvmult(n, d)), a->p);
	}
	
	// Calculate mass tensor
	joint->k = phy_k_tensor(a, b, joint->r1, joint->r2);
	
	// calculate bias velocity
	phy_vect delta = cpvsub(cpvadd(b->p, joint->r2), cpvadd(a->p, joint->r1));
	joint->bias = cpvclamp(cpvmult(delta, -phy_bias_coef(joint->constraint.error_bias, dt)/dt), joint->constraint.max_bias);
}

static void
applyCachedImpulse(phy_groove_joint *joint, float dt_coef)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
		
	phy_apply_impulses(a, b, joint->r1, joint->r2, cpvmult(joint->j_acc, dt_coef));
}

static inline phy_vect
grooveConstrain(phy_groove_joint *joint, phy_vect j, float dt){
	phy_vect n = joint->grv_tn;
	phy_vect jClamp = (joint->clamp*cpvcross(j, n) > 0.0f) ? j : cpvproject(j, n);
	return cpvclamp(jClamp, joint->constraint.max_force*dt);
}

static void
applyImpulse(phy_groove_joint *joint, float dt)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	phy_vect r1 = joint->r1;
	phy_vect r2 = joint->r2;
	
	// compute impulse
	phy_vect vr = phy_relative_velocity(a, b, r1, r2);

	phy_vect j = cpMat2x2Transform(joint->k, cpvsub(joint->bias, vr));
	phy_vect jOld = joint->j_acc;
	joint->j_acc = grooveConstrain(joint, cpvadd(jOld, j), dt);
	j = cpvsub(joint->j_acc, jOld);
	
	// apply impulse
	phy_apply_impulses(a, b, joint->r1, joint->r2, j);
}

static float
getImpulse(phy_groove_joint *joint)
{
	return cpvlength(joint->j_acc);
}

static const phy_constraint_class klass = {
	(phy_constraint_pre_step_impl)preStep,
	(phy_constraint_apply_cached_impulse_impl)applyCachedImpulse,
	(phy_constraint_apply_impulse_impl)applyImpulse,
	(phy_constraint_get_impulse_impl)getImpulse,
};

phy_groove_joint *
phy_groove_joint_alloc(void)
{
	return (phy_groove_joint *)calloc(1, sizeof(phy_groove_joint));
}

phy_groove_joint *
phy_groove_joint_init(phy_groove_joint *joint, phy_body *a, phy_body *b, phy_vect groove_a, phy_vect groove_b, phy_vect anchorB)
{
	cp_constraint_init((phy_constraint *)joint, &klass, a, b);
	
	joint->grv_a = groove_a;
	joint->grv_b = groove_b;
	joint->grv_n = cpvperp(cpvnormalize(cpvsub(groove_b, groove_a)));
	joint->anchor_B = anchorB;
	
	joint->j_acc = cpvzero;
	
	return joint;
}

phy_constraint *
phy_groove_joint_new(phy_body *a, phy_body *b, phy_vect groove_a, phy_vect groove_b, phy_vect anchorB)
{
	return (phy_constraint *)phy_groove_joint_init(phy_groove_joint_alloc(), a, b, groove_a, groove_b, anchorB);
}

bool
phy_constraint_is_groove_joint(const phy_constraint *constraint)
{
	return (constraint->class == &klass);
}

phy_vect
phy_groove_joint_get_groove_A(const phy_constraint *constraint)
{
	if (!phy_constraint_is_groove_joint(constraint)) {
    utl_error_func("Constraint is not a groove joint", utl_user_defined_data);
  }
	return ((phy_groove_joint *)constraint)->grv_a;
}

void
phy_groove_joint_set_groove_A(phy_constraint *constraint, phy_vect value)
{
	if (!phy_constraint_is_groove_joint(constraint)) {
    utl_error_func("Constraint is not a groove joint", utl_user_defined_data);
  }
	phy_groove_joint *g = (phy_groove_joint *)constraint;
	
	g->grv_a = value;
	g->grv_n = cpvperp(cpvnormalize(cpvsub(g->grv_b, value)));
	
	phy_constraint_activate_bodies(constraint);
}

phy_vect
phy_groove_joint_get_groove_B(const phy_constraint *constraint)
{
	if (!phy_constraint_is_groove_joint(constraint)) {
    utl_error_func("Constraint is not a groove joint", utl_user_defined_data);
  }
	return ((phy_groove_joint *)constraint)->grv_b;
}

void
phy_groove_joint_set_groove_B(phy_constraint *constraint, phy_vect value)
{
	if (!phy_constraint_is_groove_joint(constraint)) {
    utl_error_func("Constraint is not a groove joint", utl_user_defined_data);
  }
	phy_groove_joint *g = (phy_groove_joint *)constraint;
	
	g->grv_b = value;
	g->grv_n = cpvperp(cpvnormalize(cpvsub(value, g->grv_a)));
	
	phy_constraint_activate_bodies(constraint);
}

phy_vect
phy_groove_joint_get_anchor_B(const phy_constraint *constraint)
{
	if (!phy_constraint_is_groove_joint(constraint)) {
    utl_error_func("Constraint is not a groove joint", utl_user_defined_data);
  }
	return ((phy_groove_joint *)constraint)->anchor_B;
}

void
phy_groove_joint_set_anchor_B(phy_constraint *constraint, phy_vect anchorB)
{
	if (!phy_constraint_is_groove_joint(constraint)) {
    utl_error_func("Constraint is not a groove joint", utl_user_defined_data);
  }
	phy_constraint_activate_bodies(constraint);
	((phy_groove_joint *)constraint)->anchor_B = anchorB;
}
