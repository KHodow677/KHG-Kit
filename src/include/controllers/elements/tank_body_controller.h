#include "generators/elements/tank_body_generator.h"
#include "khg_phy/phy_types.h"

bool tank_body_at_position(tank_body *tb, cpVect pos);
void tank_body_set_speed(tank_body *tb, cpFloat vel);
void tank_body_set_rotation_speed(tank_body *tb, cpFloat ang_vel);

void tank_body_rotate_to_position(tank_body *tb, cpFloat ang_vel, float angle_diff);
void tank_body_move_to_position(tank_body *tb, cpVect pos, cpVect body_pos, cpFloat vel);

void tank_body_set_position(tank_body *tb, cpVect pos, cpVect body_pos);
void tank_body_set_angle(tank_body *tb, float target_ang);

bool tank_body_is_targeting_position(tank_body *tb, cpVect pos);
bool tank_body_is_at_position(tank_body *tb, cpVect pos);
void tank_body_target_position(tank_body *tb, cpVect pos, float vel, float ang_vel);

