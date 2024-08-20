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
defaultSpringTorque(phy_damped_rotary_spring *spring, float relativeAngle){
	return (relativeAngle - spring->restAngle)*spring->stiffness;
}

static void
preStep(phy_damped_rotary_spring *spring, float dt)
{
	phy_body *a = spring->constraint.a;
	phy_body *b = spring->constraint.b;
	
	float moment = a->i_inv + b->i_inv;
	cpAssertSoft(moment != 0.0, "Unsolvable spring.");
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
cpDampedRotarySpringAlloc(void)
{
	return (phy_damped_rotary_spring *)calloc(1, sizeof(phy_damped_rotary_spring));
}

phy_damped_rotary_spring *
cpDampedRotarySpringInit(phy_damped_rotary_spring *spring, phy_body *a, phy_body *b, float restAngle, float stiffness, float damping)
{
	cpConstraintInit((phy_constraint *)spring, &klass, a, b);
	
	spring->restAngle = restAngle;
	spring->stiffness = stiffness;
	spring->damping = damping;
	spring->springTorqueFunc = (cpDampedRotarySpringTorqueFunc)defaultSpringTorque;
	
	spring->jAcc = 0.0f;
	
	return spring;
}

phy_constraint *
cpDampedRotarySpringNew(phy_body *a, phy_body *b, float restAngle, float stiffness, float damping)
{
	return (phy_constraint *)cpDampedRotarySpringInit(cpDampedRotarySpringAlloc(), a, b, restAngle, stiffness, damping);
}

bool
cpConstraintIsDampedRotarySpring(const phy_constraint *constraint)
{
	return (constraint->klass == &klass);
}

float
cpDampedRotarySpringGetRestAngle(const phy_constraint *constraint)
{
	cpAssertHard(cpConstraintIsDampedRotarySpring(constraint), "Constraint is not a damped rotary spring.");
	return ((phy_damped_rotary_spring *)constraint)->restAngle;
}

void
cpDampedRotarySpringSetRestAngle(phy_constraint *constraint, float restAngle)
{
	cpAssertHard(cpConstraintIsDampedRotarySpring(constraint), "Constraint is not a damped rotary spring.");
	cpConstraintActivateBodies(constraint);
	((phy_damped_rotary_spring *)constraint)->restAngle = restAngle;
}

float
cpDampedRotarySpringGetStiffness(const phy_constraint *constraint)
{
	cpAssertHard(cpConstraintIsDampedRotarySpring(constraint), "Constraint is not a damped rotary spring.");
	return ((phy_damped_rotary_spring *)constraint)->stiffness;
}

void
cpDampedRotarySpringSetStiffness(phy_constraint *constraint, float stiffness)
{
	cpAssertHard(cpConstraintIsDampedRotarySpring(constraint), "Constraint is not a damped rotary spring.");
	cpConstraintActivateBodies(constraint);
	((phy_damped_rotary_spring *)constraint)->stiffness = stiffness;
}

float
cpDampedRotarySpringGetDamping(const phy_constraint *constraint)
{
	cpAssertHard(cpConstraintIsDampedRotarySpring(constraint), "Constraint is not a damped rotary spring.");
	return ((phy_damped_rotary_spring *)constraint)->damping;
}

void
cpDampedRotarySpringSetDamping(phy_constraint *constraint, float damping)
{
	cpAssertHard(cpConstraintIsDampedRotarySpring(constraint), "Constraint is not a damped rotary spring.");
	cpConstraintActivateBodies(constraint);
	((phy_damped_rotary_spring *)constraint)->damping = damping;
}

cpDampedRotarySpringTorqueFunc
cpDampedRotarySpringGetSpringTorqueFunc(const phy_constraint *constraint)
{
	cpAssertHard(cpConstraintIsDampedRotarySpring(constraint), "Constraint is not a damped rotary spring.");
	return ((phy_damped_rotary_spring *)constraint)->springTorqueFunc;
}

void
cpDampedRotarySpringSetSpringTorqueFunc(phy_constraint *constraint, cpDampedRotarySpringTorqueFunc springTorqueFunc)
{
	cpAssertHard(cpConstraintIsDampedRotarySpring(constraint), "Constraint is not a damped rotary spring.");
	cpConstraintActivateBodies(constraint);
	((phy_damped_rotary_spring *)constraint)->springTorqueFunc = springTorqueFunc;
}
