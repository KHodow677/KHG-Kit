#pragma once

#include "entity/comp_physics.h"
#include "khg_phy/phy_types.h"

void element_set_speed(physics_info *info, float vel);
void element_set_rotation_speed(physics_info *info, float ang_vel);

void element_rotate_to_position(physics_info *info, float max_vel, float body_ang, float target_ang, float easing_factor);
void element_move_to_position(physics_info *info, float max_vel, phy_vect body_pos, phy_vect target_pos, float easing_factor);

void element_set_position(physics_info *info, phy_vect pos);
void element_set_angle(physics_info *info, float target_ang);

bool element_is_targeting_position(physics_info *info, phy_vect pos, float tolerance);
bool element_is_targeting_position_default(physics_info *info, phy_vect pos);
bool element_is_at_position(physics_info *info, phy_vect pos, float tolerance);
bool element_is_at_position_default(physics_info *info, phy_vect pos);

void element_target_position(physics_info *info, phy_vect pos, float max_vel, float max_ang_vel);
void element_lock_on_position(physics_info *info, phy_vect pos, float max_ang_vel);

