#include "controllers/elements/tank_body_controller.h"
#include "data_utl/kinematic_utl.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include <math.h>
#include <stdbool.h>

void tank_body_set_speed(tank_body *tb, cpFloat vel) {
  tb->physics_info.target_vel = vel;
  tb->physics_info.is_moving = vel == 0.0f ? false : true;
}

void tank_body_set_rotation_speed(tank_body *tb, cpFloat ang_vel) {
  tb->physics_info.target_ang_vel = ang_vel;  
  tb->physics_info.is_turning = ang_vel == 0.0f ? false : true;
}

void tank_body_rotate_to_position(tank_body *tb, cpFloat ang_vel, float angle_diff) {
  float r_speed = angle_diff < M_PI ? ang_vel : -ang_vel;
  tank_body_set_rotation_speed(tb, r_speed);
  return;
}

void tank_body_move_to_position(tank_body *tb, cpVect pos, cpVect body_pos, cpFloat vel) {
  float pos_diff = cpvdist(pos, body_pos);
  tank_body_set_speed(tb, vel);
}

void tank_body_set_position(tank_body *tb, cpVect pos, cpVect body_pos) {
  tank_body_set_speed(tb, 0.0f);
  cpVect offset = cpvsub(pos, body_pos);
  cpBodySetPosition(tb->physics_info.body, cpvadd(body_pos, offset));
}

void tank_body_set_angle(tank_body *tb, float target_ang) {
  tank_body_set_rotation_speed(tb, 0.0f);
  float body_ang = normalize_angle(cpBodyGetAngle(tb->physics_info.body));
  float angle_diff = normalize_angle(target_ang - body_ang);
  angle_diff = angle_diff < M_PI ? angle_diff : -angle_diff;
  cpBodySetAngle(tb->physics_info.body, body_ang + angle_diff);
}

bool tank_body_is_targeting_position(tank_body *tb, cpVect pos) {
  cpVect body_pos = cpBodyGetPosition(tb->physics_info.body);
  float body_ang = normalize_angle(cpBodyGetAngle(tb->physics_info.body));
  float target_ang = normalize_angle(atan2f(body_pos.y - pos.y, body_pos.x - pos.x) - M_PI / 2);
  float angle_diff = normalize_angle(target_ang - body_ang);
  return angle_diff <= 0.05f;
}

bool tank_body_is_at_position(tank_body *tb, cpVect pos, float tolerance) {
  cpVect body_pos = cpBodyGetPosition(tb->physics_info.body);
  float pos_diff = cpvdist(pos, body_pos);
  if (pos_diff <= tolerance) {
    return true;
  }
  return false;
}

void tank_body_target_position(tank_body *tb, cpVect pos, float vel, float ang_vel) {
  cpVect body_pos = cpBodyGetPosition(tb->physics_info.body);
  float body_ang = normalize_angle(cpBodyGetAngle(tb->physics_info.body));
  float target_ang = normalize_angle(atan2f(body_pos.y - pos.y, body_pos.x - pos.x) - M_PI / 2);
  float angle_diff = normalize_angle(target_ang - body_ang);
  if (tank_body_is_at_position(tb, pos, 1.0f)) {
    tank_body_set_speed(tb, 0.0f);
    tank_body_set_rotation_speed(tb, 0.0f);
    return;
  }
  if (!tank_body_is_targeting_position(tb, pos) && !tank_body_is_at_position(tb, pos, 1.0f)) {
    tank_body_rotate_to_position(tb, ang_vel, angle_diff);
    tank_body_move_to_position(tb, pos, body_pos, vel);
  }
  else {
    if (!tank_body_is_at_position(tb, pos, 1.0f)) {
      tank_body_set_angle(tb, target_ang);
    }
    tank_body_move_to_position(tb, pos, body_pos, vel);
  }
}

