#include "generators/elements/tank_body_generator.h"
#include "generators/elements/tank_top_generator.h"
#include "khg_phy/phy_types.h"

bool tank_body_at_position(tank_body *tb, cpVect pos);
void tank_body_set_speed(tank_body *tb, cpFloat vel);
void tank_body_set_rotation_speed(tank_body *tb, cpFloat ang_vel);
void tank_body_rotate_to_position(tank_body *tb, tank_top *tt, cpVect pos, cpFloat ang_vel);
void tank_body_move_to_position(tank_body *tb, tank_top *tt, cpVect pos, cpFloat vel);

