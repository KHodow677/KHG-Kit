#include "controllers/elements/tank_top_controller.h"
#include "khg_phy/phy_types.h"

void tank_top_set_speed(tank_top *tt, cpFloat vel) {
  tt->physics_info.target_vel = vel;
}

void tank_top_set_rotation_speed(tank_top *tt, cpFloat ang_vel) {
  tt->physics_info.target_ang_vel = ang_vel;  
}

