#include "ecs/comp_physics.h"
#include "ecs/ecs_manager.h"
#include "khg_phy/vector.h"
#include "physics/physics.h"
#include "utility/math_utl.h"
#include "khg_phy/body.h"
#include "khg_phy/space.h"
#include "khg_phy/vector.h"
#include <stdio.h>

ecs_id PHYSICS_COMPONENT_SIGNATURE;
ecs_id PHYSICS_SYSTEM_SIGNATURE;

comp_physics_constructor_info *PHYSICS_CONSTRUCTOR_INFO = NULL;

static ecs_ret sys_physics_update(ecs_ecs *ecs, ecs_id *entities, const int entity_count, const ecs_dt dt, void *udata) {
  if (dt == 0.0f) {
    return 0;
  }
  for (int id = 0; id < entity_count; id++) {
    comp_physics *info = ecs_get(ECS, entities[id], PHYSICS_COMPONENT_SIGNATURE);
    if (info->move_enabled) {
      nvRigidBody_set_linear_velocity(info->body, NV_VECTOR2(info->target_vel, 0.0f));
    }
  }
  return 0;
}

static void comp_physics_constructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr, void *args) {
  comp_physics *info = ptr;
  const comp_physics_constructor_info *constructor_info = PHYSICS_CONSTRUCTOR_INFO;
  if (info && constructor_info && constructor_info->mode == PHYSICS_BOX) {
    nvRigidBodyInitializer body_init = nvRigidBodyInitializer_default;
    body_init.type = nvRigidBodyType_DYNAMIC;
    body_init.position = NV_VECTOR2(0.0, 0.0);
    body_init.material = (nvMaterial){ .density=1.0, .restitution = 0.85, .friction = 0.0 };
    info->body = nvRigidBody_new(body_init);
    nvRigidBody_set_position(info->body, constructor_info->pos);
    nvRigidBody_set_mass(info->body, constructor_info->mass);
    nvRigidBody_set_angle(info->body, constructor_info->ang);
    info->is_moving = false;
    info->target_vel = 0.0f;
    info->move_enabled = constructor_info->move_enabled;
    info->shape = nvRectShape_new(constructor_info->width, constructor_info->height, NV_VECTOR2(0.0, 0.0));
    nvRigidBody_add_shape(info->body, info->shape);
    if (constructor_info->collision_enabled) {
      nvRigidBody_disable_collisions(info->body);
    }
    nvSpace_add_rigidbody(SPACE, info->body);
  }
}

static void comp_physics_destructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr) {
  const comp_physics *info = ptr;
  if (info) {
    nvSpace_remove_rigidbody(SPACE, info->body);
    nvRigidBody_remove_shape(info->body, info->shape);
    nvRigidBody_free(info->body);
    nvShape_free(info->shape);
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

