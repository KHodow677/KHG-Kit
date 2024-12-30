#include "ecs/comp_physics.h"
#include "ecs/ecs_manager.h"
#include "khg_phy/core/phy_vector.h"
#include "physics/physics.h"
#include "utility/math_utl.h"
#include "khg_phy/body.h"
#include "khg_phy/space.h"
#include "khg_phy/core/phy_vector.h"
#include <stdio.h>

ecs_id PHYSICS_COMPONENT_SIGNATURE;
ecs_id PHYSICS_SYSTEM_SIGNATURE;

comp_physics_constructor_info *PHYSICS_CONSTRUCTOR_INFO = NULL;

static ecs_ret sys_physics_update(ecs_ecs *ecs, ecs_id *entities, const unsigned int entity_count, const ecs_dt dt, void *udata) {
  if (dt == 0.0f) {
    return 0;
  }
  for (int id = 0; id < entity_count; id++) {
    comp_physics *info = ecs_get(ECS, entities[id], PHYSICS_COMPONENT_SIGNATURE);
    if (info->move_enabled) {
      phy_vector2 current_velocity = phy_rigid_body_get_linear_velocity(info->body);
      phy_rigid_body_set_linear_velocity(info->body, phy_vector2_new(info->horizontal_vel, fabsf(info->vertical_vel) > 0.0f ? info->vertical_vel : current_velocity.y));
      info->vertical_vel = 0.0f;
      phy_rigid_body_set_angle(info->body, 0.0f);
    }
  }
  return 0;
}

static void comp_physics_constructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr, void *args) {
  comp_physics *info = ptr;
  const comp_physics_constructor_info *constructor_info = PHYSICS_CONSTRUCTOR_INFO;
  if (info && constructor_info && constructor_info->mode == PHYSICS_BOX) {
    phy_rigid_body_initializer body_init = phy_rigid_body_initializer_default;
    body_init.type = PHY_RIGID_BODY_TYPE_DYNAMIC;
    body_init.position = phy_vector2_new(0.0, 0.0);
    body_init.material = (phy_material){ .density=1.0, .restitution = 0.85, .friction = 0.0 };
    info->body = phy_rigid_body_new(body_init);
    phy_rigid_body_set_position(info->body, constructor_info->pos);
    phy_rigid_body_set_mass(info->body, constructor_info->mass);
    phy_rigid_body_set_angle(info->body, constructor_info->ang);
    phy_rigid_body_set_gravity_scale(info->body, constructor_info->gravity_enabled ? 1.0f : 0.0f);
    info->is_moving = false;
    info->horizontal_vel = 0.0f;
    info->vertical_vel = 0.0f;
    info->move_enabled = constructor_info->move_enabled;
    info->shape = phy_rect_shape_new(constructor_info->width, constructor_info->height, phy_vector2_zero);
    phy_rigid_body_add_shape(info->body, info->shape);
    if (constructor_info->collision_enabled) {
      phy_rigid_body_disable_collisions(info->body);
    }
    phy_space_add_rigidbody(SPACE, info->body);
  }
}

static void comp_physics_destructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr) {
  const comp_physics *info = ptr;
  if (info) {
    phy_space_remove_rigidbody(SPACE, info->body);
    phy_rigid_body_remove_shape(info->body, info->shape);
    phy_rigid_body_free(info->body);
    phy_shape_free(info->shape);
  }
}

void comp_physics_register() {
  PHYSICS_COMPONENT_SIGNATURE = ecs_register_component(ECS, sizeof(comp_physics), comp_physics_constructor, comp_physics_destructor);
}

void sys_physics_register() {
  PHYSICS_SYSTEM_SIGNATURE = ecs_register_system(ECS, sys_physics_update, NULL, NULL, NULL);
  ecs_require_component(ECS, PHYSICS_SYSTEM_SIGNATURE, PHYSICS_COMPONENT_SIGNATURE);
}

comp_physics *sys_physics_add(const ecs_id eid, comp_physics_constructor_info *cpci) {
  PHYSICS_CONSTRUCTOR_INFO = cpci;
  return ecs_add(ECS, eid, PHYSICS_COMPONENT_SIGNATURE, NULL);
}

