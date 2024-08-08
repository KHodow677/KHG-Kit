#include "controllers/elements/tank_body_controller.h"
#include "khg_phy/phy_types.h"

void tank_body_set_speed(tank_body *tb, cpFloat vel) {
  tb->physics_info.target_vel = vel;
}

void tank_body_set_rotation_speed(tank_body *tb, cpFloat ang_vel) {
  tb->physics_info.target_ang_vel = ang_vel;  
}

