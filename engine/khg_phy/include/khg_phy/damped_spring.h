#pragma once

#include "khg_phy/phy_types.h"

bool phy_constraint_is_damped_spring(const phy_constraint *constraint);

typedef float (*phy_damped_spring_force_func)(phy_constraint *spring, float dist);

phy_damped_spring *phy_damped_spring_alloc(void);
phy_damped_spring *phy_damped_spring_init(phy_damped_spring *joint, phy_body *a, phy_body *b, phy_vect anchor_A, phy_vect anchor_B, float rest_length, float stiffness, float damping);
phy_constraint *phy_damped_spring_new(phy_body *a, phy_body *b, phy_vect anchor_A, phy_vect anchor_B, float restLength, float stiffness, float damping);

phy_vect phy_damped_spring_get_anchor_A(const phy_constraint *constraint);
void phy_damped_spring_set_anchor_A(phy_constraint *constraint, phy_vect anchor_A);

phy_vect phy_damped_spring_get_anchor_B(const phy_constraint *constraint);
void phy_damped_spring_set_anchor_B(phy_constraint *constraint, phy_vect anchorB);

float phy_damped_spring_get_rest_length(const phy_constraint *constraint);
void phy_damped_spring_set_rest_length(phy_constraint *constraint, float restLength);

float phy_damped_spring_get_stiffness(const phy_constraint *constraint);
void phy_damped_spring_set_stiffness(phy_constraint *constraint, float stiffness);

float phy_damped_spring_get_damping(const phy_constraint *constraint);
void phy_damped_spring_set_damping(phy_constraint *constraint, float damping);

phy_damped_spring_force_func phy_damped_spring_get_spring_force_func(const phy_constraint *constraint);
void phy_damped_spring_set_spring_force_func(phy_constraint *constraint, phy_damped_spring_force_func springForceFunc);

