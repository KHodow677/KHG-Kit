#include "controllers/elements/tank_top_controller.h"
#include "data_utl/kinematic_utl.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include <math.h>

void tank_top_set_speed(tank_top *tt, cpFloat vel) {
  tt->physics_info.target_vel = vel;
  tt->physics_info.is_moving = vel == 0.0f ? false : true;
}

void tank_top_set_rotation_speed(tank_top *tt, cpFloat ang_vel) {
  tt->physics_info.target_ang_vel = ang_vel;  
  tt->physics_info.is_turning = ang_vel == 0.0f ? false : true;
}

void tank_top_rotate_to_position(tank_top *tt, cpFloat ang_vel, float angle_diff) {
  float r_speed = angle_diff < M_PI ? ang_vel : -ang_vel;
  tank_top_set_rotation_speed(tt, r_speed);
  return;
}

void tank_top_set_angle(tank_top *tt, float target_ang) {
  tank_top_set_rotation_speed(tt, 0.0f);
  cpBodySetAngle(tt->physics_info.body, target_ang);
}

bool tank_top_is_targetting_position(tank_top *tt, cpVect pos) {
  cpVect body_pos = cpBodyGetPosition(tt->physics_info.body);
  float body_ang = normalize_angle(cpBodyGetAngle(tt->physics_info.body));
  float target_ang = normalize_angle(atan2f(body_pos.y - pos.y, body_pos.x - pos.x) - M_PI / 2);
  float angle_diff = normalize_angle(target_ang - body_ang);
  return angle_diff <= 0.05f;
}

void tank_top_lock_on_position(tank_top *tt, cpVect pos, cpFloat ang_vel) {
  cpVect body_pos = cpBodyGetPosition(tt->physics_info.body);
  float body_ang = normalize_angle(cpBodyGetAngle(tt->physics_info.body));
  float target_ang = normalize_angle(atan2f(body_pos.y - pos.y, body_pos.x - pos.x) - M_PI / 2);
  float angle_diff = normalize_angle(target_ang - body_ang);
  if (cpvdist(body_pos, pos) <= 50.0f) {
    return;
  }
  if (!tank_top_is_targetting_position(tt, pos)) {
    tank_top_rotate_to_position(tt, ang_vel, angle_diff);
  }
  else {
    tt->is_locked_on = true;
    tank_top_set_angle(tt, target_ang);
  }
}

