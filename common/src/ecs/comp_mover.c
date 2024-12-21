#include "ecs/comp_mover.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/ecs_manager.h"
#include "io/key_controller.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/contact.h"
#include "khg_phy/space.h"
#include "khg_utl/easing.h"
#include "physics/physics.h"
#include <stdio.h>

static const float POSITION_TOLERANCE = 10.0f;
static const float SPEED_TOLERANCE = 60.0f;
static const float POSITION_SPEED_SCALE = 120.0f;
static const float POSITION_EASING = 3.0f;

ecs_id MOVER_COMPONENT_SIGNATURE;
ecs_id MOVER_SYSTEM_SIGNATURE;

comp_mover_constructor_info *MOVER_CONSTRUCTOR_INFO = NULL;

static void player_set_speed(comp_physics *p_info, const float vel) {
  p_info->target_vel = vel;
  p_info->is_moving = fabsf(vel) <= SPEED_TOLERANCE ? false : true;
}

void player_on_collision(phy_space *space, phy_contact_event event, void *user_arg) {
  phy_rigid_body *player = (phy_rigid_body *)user_arg;
  if (event.body_a == player || event.body_b == player) {
    printf("Player is Body A\n");

  }
}

static ecs_ret sys_mover_update(ecs_ecs *ecs, ecs_id *entities, const size_t entity_count, const ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    comp_mover *info = ecs_get(ECS, entities[id], MOVER_COMPONENT_SIGNATURE);
    comp_physics *p_info = ecs_get(ECS, entities[id], PHYSICS_COMPONENT_SIGNATURE);
    comp_renderer *r_info = ecs_get(ECS, entities[id], RENDERER_COMPONENT_SIGNATURE);
    if (key_button_went_down(GLFW_KEY_D)) {
      r_info->flipped = false;
    }
    else if (key_button_went_down(GLFW_KEY_A)) {
      r_info->flipped = true;
    }
    bool d_key_pressed = key_button_is_down(GLFW_KEY_D);
    info->right_current_speed = info->max_speed * utl_easing_quadratic_ease_out(phy_fclamp(info->right_pressed_time / info->time_to_max_speed, 0.0f, 1.0f));
    info->right_pressed_time += d_key_pressed ? dt : -dt;
    info->right_pressed_time = phy_fclamp(info->right_pressed_time, 0.0f, info->time_to_max_speed);
    bool a_key_pressed = key_button_is_down(GLFW_KEY_A);
    info->left_current_speed = info->max_speed * utl_easing_quadratic_ease_out(phy_fclamp(info->left_pressed_time / info->time_to_max_speed, 0.0f, 1.0f));
    info->left_pressed_time += a_key_pressed ? dt : -dt;
    info->left_pressed_time = phy_fclamp(info->left_pressed_time, 0.0f, info->time_to_max_speed);
    player_set_speed(p_info, info->right_current_speed - info->left_current_speed);
  }
  return 0;
}

static void comp_mover_constructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr, void *args) {
  comp_mover *info = ptr;
  const comp_mover_constructor_info *constructor_info = MOVER_CONSTRUCTOR_INFO;
  if (info && constructor_info) {
    info->listener.on_contact_added = player_on_collision;
    phy_space_set_contact_listener(SPACE, info->listener, constructor_info->body);
    info->current_direction = MOVE_RIGHT;
    info->left_current_speed = 0.0f;
    info->right_current_speed = 0.0f;
    info->time_to_max_speed = constructor_info->time_to_max_speed;
    info->max_speed = constructor_info->max_speed;
    info->left_pressed_time = 0.0f;
    info->right_pressed_time = 0.0f;
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

