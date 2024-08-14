#include "khg_phy/phy_private.h"

// TODO: Comment me!

void cpConstraintDestroy(cpConstraint *constraint){}

void
cpConstraintFree(cpConstraint *constraint)
{
	if(constraint){
		cpConstraintDestroy(constraint);
		cpfree(constraint);
	}
}

void
cpConstraintInit(cpConstraint *constraint, const cpConstraintClass *klass, cpBody *a, cpBody *b)
{
	constraint->klass = klass;
	
	constraint->a = a;
	constraint->b = b;
	constraint->space = NULL;
	
	constraint->next_a = NULL;
	constraint->next_b = NULL;
	
	constraint->maxForce = (float)INFINITY;
	constraint->errorBias = powf(1.0f - 0.1f, 60.0f);
	constraint->maxBias = (float)INFINITY;
	
	constraint->collideBodies = true;
	
	constraint->preSolve = NULL;
	constraint->postSolve = NULL;
}

cpSpace *
cpConstraintGetSpace(const cpConstraint *constraint)
{
	return constraint->space;
}

cpBody *
cpConstraintGetBodyA(const cpConstraint *constraint)
{
	return constraint->a;
}

cpBody *
cpConstraintGetBodyB(const cpConstraint *constraint)
{
	return constraint->b;
}

float
cpConstraintGetMaxForce(const cpConstraint *constraint)
{
	return constraint->maxForce;
}

void
cpConstraintSetMaxForce(cpConstraint *constraint, float maxForce)
{
	cpAssertHard(maxForce >= 0.0f, "maxForce must be positive.");
	cpConstraintActivateBodies(constraint);
	constraint->maxForce = maxForce;
}

float
cpConstraintGetErrorBias(const cpConstraint *constraint)
{
	return constraint->errorBias;
}

void
cpConstraintSetErrorBias(cpConstraint *constraint, float errorBias)
{
	cpAssertHard(errorBias >= 0.0f, "errorBias must be positive.");
	cpConstraintActivateBodies(constraint);
	constraint->errorBias = errorBias;
}

float
cpConstraintGetMaxBias(const cpConstraint *constraint)
{
	return constraint->maxBias;
}

void
cpConstraintSetMaxBias(cpConstraint *constraint, float maxBias)
{
	cpAssertHard(maxBias >= 0.0f, "maxBias must be positive.");
	cpConstraintActivateBodies(constraint);
	constraint->maxBias = maxBias;
}

bool
cpConstraintGetCollideBodies(const cpConstraint *constraint)
{
	return constraint->collideBodies;
}

void
cpConstraintSetCollideBodies(cpConstraint *constraint, bool collideBodies)
{
	cpConstraintActivateBodies(constraint);
	constraint->collideBodies = collideBodies;
}

cpConstraintPreSolveFunc
cpConstraintGetPreSolveFunc(const cpConstraint *constraint)
{
	return constraint->preSolve;
}

void
cpConstraintSetPreSolveFunc(cpConstraint *constraint, cpConstraintPreSolveFunc preSolveFunc)
{
	constraint->preSolve = preSolveFunc;
}

cpConstraintPostSolveFunc
cpConstraintGetPostSolveFunc(const cpConstraint *constraint)
{
	return constraint->postSolve;
}

void
cpConstraintSetPostSolveFunc(cpConstraint *constraint, cpConstraintPostSolveFunc postSolveFunc)
{
	constraint->postSolve = postSolveFunc;
}

cpDataPointer
cpConstraintGetUserData(const cpConstraint *constraint)
{
	return constraint->userData;
}

void
cpConstraintSetUserData(cpConstraint *constraint, cpDataPointer userData)
{
	constraint->userData = userData;
}


float
cpConstraintGetImpulse(cpConstraint *constraint)
{
	return constraint->klass->getImpulse(constraint);
}
