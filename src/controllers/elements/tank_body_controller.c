#include "controllers/elements/tank_body_controller.h"
#include "data_utl/kinematic_utl.h"
#include "generators/elements/tank_top_generator.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include <math.h>
#include <stdbool.h>

bool tank_body_at_position(tank_body *tb, cpVect pos) {
  return cpveql(cpBodyGetPosition(tb->physics_info.body), pos);
}

void tank_body_set_speed(tank_body *tb, cpFloat vel) {
  tb->physics_info.target_vel = vel;
  tb->physics_info.is_moving = vel == 0.0f ? false : true;
}

void tank_body_set_rotation_speed(tank_body *tb, cpFloat ang_vel) {
  tb->physics_info.target_ang_vel = ang_vel;  
  tb->physics_info.is_turning = ang_vel == 0.0f ? false : true;
}

void tank_body_rotate_to_position(tank_body *tb, tank_top *tt, cpVect pos, cpFloat ang_vel) {
  cpVect body_pos = cpBodyGetPosition(tb->physics_info.body);
  float body_ang = normalize_angle(cpBodyGetAngle(tb->physics_info.body));
  float target_ang = normalize_angle(atan2f(body_pos.y - pos.y, body_pos.x - pos.x) - M_PI / 2);
  float angle_diff = normalize_angle(target_ang - body_ang);
  float r_speed = angle_diff < M_PI ? ang_vel : -ang_vel;
  if (angle_diff <= 0.05f) {
    tank_body_set_rotation_speed(tb, 0.0f);
    angle_diff = r_speed > 0 ? angle_diff : -angle_diff;
    float top_ang = normalize_angle(cpBodyGetAngle(tt->physics_info.body));
    cpBodySetAngle(tb->physics_info.body, body_ang + angle_diff);
    cpBodySetAngle(tt->physics_info.body, top_ang + angle_diff);
    return;
  }
  tank_body_set_rotation_speed(tb, r_speed);
  return;
}

void tank_body_move_to_position(tank_body *tb, tank_top *tt, cpVect pos, cpFloat vel) {
  cpVect body_pos = cpBodyGetPosition(tb->physics_info.body);
  float pos_diff = cpvdist(pos, body_pos);
  if (pos_diff <= 1.0f) {
    tank_body_set_speed(tb, 0.0f);
    cpVect offset = cpvsub(pos, body_pos);
    cpVect top_pos = cpBodyGetPosition(tt->physics_info.body);
    cpBodySetPosition(tb->physics_info.body, pos);
    cpBodySetPosition(tt->physics_info.body, cpvadd(top_pos, offset));
    return;
  }
  tank_body_set_speed(tb, vel);
}

