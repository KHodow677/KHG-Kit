#include "ecs/comp_mover.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/contact.h"
#include "khg_utl/easing.h"
#include "util/io/key_controller.h"
#include "util/physics.h"
#include <stdio.h>

ecs_id MOVER_COMPONENT_SIGNATURE;
ecs_id MOVER_SYSTEM_SIGNATURE;

comp_mover_constructor_info *MOVER_CONSTRUCTOR_INFO = NULL;

static void player_set_speed(comp_physics *p_info, const float vel) {
  p_info->horizontal_vel = vel;
  p_info->is_moving = fabsf(vel) > 0.0f; 
}

static ecs_ret sys_mover_update(ecs_ecs *ecs, ecs_id *entities, const unsigned int entity_count, const ecs_dt dt, void *udata) {
  for (unsigned int id = 0; id < entity_count; id++) {
    comp_mover *info = ecs_get(ECS, entities[id], MOVER_COMPONENT_SIGNATURE);
    comp_physics *p_info = ecs_get(ECS, entities[id], PHYSICS_COMPONENT_SIGNATURE);
    comp_renderer *r_info = ecs_get(ECS, entities[id], RENDERER_COMPONENT_SIGNATURE);
    if (key_button_went_down(GLFW_KEY_D)) {
      r_info->flipped = false;
    }
    else if (key_button_went_down(GLFW_KEY_A)) {
      r_info->flipped = true;
    }
    info->right_current_speed = info->max_speed * utl_easing_quadratic_ease_out(phy_fclamp(info->right_pressed_time / info->time_to_max_speed, 0.0f, 1.0f));
    info->right_pressed_time += key_button_is_down(GLFW_KEY_D) ? dt : -dt;
    info->right_pressed_time = phy_fclamp(info->right_pressed_time, 0.0f, info->time_to_max_speed);
    info->left_current_speed = info->max_speed * utl_easing_quadratic_ease_out(phy_fclamp(info->left_pressed_time / info->time_to_max_speed, 0.0f, 1.0f));
    info->left_pressed_time += key_button_is_down(GLFW_KEY_A) ? dt : -dt;
    info->left_pressed_time = phy_fclamp(info->left_pressed_time, 0.0f, info->time_to_max_speed);
    player_set_speed(p_info, info->right_current_speed - info->left_current_speed);
    if (key_button_went_down(GLFW_KEY_W)) {
      p_info->vertical_vel = -500.0f;
    }
  }
  return 0;
}

static void comp_mover_constructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr, void *args) {
  comp_mover *info = ptr;
  const comp_mover_constructor_info *constructor_info = MOVER_CONSTRUCTOR_INFO;
  if (info && constructor_info) {
    info->current_direction = MOVE_RIGHT;
    info->left_current_speed = 0.0f;
    info->right_current_speed = 0.0f;
    info->time_to_max_speed = constructor_info->time_to_max_speed;
    info->max_speed = constructor_info->max_speed;
    info->left_pressed_time = 0.0f;
    info->right_pressed_time = 0.0f;
    COLLISION_INFO.player_body = constructor_info->body;
    COLLISION_INFO.player_on_ground = false;
  }
}

void mover_on_collision_added(phy_contact_event event, physics_collision_info *collision_info) {
  if (!collision_info) {
    return;
  }
  if (event.body_a == collision_info->player_body || event.body_b == collision_info->player_body) {
    printf("Player touched ground at (%f, %f)\n", collision_info->player_body->position.x, collision_info->player_body->position.y);
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
}

comp_mover *sys_mover_add(const ecs_id eid, comp_mover_constructor_info *clci) {
  MOVER_CONSTRUCTOR_INFO = clci;
  return ecs_add(ECS, eid, MOVER_COMPONENT_SIGNATURE, NULL);
}

