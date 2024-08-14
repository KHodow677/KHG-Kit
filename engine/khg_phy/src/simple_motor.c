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
preStep(cpSimpleMotor *joint, float dt)
{
	cpBody *a = joint->constraint.a;
	cpBody *b = joint->constraint.b;
	
	// calculate moment of inertia coefficient.
	joint->iSum = 1.0f/(a->i_inv + b->i_inv);
}

static void
applyCachedImpulse(cpSimpleMotor *joint, float dt_coef)
{
	cpBody *a = joint->constraint.a;
	cpBody *b = joint->constraint.b;
	
	float j = joint->jAcc*dt_coef;
	a->w -= j*a->i_inv;
	b->w += j*b->i_inv;
}

static void
applyImpulse(cpSimpleMotor *joint, float dt)
{
	cpBody *a = joint->constraint.a;
	cpBody *b = joint->constraint.b;
	
	// compute relative rotational velocity
	float wr = b->w - a->w + joint->rate;
	
	float jMax = joint->constraint.maxForce*dt;
	
	// compute normal impulse	
	float j = -wr*joint->iSum;
	float jOld = joint->jAcc;
	joint->jAcc = phy_clamp(jOld + j, -jMax, jMax);
	j = joint->jAcc - jOld;
	
	// apply impulse
	a->w -= j*a->i_inv;
	b->w += j*b->i_inv;
}

static float
getImpulse(cpSimpleMotor *joint)
{
	return phy_abs(joint->jAcc);
}

static const cpConstraintClass klass = {
	(cpConstraintPreStepImpl)preStep,
	(cpConstraintApplyCachedImpulseImpl)applyCachedImpulse,
	(cpConstraintApplyImpulseImpl)applyImpulse,
	(cpConstraintGetImpulseImpl)getImpulse,
};

cpSimpleMotor *
cpSimpleMotorAlloc(void)
{
	return (cpSimpleMotor *)cpcalloc(1, sizeof(cpSimpleMotor));
}

cpSimpleMotor *
cpSimpleMotorInit(cpSimpleMotor *joint, cpBody *a, cpBody *b, float rate)
{
	cpConstraintInit((cpConstraint *)joint, &klass, a, b);
	
	joint->rate = rate;
	
	joint->jAcc = 0.0f;
	
	return joint;
}

cpConstraint *
cpSimpleMotorNew(cpBody *a, cpBody *b, float rate)
{
	return (cpConstraint *)cpSimpleMotorInit(cpSimpleMotorAlloc(), a, b, rate);
}

bool
cpConstraintIsSimpleMotor(const cpConstraint *constraint)
{
	return (constraint->klass == &klass);
}

float
cpSimpleMotorGetRate(const cpConstraint *constraint)
{
	cpAssertHard(cpConstraintIsSimpleMotor(constraint), "Constraint is not a SimpleMotor.");
	return ((cpSimpleMotor *)constraint)->rate;
}

void
cpSimpleMotorSetRate(cpConstraint *constraint, float rate)
{
	cpAssertHard(cpConstraintIsSimpleMotor(constraint), "Constraint is not a SimpleMotor.");
	cpConstraintActivateBodies(constraint);
	((cpSimpleMotor *)constraint)->rate = rate;
}
