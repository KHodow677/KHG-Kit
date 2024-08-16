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
preStep(rotary_limit_joint *joint, float dt)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	float dist = b->a - a->a;
	float pdist = 0.0f;
	if(dist > joint->max) {
		pdist = joint->max - dist;
	} else if(dist < joint->min) {
		pdist = joint->min - dist;
	}
	
	// calculate moment of inertia coefficient.
	joint->iSum = 1.0f/(a->i_inv + b->i_inv);
	
	// calculate bias velocity
	float maxBias = joint->constraint.maxBias;
	joint->bias = phy_clamp(-bias_coef(joint->constraint.errorBias, dt)*pdist/dt, -maxBias, maxBias);

	// If the bias is 0, the joint is not at a limit. Reset the impulse.
	if(!joint->bias) joint->jAcc = 0.0f;
}

static void
applyCachedImpulse(rotary_limit_joint *joint, float dt_coef)
{
	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	float j = joint->jAcc*dt_coef;
	a->w -= j*a->i_inv;
	b->w += j*b->i_inv;
}

static void
applyImpulse(rotary_limit_joint *joint, float dt)
{
	if(!joint->bias) return; // early exit

	phy_body *a = joint->constraint.a;
	phy_body *b = joint->constraint.b;
	
	// compute relative rotational velocity
	float wr = b->w - a->w;
	
	float jMax = joint->constraint.maxForce*dt;
	
	// compute normal impulse	
	float j = -(joint->bias + wr)*joint->iSum;
	float jOld = joint->jAcc;
	if(joint->bias < 0.0f){
		joint->jAcc = phy_clamp(jOld + j, 0.0f, jMax);
	} else {
		joint->jAcc = phy_clamp(jOld + j, -jMax, 0.0f);
	}
	j = joint->jAcc - jOld;
	
	// apply impulse
	a->w -= j*a->i_inv;
	b->w += j*b->i_inv;
}

static float
getImpulse(rotary_limit_joint *joint)
{
	return phy_abs(joint->jAcc);
}

static const cpConstraintClass klass = {
	(cpConstraintPreStepImpl)preStep,
	(cpConstraintApplyCachedImpulseImpl)applyCachedImpulse,
	(cpConstraintApplyImpulseImpl)applyImpulse,
	(cpConstraintGetImpulseImpl)getImpulse,
};

rotary_limit_joint *
cpRotaryLimitJointAlloc(void)
{
	return (rotary_limit_joint *)calloc(1, sizeof(rotary_limit_joint));
}

rotary_limit_joint *
cpRotaryLimitJointInit(rotary_limit_joint *joint, phy_body *a, phy_body *b, float min, float max)
{
	cpConstraintInit((phy_constraint *)joint, &klass, a, b);
	
	joint->min = min;
	joint->max  = max;
	
	joint->jAcc = 0.0f;
	
	return joint;
}

phy_constraint *
cpRotaryLimitJointNew(phy_body *a, phy_body *b, float min, float max)
{
	return (phy_constraint *)cpRotaryLimitJointInit(cpRotaryLimitJointAlloc(), a, b, min, max);
}

bool
cpConstraintIsRotaryLimitJoint(const phy_constraint *constraint)
{
	return (constraint->klass == &klass);
}

float
cpRotaryLimitJointGetMin(const phy_constraint *constraint)
{
	cpAssertHard(cpConstraintIsRotaryLimitJoint(constraint), "Constraint is not a rotary limit joint.");
	return ((rotary_limit_joint *)constraint)->min;
}

void
cpRotaryLimitJointSetMin(phy_constraint *constraint, float min)
{
	cpAssertHard(cpConstraintIsRotaryLimitJoint(constraint), "Constraint is not a rotary limit joint.");
	cpConstraintActivateBodies(constraint);
	((rotary_limit_joint *)constraint)->min = min;
}

float
cpRotaryLimitJointGetMax(const phy_constraint *constraint)
{
	cpAssertHard(cpConstraintIsRotaryLimitJoint(constraint), "Constraint is not a rotary limit joint.");
	return ((rotary_limit_joint *)constraint)->max;
}

void
cpRotaryLimitJointSetMax(phy_constraint *constraint, float max)
{
	cpAssertHard(cpConstraintIsRotaryLimitJoint(constraint), "Constraint is not a rotary limit joint.");
	cpConstraintActivateBodies(constraint);
	((rotary_limit_joint *)constraint)->max = max;
}
