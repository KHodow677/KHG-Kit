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
#include "khg_phy/phy_types.h"

static void
preStep(phy_simple_motor_joint *joint, float dt)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	// calculate moment of inertia coefficient.
	joint->iSum = 1.0f/(a->i_inv + b->i_inv);
}

static void
applyCachedImpulse(phy_simple_motor_joint *joint, float dt_coef)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	float j = joint->jAcc*dt_coef;
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
getImpulse(phy_simple_motor_joint *joint)
{
	return phy_abs(joint->jAcc);
}

static const cpConstraintClass klass = {
	(cpConstraintPreStepImpl)preStep,
	(cpConstraintApplyCachedImpulseImpl)applyCachedImpulse,
	(cpConstraintApplyImpulseImpl)applyImpulse,
	(cpConstraintGetImpulseImpl)getImpulse,
};

phy_simple_motor_joint *
cpSimpleMotorAlloc(void)
{
	return (phy_simple_motor_joint *)calloc(1, sizeof(phy_simple_motor_joint));
}

phy_simple_motor_joint *
cpSimpleMotorInit(phy_simple_motor_joint *joint, phy_body *a, phy_body *b, float rate)
{
	cpConstraintInit((phy_constraint *)joint, &klass, a, b);
	
	joint->rate = rate;
	
	joint->jAcc = 0.0f;
	
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
	return (constraint->klass == &klass);
}

float
cpSimpleMotorGetRate(const phy_constraint *constraint)
{
	cpAssertHard(cpConstraintIsSimpleMotor(constraint), "Constraint is not a SimpleMotor.");
	return ((phy_simple_motor_joint *)constraint)->rate;
}

void
cpSimpleMotorSetRate(phy_constraint *constraint, float rate)
{
	cpAssertHard(cpConstraintIsSimpleMotor(constraint), "Constraint is not a SimpleMotor.");
	cpConstraintActivateBodies(constraint);
	((phy_simple_motor_joint *)constraint)->rate = rate;
}
