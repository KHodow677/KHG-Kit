#include "khg_phy/phy_private.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

void phy_constraint_destroy(phy_constraint *constraint){}

void
phy_constraint_free(phy_constraint *constraint)
{
	if(constraint){
		phy_constraint_destroy(constraint);
		free(constraint);
	}
}

void
cp_constraint_init(phy_constraint *constraint, const phy_constraint_class *klass, phy_body *a, phy_body *b)
{
	constraint->class = klass;
	
	constraint->a = a;
	constraint->b = b;
	constraint->space = NULL;
	
	constraint->next_a = NULL;
	constraint->next_b = NULL;
	
	constraint->max_force = (float)INFINITY;
	constraint->error_bias = powf(1.0f - 0.1f, 60.0f);
	constraint->max_bias = (float)INFINITY;
	
	constraint->collide_bodies = true;
	
	constraint->pre_solve = NULL;
	constraint->post_solve = NULL;
}

phy_space *
phy_constraint_get_space(const phy_constraint *constraint)
{
	return constraint->space;
}

phy_body *
phy_constraint_get_body_A(const phy_constraint *constraint)
{
	return constraint->a;
}

phy_body *
phy_constraint_get_body_B(const phy_constraint *constraint)
{
	return constraint->b;
}

float
phy_constraint_get_max_force(const phy_constraint *constraint)
{
	return constraint->max_force;
}

void
phy_constraint_set_max_force(phy_constraint *constraint, float maxForce)
{
	if (maxForce < 0.0f) {
    utl_error_func("Max foce must be positive", utl_user_defined_data);
  }
	phy_constraint_activate_bodies(constraint);
	constraint->max_force = maxForce;
}

float
phy_constraint_get_error_bias(const phy_constraint *constraint)
{
	return constraint->error_bias;
}

void
phy_constraint_set_error_bias(phy_constraint *constraint, float errorBias)
{
	if (errorBias < 0.0f) {
	  utl_error_func("Bias must be positive", utl_user_defined_data);
  }
	phy_constraint_activate_bodies(constraint);
	constraint->error_bias = errorBias;
}

float
phy_constraint_get_max_bias(const phy_constraint *constraint)
{
	return constraint->max_bias;
}

void
phy_constraint_set_max_bias(phy_constraint *constraint, float maxBias)
{
	if (maxBias < 0.0f) {
	  utl_error_func("Bias must be positive", utl_user_defined_data);
  }
	phy_constraint_activate_bodies(constraint);
	constraint->max_bias = maxBias;
}

bool
phy_constraint_get_collide_bodies(const phy_constraint *constraint)
{
	return constraint->collide_bodies;
}

void
phy_constraint_set_collide_bodies(phy_constraint *constraint, bool collideBodies)
{
	phy_constraint_activate_bodies(constraint);
	constraint->collide_bodies = collideBodies;
}

phy_constraint_pre_solve_func
phy_constraint_get_pre_solve_func(const phy_constraint *constraint)
{
	return constraint->pre_solve;
}

void
phy_constraint_set_pre_solve_func(phy_constraint *constraint, phy_constraint_pre_solve_func preSolveFunc)
{
	constraint->pre_solve = preSolveFunc;
}

phy_constraint_post_solve_func
phy_constraint_get_post_solve_func(const phy_constraint *constraint)
{
	return constraint->post_solve;
}

void
phy_constraint_set_post_solve_func(phy_constraint *constraint, phy_constraint_post_solve_func postSolveFunc)
{
	constraint->post_solve = postSolveFunc;
}

phy_data_pointer
phy_constraint_get_user_data(const phy_constraint *constraint)
{
	return constraint->user_data;
}

void
phy_constraint_set_user_data(phy_constraint *constraint, phy_data_pointer userData)
{
	constraint->user_data = userData;
}

float
phy_constraint_get_impulse(phy_constraint *constraint)
{
	return constraint->class->get_impulse(constraint);
}

