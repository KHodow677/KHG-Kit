#include "ecs/comp_mover.h"
#include "ecs/comp_animator.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/ecs_manager.h"
#include "io/key_controller.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/vector.h"
#include <stdio.h>

static const float POSITION_TOLERANCE = 10.0f;
static const float SPEED_TOLERANCE = 60.0f;
static const float POSITION_SPEED_SCALE = 120.0f;
static const float POSITION_EASING = 3.0f;

ecs_id MOVER_COMPONENT_SIGNATURE;
ecs_id MOVER_SYSTEM_SIGNATURE;

comp_mover_constructor_info *MOVER_CONSTRUCTOR_INFO = NULL;

static void element_set_speed(comp_physics *p_info, const float vel) {
  p_info->target_vel = vel;
  p_info->is_moving = fabsf(vel) <= SPEED_TOLERANCE ? false : true;
}

static float ease_in_out(const float t) {
  return t * t * (3 - 2 * t);
}

static const bool element_is_at_position(comp_physics *info, const phy_vector2 pos, const float tolerance) {
  phy_vector2 body_pos = phy_rigid_body_get_position(info->body);
  float pos_diff = phy_vector2_dist(pos, body_pos);
  if (pos_diff <= tolerance) {
    return true;
  }
  return false;
}

static void element_set_position(comp_physics *info, const phy_vector2 pos) {
  element_set_speed(info, 0.0f);
  phy_rigid_body_set_position(info->body, pos);
}

static void element_move_to_position_x(comp_physics *info, const float max_vel, const phy_vector2 body_pos, const phy_vector2 target_pos, const float easing_factor) {
  float pos_diff = target_pos.x - body_pos.x;
  if (fabsf(pos_diff) < POSITION_TOLERANCE) {
    element_set_position(info, target_pos);
    return;
  }
  float speed = fmaxf(fminf(POSITION_SPEED_SCALE * pos_diff * easing_factor * (0.01f), max_vel), -max_vel);
  element_set_speed(info, speed);
}

static void element_target_position(comp_physics *info, const phy_vector2 pos, const float max_vel) {
  phy_vector2 body_pos = phy_rigid_body_get_position(info->body);
  if (element_is_at_position(info, pos, POSITION_TOLERANCE)) {
    element_set_speed(info, 0.0f);
    return;
  }
  else {
    element_move_to_position_x(info, max_vel, body_pos, pos, POSITION_EASING);
  }
}

static ecs_ret sys_mover_update(ecs_ecs *ecs, ecs_id *entities, const int entity_count, const ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    comp_mover *info = ecs_get(ECS, entities[id], MOVER_COMPONENT_SIGNATURE);
    comp_physics *p_info = ecs_get(ECS, entities[id], PHYSICS_COMPONENT_SIGNATURE);
    comp_renderer *r_info = ecs_get(ECS, entities[id], RENDERER_COMPONENT_SIGNATURE);
    comp_animator *a_info = ecs_get(ECS, entities[id], ANIMATOR_COMPONENT_SIGNATURE);
    if (KEYBOARD_STATE.a_key_is_down) {
      info->target.x -= info->target_vel * dt;
    }
    if (KEYBOARD_STATE.d_key_is_down) {
      info->target.x += info->target_vel * dt;
    }
    element_target_position(p_info, info->target, info->max_vel);
    if (p_info->is_moving) {
      a_info->min_tex_id = info->walk_min_tex_id;
      a_info->max_tex_id = info->walk_max_tex_id;
      r_info->offset = phy_vector2_new(-16.0f, -8.0f);
    }
    else {
      a_info->min_tex_id = info->idle_min_tex_id;
      a_info->max_tex_id = info->idle_max_tex_id;
      r_info->offset = phy_vector2_new(0.0f, 0.0f);
    }
  }
  return 0;
}

static void comp_mover_constructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr, void *args) {
  comp_mover *info = ptr;
  const comp_mover_constructor_info *constructor_info = MOVER_CONSTRUCTOR_INFO;
  if (info && constructor_info) {
    info->target = phy_rigid_body_get_position(constructor_info->body);
    info->target_vel = constructor_info->target_vel;
    info->max_vel = constructor_info->max_vel;
    info->idle_min_tex_id = constructor_info->idle_min_tex_id;
    info->idle_max_tex_id = constructor_info->idle_max_tex_id;
    info->walk_min_tex_id = constructor_info->walk_min_tex_id;
    info->walk_max_tex_id = constructor_info->walk_max_tex_id;
  }
}

void comp_mover_register() {
  MOVER_COMPONENT_SIGNATURE = ecs_register_component(ECS, sizeof(comp_mover), comp_mover_constructor, NULL);
}

void sys_mover_register() {
  MOVER_SYSTEM_SIGNATURE = ecs_register_system(ECS, sys_mover_update, NULL, NULL, NULL);
  ecs_require_component(ECS, MOVER_SYSTEM_SIGNATURE, MOVER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, MOVER_SYSTEM_SIGNATURE, PHYSICS_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, MOVER_SYSTEM_SIGNATURE, RENDERER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, MOVER_SYSTEM_SIGNATURE, ANIMATOR_COMPONENT_SIGNATURE);
}

comp_mover *sys_mover_add(const ecs_id eid, comp_mover_constructor_info *clci) {
  MOVER_CONSTRUCTOR_INFO = clci;
  return ecs_add(ECS, eid, MOVER_COMPONENT_SIGNATURE, NULL);
}

