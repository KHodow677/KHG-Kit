#include "controllers/elements/element_controller.h"
#include "data_utl/kinematic_utl.h"
#include "entity/comp_physics.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include <math.h>
#include <stdbool.h>

void element_set_speed(physics_info *info, cpFloat vel) {
  info->target_vel = vel;
  info->is_moving = vel == 0.0f ? false : true;
}

void element_set_rotation_speed(physics_info *info, cpFloat ang_vel) {
  info->target_ang_vel = ang_vel;  
  info->is_turning = ang_vel == 0.0f ? false : true;
}

void element_rotate_to_position(physics_info *info, cpFloat ang_vel, float angle_diff) {
  float r_speed = angle_diff < M_PI ? ang_vel : -ang_vel;
  element_set_rotation_speed(info, r_speed);
  return;
}

void element_move_to_position(physics_info *info, cpVect pos, cpVect body_pos, cpFloat vel) {
  float pos_diff = cpvdist(pos, body_pos);
  element_set_speed(info, vel);
}

void element_set_position(physics_info *info, cpVect pos, cpVect body_pos) {
  element_set_speed(info, 0.0f);
  cpBodySetPosition(info->body, pos);
}

void element_set_angle(physics_info *info, float target_ang) {
  element_set_rotation_speed(info, 0.0f);
  cpBodySetAngle(info->body, target_ang);
}

bool element_is_targeting_position(physics_info *info, cpVect pos) {
  cpVect body_pos = cpBodyGetPosition(info->body);
  float body_ang = normalize_angle(cpBodyGetAngle(info->body));
  float target_ang = normalize_angle(atan2f(body_pos.y - pos.y, body_pos.x - pos.x) - M_PI / 2);
  float angle_diff = normalize_angle(target_ang - body_ang);
  return angle_diff <= 0.05f;
}

bool element_is_at_position(physics_info *info, cpVect pos, float tolerance) {
  cpVect body_pos = cpBodyGetPosition(info->body);
  float pos_diff = cpvdist(pos, body_pos);
  if (pos_diff <= tolerance) {
    return true;
  }
  return false;
}

void element_target_position(physics_info *info, cpVect pos, float vel, float ang_vel) {
  cpVect body_pos = cpBodyGetPosition(info->body);
  float body_ang = normalize_angle(cpBodyGetAngle(info->body));
  float target_ang = normalize_angle(atan2f(body_pos.y - pos.y, body_pos.x - pos.x) - M_PI / 2);
  float angle_diff = normalize_angle(target_ang - body_ang);
  if (element_is_at_position(info, pos, 1.0f)) {
    element_set_speed(info, 0.0f);
    element_set_rotation_speed(info, 0.0f);
    return;
  }
  if (!element_is_targeting_position(info, pos) && !element_is_at_position(info, pos, 1.0f)) {
    element_rotate_to_position(info, ang_vel, angle_diff);
    element_move_to_position(info, pos, body_pos, vel);
  }
  else {
    if (!element_is_at_position(info, pos, 1.0f)) {
      element_set_angle(info, target_ang);
    }
    element_move_to_position(info, pos, body_pos, vel);
  }
}

void element_lock_on_position(physics_info *info, cpVect pos, cpFloat ang_vel) {
  cpVect body_pos = cpBodyGetPosition(info->body);
  float body_ang = normalize_angle(cpBodyGetAngle(info->body));
  float target_ang = normalize_angle(atan2f(body_pos.y - pos.y, body_pos.x - pos.x) - M_PI / 2);
  float angle_diff = normalize_angle(target_ang - body_ang);
  if (cpvdist(body_pos, pos) <= 50.0f) {
    return;
  }
  if (!element_is_targeting_position(info, pos)) {
    element_rotate_to_position(info, ang_vel, angle_diff);
  }
  else {
    info->is_locked_on = true;
    element_set_angle(info, target_ang);
  }
}

