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
preStep(cpSlideJoint *joint, float dt)
{
	cpBody *a = joint->constraint.a;
	cpBody *b = joint->constraint.b;
	
	joint->r1 = cpTransformVect(a->transform, cpvsub(joint->anchorA, a->cog));
	joint->r2 = cpTransformVect(b->transform, cpvsub(joint->anchorB, b->cog));
	
	cpVect delta = cpvsub(cpvadd(b->p, joint->r2), cpvadd(a->p, joint->r1));
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
applyCachedImpulse(cpSlideJoint *joint, float dt_coef)
{
	cpBody *a = joint->constraint.a;
	cpBody *b = joint->constraint.b;
	
	cpVect j = cpvmult(joint->n, joint->jnAcc*dt_coef);
	apply_impulses(a, b, joint->r1, joint->r2, j);
}

static void
applyImpulse(cpSlideJoint *joint, float dt)
{
	if(cpveql(joint->n, cpvzero)) return;  // early exit

	cpBody *a = joint->constraint.a;
	cpBody *b = joint->constraint.b;
	
	cpVect n = joint->n;
	cpVect r1 = joint->r1;
	cpVect r2 = joint->r2;
		
	// compute relative velocity
	cpVect vr = relative_velocity(a, b, r1, r2);
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
getImpulse(cpConstraint *joint)
{
	return phy_abs(((cpSlideJoint *)joint)->jnAcc);
}

static const cpConstraintClass klass = {
	(cpConstraintPreStepImpl)preStep,
	(cpConstraintApplyCachedImpulseImpl)applyCachedImpulse,
	(cpConstraintApplyImpulseImpl)applyImpulse,
	(cpConstraintGetImpulseImpl)getImpulse,
};

cpSlideJoint *
cpSlideJointAlloc(void)
{
	return (cpSlideJoint *)cpcalloc(1, sizeof(cpSlideJoint));
}

cpSlideJoint *
cpSlideJointInit(cpSlideJoint *joint, cpBody *a, cpBody *b, cpVect anchorA, cpVect anchorB, float min, float max)
{
	cpConstraintInit((cpConstraint *)joint, &klass, a, b);
	
	joint->anchorA = anchorA;
	joint->anchorB = anchorB;
	joint->min = min;
	joint->max = max;
	
	joint->jnAcc = 0.0f;
	
	return joint;
}

cpConstraint *
cpSlideJointNew(cpBody *a, cpBody *b, cpVect anchorA, cpVect anchorB, float min, float max)
{
	return (cpConstraint *)cpSlideJointInit(cpSlideJointAlloc(), a, b, anchorA, anchorB, min, max);
}

bool
cpConstraintIsSlideJoint(const cpConstraint *constraint)
{
	return (constraint->klass == &klass);
}

cpVect
cpSlideJointGetAnchorA(const cpConstraint *constraint)
{
	cpAssertHard(cpConstraintIsSlideJoint(constraint), "Constraint is not a slide joint.");
	return ((cpSlideJoint *)constraint)->anchorA;
}

void
cpSlideJointSetAnchorA(cpConstraint *constraint, cpVect anchorA)
{
	cpAssertHard(cpConstraintIsSlideJoint(constraint), "Constraint is not a slide joint.");
	cpConstraintActivateBodies(constraint);
	((cpSlideJoint *)constraint)->anchorA = anchorA;
}

cpVect
cpSlideJointGetAnchorB(const cpConstraint *constraint)
{
	cpAssertHard(cpConstraintIsSlideJoint(constraint), "Constraint is not a slide joint.");
	return ((cpSlideJoint *)constraint)->anchorB;
}

void
cpSlideJointSetAnchorB(cpConstraint *constraint, cpVect anchorB)
{
	cpAssertHard(cpConstraintIsSlideJoint(constraint), "Constraint is not a slide joint.");
	cpConstraintActivateBodies(constraint);
	((cpSlideJoint *)constraint)->anchorB = anchorB;
}

float
cpSlideJointGetMin(const cpConstraint *constraint)
{
	cpAssertHard(cpConstraintIsSlideJoint(constraint), "Constraint is not a slide joint.");
	return ((cpSlideJoint *)constraint)->min;
}

void
cpSlideJointSetMin(cpConstraint *constraint, float min)
{
	cpAssertHard(cpConstraintIsSlideJoint(constraint), "Constraint is not a slide joint.");
	cpConstraintActivateBodies(constraint);
	((cpSlideJoint *)constraint)->min = min;
}

float
cpSlideJointGetMax(const cpConstraint *constraint)
{
	cpAssertHard(cpConstraintIsSlideJoint(constraint), "Constraint is not a slide joint.");
	return ((cpSlideJoint *)constraint)->max;
}

void
cpSlideJointSetMax(cpConstraint *constraint, float max)
{
	cpAssertHard(cpConstraintIsSlideJoint(constraint), "Constraint is not a slide joint.");
	cpConstraintActivateBodies(constraint);
	((cpSlideJoint *)constraint)->max = max;
}
