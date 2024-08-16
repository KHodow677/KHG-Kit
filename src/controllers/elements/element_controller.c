#include "controllers/elements/element_controller.h"
#include "data_utl/kinematic_utl.h"
#include "entity/comp_physics.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include <math.h>
#include <stdbool.h>

float POSITION_TOLERANCE = 10.0f;
float ANGLE_TOLERANCE = 0.001f;
float POSITION_SPEED_SCALE = 50.0f;
float ROTATION_SPEED_SCALE = 5.0f;
float POSITION_EASING = 3.0f;
float ROTATION_EASING = 1.0f;

void element_set_speed(physics_info *info, float vel) {
  info->target_vel = vel;
  info->is_moving = vel == 0.0f ? false : true;
}

void element_set_rotation_speed(physics_info *info, float ang_vel) {
  info->target_ang_vel = ang_vel;  
  info->is_turning = ang_vel == 0.0f ? false : true;
}

float ease_in_out(float t) {
  return t * t * (3 - 2 * t);
}

void element_rotate_to_position(physics_info *info, float max_vel, float body_ang, float target_ang, float easing_factor) {
  float angle_diff = normalize_angle(target_ang - body_ang);
  if (angle_diff > M_PI) {
    angle_diff = angle_diff - 2 * M_PI;
  } 
  else if (angle_diff < -M_PI) {
    angle_diff = angle_diff + 2 * M_PI;
  }
  if (fabsf(angle_diff) < ANGLE_TOLERANCE) {
    element_set_angle(info, target_ang);
    return;
  }
  float r_speed = fminf(ROTATION_SPEED_SCALE * fabsf(angle_diff) * easing_factor, max_vel);
  r_speed = angle_diff < 0 ? -r_speed : r_speed;
  element_set_rotation_speed(info, r_speed);
}

void element_move_to_position(physics_info *info, float max_vel, cpVect body_pos, cpVect target_pos, float easing_factor) {
  float pos_diff = cpvdist(body_pos, target_pos);
  if (fabsf(pos_diff) < POSITION_TOLERANCE) {
    element_set_position(info, target_pos);
    return;
  }
  float speed = fminf(POSITION_SPEED_SCALE * fabsf(pos_diff) * easing_factor * (0.01f), max_vel);
  element_set_speed(info, speed);
}

void element_set_position(physics_info *info, cpVect pos) {
  element_set_speed(info, 0.0f);
  cpBodySetPosition(info->body, pos);
}

void element_set_angle(physics_info *info, float target_ang) {
  element_set_rotation_speed(info, 0.0f);
  cpBodySetAngle(info->body, target_ang);
}

bool element_is_targeting_position(physics_info *info, cpVect pos, float tolerance) {
  cpVect body_pos = cpBodyGetPosition(info->body);
  float body_ang = normalize_angle(cpBodyGetAngle(info->body));
  float target_ang = normalize_angle(atan2f(body_pos.y - pos.y, body_pos.x - pos.x) - M_PI / 2);
  float angle_diff = normalize_angle(target_ang - body_ang);
  if (angle_diff > M_PI) {
    angle_diff = angle_diff - 2 * M_PI;
  } 
  else if (angle_diff < -M_PI) {
    angle_diff = angle_diff + 2 * M_PI;
  }
  return fabsf(angle_diff) <= tolerance;
}

bool element_is_at_position(physics_info *info, cpVect pos, float tolerance) {
  cpVect body_pos = cpBodyGetPosition(info->body);
  float pos_diff = cpvdist(pos, body_pos);
  if (pos_diff <= tolerance) {
    return true;
  }
  return false;
}

void element_target_position(physics_info *info, cpVect pos, float max_vel, float max_ang_vel) {
  cpVect body_pos = cpBodyGetPosition(info->body);
  float body_ang = normalize_angle(cpBodyGetAngle(info->body));
  float target_ang = normalize_angle(atan2f(body_pos.y - pos.y, body_pos.x - pos.x) - M_PI / 2);
  float angle_diff = normalize_angle(target_ang - body_ang);
  if (element_is_at_position(info, pos, POSITION_TOLERANCE)) {
    element_set_speed(info, 0.0f);
    element_set_rotation_speed(info, 0.0f);
    return;
  }
  if (!element_is_targeting_position(info, pos, ANGLE_TOLERANCE) && !element_is_at_position(info, pos, POSITION_TOLERANCE)) {
    element_rotate_to_position(info, max_ang_vel, body_ang, target_ang, ROTATION_EASING);
    element_move_to_position(info, max_vel, pos, body_pos, POSITION_EASING);
  }
  else {
    element_set_rotation_speed(info, 0.0f);
    element_move_to_position(info, max_vel, body_pos, pos, POSITION_EASING);
  }
}

void element_lock_on_position(physics_info *info, cpVect pos, float max_ang_vel) {
  cpVect body_pos = cpBodyGetPosition(info->body);
  float body_ang = normalize_angle(cpBodyGetAngle(info->body));
  float target_ang = normalize_angle(atan2f(body_pos.y - pos.y, body_pos.x - pos.x) - M_PI / 2);
  float angle_diff = normalize_angle(target_ang - body_ang);
  if (!element_is_targeting_position(info, pos, ANGLE_TOLERANCE)) {
    element_rotate_to_position(info, max_ang_vel, body_ang, target_ang, ROTATION_EASING);
  }
  else {
    info->is_locked_on = true;
    element_set_angle(info, target_ang);
  }
}
