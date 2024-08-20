/* Copyright (c) 2013 Scott Lembcke and Howling Moon Software
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "khg_phy/phy_private.h"

static void
preStep(phy_pin_joint *joint, float dt)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	joint->r1 = cpTransformVect(a->transform, cpvsub(joint->anchorA, a->cog));
	joint->r2 = cpTransformVect(b->transform, cpvsub(joint->anchorB, b->cog));
	
	phy_vect delta = cpvsub(cpvadd(b->p, joint->r2), cpvadd(a->p, joint->r1));
	float dist = cpvlength(delta);
	joint->n = cpvmult(delta, 1.0f/(dist ? dist : (float)INFINITY));
	
	// calculate mass normal
	joint->nMass = 1.0f/k_scalar(a, b, joint->r1, joint->r2, joint->n);
	
	// calculate bias velocity
	float maxBias = joint->constraint.maxBias;
	joint->bias = phy_clamp(-bias_coef(joint->constraint.errorBias, dt)*(dist - joint->dist)/dt, -maxBias, maxBias);
}

static void
applyCachedImpulse(phy_pin_joint *joint, float dt_coef)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	phy_vect j = cpvmult(joint->n, joint->jnAcc*dt_coef);
	apply_impulses(a, b, joint->r1, joint->r2, j);
}

static void
applyImpulse(phy_pin_joint *joint, float dt)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	phy_vect n = joint->n;

	// compute relative velocity
	float vrn = normal_relative_velocity(a, b, joint->r1, joint->r2, n);
	
	float jnMax = joint->constraint.maxForce*dt;
	
	// compute normal impulse
	float jn = (joint->bias - vrn)*joint->nMass;
	float jnOld = joint->jnAcc;
	joint->jnAcc = phy_clamp(jnOld + jn, -jnMax, jnMax);
	jn = joint->jnAcc - jnOld;
	
	// apply impulse
	apply_impulses(a, b, joint->r1, joint->r2, cpvmult(n, jn));
}

static float
getImpulse(phy_pin_joint *joint)
{
	return phy_abs(joint->jnAcc);
}

static const cpConstraintClass klass = {
	(cpConstraintPreStepImpl)preStep,
	(cpConstraintApplyCachedImpulseImpl)applyCachedImpulse,
	(cpConstraintApplyImpulseImpl)applyImpulse,
	(cpConstraintGetImpulseImpl)getImpulse,
};


phy_pin_joint *
cpPinJointAlloc(void)
{
	return (phy_pin_joint *)calloc(1, sizeof(phy_pin_joint));
}

phy_pin_joint *
cpPinJointInit(phy_pin_joint *joint, phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB)
{
	cpConstraintInit((phy_constraint *)joint, &klass, a, b);
	
	joint->anchorA = anchorA;
	joint->anchorB = anchorB;
	
	// STATIC_BODY_CHECK
	phy_vect p1 = (a ? cpTransformPoint(a->transform, anchorA) : anchorA);
	phy_vect p2 = (b ? cpTransformPoint(b->transform, anchorB) : anchorB);
	joint->dist = cpvlength(cpvsub(p2, p1));
	
	cpAssertWarn(joint->dist > 0.0, "You created a 0 length pin joint. A pivot joint will be much more stable.");

	joint->jnAcc = 0.0f;
	
	return joint;
}

phy_constraint *
cpPinJointNew(phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB)
{
	return (phy_constraint *)cpPinJointInit(cpPinJointAlloc(), a, b, anchorA, anchorB);
}

bool
cpConstraintIsPinJoint(const phy_constraint *constraint)
{
	return (constraint->klass == &klass);
}

phy_vect
cpPinJointGetAnchorA(const phy_constraint *constraint)
{
	cpAssertHard(cpConstraintIsPinJoint(constraint), "Constraint is not a pin joint.");
	return ((phy_pin_joint *)constraint)->anchorA;
}

void
cpPinJointSetAnchorA(phy_constraint *constraint, phy_vect anchorA)
{
	cpAssertHard(cpConstraintIsPinJoint(constraint), "Constraint is not a pin joint.");
	cpConstraintActivateBodies(constraint);
	((phy_pin_joint *)constraint)->anchorA = anchorA;
}

phy_vect
cpPinJointGetAnchorB(const phy_constraint *constraint)
{
	cpAssertHard(cpConstraintIsPinJoint(constraint), "Constraint is not a pin joint.");
	return ((phy_pin_joint *)constraint)->anchorB;
}

void
cpPinJointSetAnchorB(phy_constraint *constraint, phy_vect anchorB)
{
	cpAssertHard(cpConstraintIsPinJoint(constraint), "Constraint is not a pin joint.");
	cpConstraintActivateBodies(constraint);
	((phy_pin_joint *)constraint)->anchorB = anchorB;
}

float
cpPinJointGetDist(const phy_constraint *constraint)
{
	cpAssertHard(cpConstraintIsPinJoint(constraint), "Constraint is not a pin joint.");
	return ((phy_pin_joint *)constraint)->dist;
}

void
cpPinJointSetDist(phy_constraint *constraint, float dist)
{
	cpAssertHard(cpConstraintIsPinJoint(constraint), "Constraint is not a pin joint.");
	cpConstraintActivateBodies(constraint);
	((phy_pin_joint *)constraint)->dist = dist;
}
