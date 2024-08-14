#include "entity/comp_physics.h"
#include "khg_phy/phy_types.h"

void element_set_speed(physics_info *info, cpFloat vel);
void element_set_rotation_speed(physics_info *info, cpFloat ang_vel);

void element_rotate_to_position(physics_info *info, cpFloat ang_vel, float angle_diff);
void element_move_to_position(physics_info *info, cpVect pos, cpVect body_pos, cpFloat vel);

void element_set_position(physics_info *info, cpVect pos, cpVect body_pos);
void element_set_angle(physics_info *info, float target_ang);

bool element_is_targeting_position(physics_info *info, cpVect pos);
bool element_is_at_position(physics_info *info, cpVect pos, float tolerance);
void element_target_position(physics_info *info, cpVect pos, float vel, float ang_vel);
void element_lock_on_position(physics_info *info, cpVect pos, cpFloat ang_vel);

