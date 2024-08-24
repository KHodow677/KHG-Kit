#pragma once

#include "khg_phy/phy_types.h"

typedef void (*phy_constraint_pre_solve_func)(phy_constraint *constraint, phy_space *space);
typedef void (*phy_constraint_post_solve_func)(phy_constraint *constraint, phy_space *space);

void phy_constraint_destroy(phy_constraint *constraint);
void phy_constraint_free(phy_constraint *constraint);

phy_space *phy_constraint_get_space(const phy_constraint *constraint);

phy_body *phy_constraint_get_body_A(const phy_constraint *constraint);
phy_body *phy_constraint_get_body_B(const phy_constraint *constraint);

float phy_constraint_get_max_force(const phy_constraint *constraint);
void phy_constraint_set_max_force(phy_constraint *constraint, float max_force);

float phy_constraint_get_error_bias(const phy_constraint *constraint);
void phy_constraint_set_error_bias(phy_constraint *constraint, float error_bias);

float phy_constraint_get_max_bias(const phy_constraint *constraint);
void phy_constraint_set_max_bias(phy_constraint *constraint, float max_bias);

bool phy_constraint_get_collide_bodies(const phy_constraint *constraint);
void phy_constraint_set_collide_bodies(phy_constraint *constraint, bool collide_bodies);

phy_constraint_pre_solve_func phy_constraint_get_pre_solve_func(const phy_constraint *constraint);
void phy_constraint_set_pre_solve_func(phy_constraint *constraint, phy_constraint_pre_solve_func pre_solve_func);

phy_constraint_post_solve_func phy_constraint_get_post_solve_func(const phy_constraint *constraint);
void phy_constraint_set_post_solve_func(phy_constraint *constraint, phy_constraint_post_solve_func post_solve_func);

phy_data_pointer phy_constraint_get_user_data(const phy_constraint *constraint);
void phy_constraint_set_user_data(phy_constraint *constraint, phy_data_pointer user_data);

float phy_constraint_get_impulse(phy_constraint *constraint);

