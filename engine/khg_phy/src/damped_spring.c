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
	cpAssertSoft(k != 0.0, "Unsolvable spring.");
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
cpDampedSpringAlloc(void)
{
	return (phy_damped_spring *)calloc(1, sizeof(phy_damped_spring));
}

phy_damped_spring *
cpDampedSpringInit(phy_damped_spring *spring, phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB, float restLength, float stiffness, float damping)
{
	cpConstraintInit((phy_constraint *)spring, &klass, a, b);
	
	spring->anchorA = anchorA;
	spring->anchorB = anchorB;
	
	spring->restLength = restLength;
	spring->stiffness = stiffness;
	spring->damping = damping;
	spring->springForceFunc = (cpDampedSpringForceFunc)defaultSpringForce;
	
	spring->jAcc = 0.0f;
	
	return spring;
}

phy_constraint *
cpDampedSpringNew(phy_body *a, phy_body *b, phy_vect anchorA, phy_vect anchorB, float restLength, float stiffness, float damping)
{
	return (phy_constraint *)cpDampedSpringInit(cpDampedSpringAlloc(), a, b, anchorA, anchorB, restLength, stiffness, damping);
}

bool
cpConstraintIsDampedSpring(const phy_constraint *constraint)
{
	return (constraint->klass == &klass);
}

phy_vect
cpDampedSpringGetAnchorA(const phy_constraint *constraint)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	return ((phy_damped_spring *)constraint)->anchorA;
}

void
cpDampedSpringSetAnchorA(phy_constraint *constraint, phy_vect anchorA)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	cpConstraintActivateBodies(constraint);
	((phy_damped_spring *)constraint)->anchorA = anchorA;
}

phy_vect
cpDampedSpringGetAnchorB(const phy_constraint *constraint)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	return ((phy_damped_spring *)constraint)->anchorB;
}

void
cpDampedSpringSetAnchorB(phy_constraint *constraint, phy_vect anchorB)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	cpConstraintActivateBodies(constraint);
	((phy_damped_spring *)constraint)->anchorB = anchorB;
}

float
cpDampedSpringGetRestLength(const phy_constraint *constraint)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	return ((phy_damped_spring *)constraint)->restLength;
}

void
cpDampedSpringSetRestLength(phy_constraint *constraint, float restLength)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	cpConstraintActivateBodies(constraint);
	((phy_damped_spring *)constraint)->restLength = restLength;
}

float
cpDampedSpringGetStiffness(const phy_constraint *constraint)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	return ((phy_damped_spring *)constraint)->stiffness;
}

void
cpDampedSpringSetStiffness(phy_constraint *constraint, float stiffness)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	cpConstraintActivateBodies(constraint);
	((phy_damped_spring *)constraint)->stiffness = stiffness;
}

float
cpDampedSpringGetDamping(const phy_constraint *constraint)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	return ((phy_damped_spring *)constraint)->damping;
}

void
cpDampedSpringSetDamping(phy_constraint *constraint, float damping)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	cpConstraintActivateBodies(constraint);
	((phy_damped_spring *)constraint)->damping = damping;
}

cpDampedSpringForceFunc
cpDampedSpringGetSpringForceFunc(const phy_constraint *constraint)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	return ((phy_damped_spring *)constraint)->springForceFunc;
}

void
cpDampedSpringSetSpringForceFunc(phy_constraint *constraint, cpDampedSpringForceFunc springForceFunc)
{
	cpAssertHard(cpConstraintIsDampedSpring(constraint), "Constraint is not a damped spring.");
	cpConstraintActivateBodies(constraint);
	((phy_damped_spring *)constraint)->springForceFunc = springForceFunc;
}
