#include "khg_phy/phy_private.h"

// TODO: Comment me!

void cpConstraintDestroy(phy_constraint *constraint){}

void
cpConstraintFree(phy_constraint *constraint)
{
	if(constraint){
		cpConstraintDestroy(constraint);
		free(constraint);
	}
}

void
cpConstraintInit(phy_constraint *constraint, const cpConstraintClass *klass, phy_body *a, phy_body *b)
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

phy_space *
cpConstraintGetSpace(const phy_constraint *constraint)
{
	return constraint->space;
}

phy_body *
cpConstraintGetBodyA(const phy_constraint *constraint)
{
	return constraint->a;
}

phy_body *
cpConstraintGetBodyB(const phy_constraint *constraint)
{
	return constraint->b;
}

float
cpConstraintGetMaxForce(const phy_constraint *constraint)
{
	return constraint->maxForce;
}

void
cpConstraintSetMaxForce(phy_constraint *constraint, float maxForce)
{
	cpAssertHard(maxForce >= 0.0f, "maxForce must be positive.");
	cpConstraintActivateBodies(constraint);
	constraint->maxForce = maxForce;
}

float
cpConstraintGetErrorBias(const phy_constraint *constraint)
{
	return constraint->errorBias;
}

void
cpConstraintSetErrorBias(phy_constraint *constraint, float errorBias)
{
	cpAssertHard(errorBias >= 0.0f, "errorBias must be positive.");
	cpConstraintActivateBodies(constraint);
	constraint->errorBias = errorBias;
}

float
cpConstraintGetMaxBias(const phy_constraint *constraint)
{
	return constraint->maxBias;
}

void
cpConstraintSetMaxBias(phy_constraint *constraint, float maxBias)
{
	cpAssertHard(maxBias >= 0.0f, "maxBias must be positive.");
	cpConstraintActivateBodies(constraint);
	constraint->maxBias = maxBias;
}

bool
cpConstraintGetCollideBodies(const phy_constraint *constraint)
{
	return constraint->collideBodies;
}

void
cpConstraintSetCollideBodies(phy_constraint *constraint, bool collideBodies)
{
	cpConstraintActivateBodies(constraint);
	constraint->collideBodies = collideBodies;
}

cpConstraintPreSolveFunc
cpConstraintGetPreSolveFunc(const phy_constraint *constraint)
{
	return constraint->preSolve;
}

void
cpConstraintSetPreSolveFunc(phy_constraint *constraint, cpConstraintPreSolveFunc preSolveFunc)
{
	constraint->preSolve = preSolveFunc;
}

cpConstraintPostSolveFunc
cpConstraintGetPostSolveFunc(const phy_constraint *constraint)
{
	return constraint->postSolve;
}

void
cpConstraintSetPostSolveFunc(phy_constraint *constraint, cpConstraintPostSolveFunc postSolveFunc)
{
	constraint->postSolve = postSolveFunc;
}

phy_data_pointer
cpConstraintGetUserData(const phy_constraint *constraint)
{
	return constraint->userData;
}

void
cpConstraintSetUserData(phy_constraint *constraint, phy_data_pointer userData)
{
	constraint->userData = userData;
}


float
cpConstraintGetImpulse(phy_constraint *constraint)
{
	return constraint->klass->getImpulse(constraint);
}
