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

static float
defaultSpringForce(cpDampedSpring *spring, float dist){
	return (spring->restLength - dist)*spring->stiffness;
}

static void
preStep(cpDampedSpring *spring, float dt)
{
	cpBody *a = spring->constraint.a;
	cpBody *b = spring->constraint.b;
	
	spring->r1 = cpTransformVect(a->transform, cpvsub(spring->anchorA, a->cog));
	spring->r2 = cpTransformVect(b->transform, cpvsub(spring->anchorB, b->cog));
	
	cpVect delta = cpvsub(cpvadd(b->p, spring->r2), cpvadd(a->p, spring->r1));
	float dist = cpvlength(delta);
	spring->n = cpvmult(delta, 1.0f/(dist ? dist : INFINITY));
	
	float k = k_scalar(a, b, spring->r1, spring->r2, spring->n);
	cpAssertSoft(k != 0.0, "Unsolvable spring.");
	spring->nMass = 1.0f/k;
	
	spring->target_vrn = 0.0f;
	spring->v_coef = 1.0f - cpfexp(-spring->damping*dt*k);

	// apply spring force
	float f_spring = spring->springForceFunc((cpConstraint *)spring, dist);
	float j_spring = spring->jAcc = f_spring*dt;
	apply_impulses(a, b, spring->r1, spring->r2, cpvmult(spring->n, j_spring));
}

static void applyCachedImpulse(cpDampedSpring *spring, float dt_coef){}

static void
applyImpulse(cpDampedSpring *spring, float dt)
{
	cpBody *a = spring->constraint.a;
	cpBody *b = spring->constraint.b;
	
	cpVect n = spring->n;
	cpVect r1 = spring->r1;
	cpVect r2 = spring->r2;

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
getImpulse(cpDampedSpring *spring)
{
	return spring->jAcc;
}

static const cpConstraintClass klass = {
	(cpConstraintPreStepImpl)preStep,
	(cpConstraintApplyCachedImpulseImpl)applyCachedImpulse,
	(cpConstraintApplyImpulseImpl)applyImpulse,
	(cpConstraintGetImpulseImpl)getImpulse,
};

cpDampedSpring *
cpDampedSpringAlloc(void)
{
	return (cpDampedSpring *)cpcalloc(1, sizeof(cpDampedSpring));
}

cpDampedSpring *
cpDampedSpringInit(cpDampedSpring *spring, cpBody *a, cpBody *b, cpVect anchorA, cpVect anchorB, float restLength, float stiffness, float damping)
{
	cpConstraintInit((cpConstraint *)spring, &klass, a, b);
	
	spring->anchorA = anchorA;
	spring->anchorB = anchorB;
	
	spring->restLength = restLength;
	spring->stiffness = stiffness;
	spring->damping = damping;
	spring->springForceFunc = (cpDampedSpringForceFunc)defaultSpringForce;
	
	spring->jAcc = 0.0f;
	
	return spring;
}

cpConstraint *
cpDampedSpringNew(cpBody *a, cpBody *b, cpVect anchorA, cpVect anchorB, float restLength, float stiffness, float damping)
{
	return (cpConstraint *)cpDampedSpringInit(cpDampedSpringAlloc(), a, b, anchorA, anchorB, restLength, stiffness, damping);
}

cpBool
cpConstraintIsDampedSpring(const cpConstraint *constraint)
{
	return (constraint->klass == &klass);
}

cpVect
cpDampedSpringGetAnchorA(const cpConstraint *constraint)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	return ((cpDampedSpring *)constraint)->anchorA;
}

void
cpDampedSpringSetAnchorA(cpConstraint *constraint, cpVect anchorA)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	cpConstraintActivateBodies(constraint);
	((cpDampedSpring *)constraint)->anchorA = anchorA;
}

cpVect
cpDampedSpringGetAnchorB(const cpConstraint *constraint)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	return ((cpDampedSpring *)constraint)->anchorB;
}

void
cpDampedSpringSetAnchorB(cpConstraint *constraint, cpVect anchorB)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	cpConstraintActivateBodies(constraint);
	((cpDampedSpring *)constraint)->anchorB = anchorB;
}

float
cpDampedSpringGetRestLength(const cpConstraint *constraint)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	return ((cpDampedSpring *)constraint)->restLength;
}

void
cpDampedSpringSetRestLength(cpConstraint *constraint, float restLength)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	cpConstraintActivateBodies(constraint);
	((cpDampedSpring *)constraint)->restLength = restLength;
}

float
cpDampedSpringGetStiffness(const cpConstraint *constraint)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	return ((cpDampedSpring *)constraint)->stiffness;
}

void
cpDampedSpringSetStiffness(cpConstraint *constraint, float stiffness)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	cpConstraintActivateBodies(constraint);
	((cpDampedSpring *)constraint)->stiffness = stiffness;
}

float
cpDampedSpringGetDamping(const cpConstraint *constraint)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	return ((cpDampedSpring *)constraint)->damping;
}

void
cpDampedSpringSetDamping(cpConstraint *constraint, float damping)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	cpConstraintActivateBodies(constraint);
	((cpDampedSpring *)constraint)->damping = damping;
}

cpDampedSpringForceFunc
cpDampedSpringGetSpringForceFunc(const cpConstraint *constraint)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	return ((cpDampedSpring *)constraint)->springForceFunc;
}

void
cpDampedSpringSetSpringForceFunc(cpConstraint *constraint, cpDampedSpringForceFunc springForceFunc)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	cpConstraintActivateBodies(constraint);
	((cpDampedSpring *)constraint)->springForceFunc = springForceFunc;
}
