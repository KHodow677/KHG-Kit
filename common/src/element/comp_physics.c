#define NAMESPACE_ELEMENT_IMPL
#define NAMESPACE_KIN_USE

#include "element/namespace.h"
#include "khg_kin/namespace.h"
#include "util/physics.h"
#include <stdio.h>

static ecs_ret sys_physics_update(ecs_ecs *ecs, ecs_id *entities, const unsigned int entity_count, const ecs_dt dt, void *udata) {
  if (dt == 0.0f) {
    return 0;
  }
  for (unsigned int id = 0; id < entity_count; id++) {
    comp_physics *info = ecs_get(NAMESPACE_ELEMENT_INTERNAL.ECS, entities[id], NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.component_signature);
    info->body->lin_vel = info->vel;
    info->body->rot_scalar = 0.0f;
  }
  return 0;
}

static void comp_physics_constructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr, void *args) {
  comp_physics *info = ptr;
  const comp_physics *constructor_info = NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.init_info;
  if (info && constructor_info) {
    info->body = NAMESPACE_KIN()->body_create();
    NAMESPACE_KIN()->engine_body_add(ENGINE, info->body);
    info->body->pos = constructor_info->init_pos;
    info->body->rot_scalar = constructor_info->init_ang;
    info->body->mass = 1.0f;
    info->body->inertia = 1.0f;
    info->is_moving = false;
  }
}

static void comp_physics_destructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr) {
  comp_physics *info = ptr;
  if (info) {
    NAMESPACE_KIN()->engine_body_remove(ENGINE, info->body);
  }
}

void comp_physics_register() {
  NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.component_signature = ecs_register_component(NAMESPACE_ELEMENT_INTERNAL.ECS, sizeof(comp_physics), comp_physics_constructor, comp_physics_destructor);
}

void sys_physics_register() {
  NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.system_signature = ecs_register_system(NAMESPACE_ELEMENT_INTERNAL.ECS, sys_physics_update, NULL, NULL, NULL);
  ecs_require_component(NAMESPACE_ELEMENT_INTERNAL.ECS, NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.system_signature, NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.component_signature);
}

comp_physics *sys_physics_add(const ecs_id eid, comp_physics *cp) {
  NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.init_info = cp;
  return ecs_add(NAMESPACE_ELEMENT_INTERNAL.ECS, eid, NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.component_signature, NULL);
}

