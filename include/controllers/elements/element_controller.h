#pragma once

#include "entity/comp_physics.h"
#include "khg_phy/phy_types.h"

void element_set_speed(comp_physics *info, float vel);
void element_set_rotation_speed(comp_physics *info, float ang_vel);

void element_rotate_to_position(comp_physics *info, float max_vel, float body_ang, float target_ang, float easing_factor);
void element_move_to_position(comp_physics *info, float max_vel, phy_vect body_pos, phy_vect target_pos, float easing_factor);

void element_set_position(comp_physics *info, phy_vect pos);
void element_set_angle(comp_physics *info, float target_ang);

bool element_is_targeting_position(comp_physics *info, phy_vect pos, float tolerance);
bool element_is_targeting_position_default(comp_physics *info, phy_vect pos);
bool element_is_at_position(comp_physics *info, phy_vect pos, float tolerance);
bool element_is_at_position_default(comp_physics *info, phy_vect pos);

void element_target_position(comp_physics *info, phy_vect pos, float max_vel, float max_ang_vel);
void element_lock_on_position(comp_physics *info, phy_vect pos, float max_ang_vel);

