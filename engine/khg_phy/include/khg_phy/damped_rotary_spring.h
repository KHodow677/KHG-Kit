#pragma once

#include "khg_phy/phy_types.h"

bool phy_constraint_is_damped_rotary_spring(const phy_constraint *constraint);

typedef float (*phy_damped_rotary_spring_torque_func)(struct phy_constraint *spring, float relative_angle);

phy_damped_rotary_spring *phy_damped_rotary_spring_alloc(void);
phy_damped_rotary_spring *phy_damped_rotary_spring_init(phy_damped_rotary_spring *joint, phy_body *a, phy_body *b, float rest_angle, float stiffness, float damping);
phy_constraint *phy_damped_rotary_spring_new(phy_body *a, phy_body *b, float rest_angle, float stiffness, float damping);

float phy_damped_rotary_spring_get_rest_angle(const phy_constraint *constraint);
void phy_damped_rotary_spring_set_rest_angle(phy_constraint *constraint, float rest_angle);

float phy_damped_rotary_spring_get_stiffness(const phy_constraint *constraint);
void phy_damped_rotary_spring_set_stiffness(phy_constraint *constraint, float stiffness);

float phy_damped_rotary_spring_get_damping(const phy_constraint *constraint);
void phy_damped_rotary_spring_set_damping(phy_constraint *constraint, float damping);

phy_damped_rotary_spring_torque_func phy_damped_rotary_spring_get_spring_torque_func(const phy_constraint *constraint);
void phy_damped_rotary_spring_set_spring_torque_func(phy_constraint *constraint, phy_damped_rotary_spring_torque_func spring_torque_func);

