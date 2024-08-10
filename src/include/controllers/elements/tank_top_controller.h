#include "generators/elements/tank_top_generator.h"
#include "khg_phy/phy_types.h"

void tank_top_set_speed(tank_top *tt, cpFloat vel);
void tank_top_set_rotation_speed(tank_top *tt, cpFloat ang_vel);

void tank_top_rotate_to_position(tank_top *tt, cpFloat ang_vel, float angle_diff);

void tank_top_set_angle(tank_top *tt, float target_ang);

bool tank_top_is_targetting_position(tank_top *tt, cpVect pos);

void tank_top_lock_on_position(tank_top *tt, cpVect pos, cpFloat ang_vel);

