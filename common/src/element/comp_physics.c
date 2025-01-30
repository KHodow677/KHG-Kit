#define NAMESPACE_ELEMENT_IMPL

#include "element/namespace.h"
#include "khg_phy/core/phy_vector.h"
#include "khg_phy/body.h"
#include "khg_phy/space.h"
#include "khg_phy/core/phy_vector.h"
#include "util/physics.h"
#include <stdio.h>

static ecs_ret sys_physics_update(ecs_ecs *ecs, ecs_id *entities, const unsigned int entity_count, const ecs_dt dt, void *udata) {
  if (dt == 0.0f) {
    return 0;
  }
  for (unsigned int id = 0; id < entity_count; id++) {
    element_comp_physics *info = ecs_get(NAMESPACE_ELEMENT_INTERNAL.ECS, entities[id], NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.component_signature);
    phy_vector2 current_velocity = phy_rigid_body_get_linear_velocity(info->body);
    phy_rigid_body_set_linear_velocity(info->body, info->vel);
    phy_rigid_body_set_angle(info->body, 0.0f);
  }
  return 0;
}

static void comp_physics_constructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr, void *args) {
  element_comp_physics *info = ptr;
  const element_comp_physics *constructor_info = NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.init_info;
  if (info && constructor_info) {
    phy_rigid_body_initializer body_init = phy_rigid_body_initializer_default;
    body_init.type = PHY_RIGID_BODY_TYPE_DYNAMIC;
    body_init.position = phy_vector2_new(0.0, 0.0);
    info->body = phy_rigid_body_new(body_init);
    info->vel = phy_vector2_new(0.0f, 0.0f);
    info->is_moving = false;
    phy_rigid_body_set_mass(info->body, 1.0f);
    phy_rigid_body_set_gravity_scale(info->body, 0.0f);
    phy_rigid_body_set_position(info->body, constructor_info->init_pos);
    phy_rigid_body_set_angle(info->body, constructor_info->init_ang);
    info->shape = phy_rect_shape_new(constructor_info->init_size.x, constructor_info->init_size.y, phy_vector2_zero);
    phy_rigid_body_add_shape(info->body, info->shape);
    phy_rigid_body_disable_collisions(info->body);
    phy_space_add_rigidbody(SPACE, info->body);
  }
}

static void comp_physics_destructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr) {
  element_comp_physics *info = ptr;
  if (info) {
    phy_space_remove_rigidbody(SPACE, info->body);
    phy_rigid_body_remove_shape(info->body, info->shape);
    phy_rigid_body_free(info->body);
    phy_shape_free(info->shape);
  }
}

void comp_physics_register() {
  NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.component_signature = ecs_register_component(NAMESPACE_ELEMENT_INTERNAL.ECS, sizeof(element_comp_physics), comp_physics_constructor, comp_physics_destructor);
}

void sys_physics_register() {
  NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.system_signature = ecs_register_system(NAMESPACE_ELEMENT_INTERNAL.ECS, sys_physics_update, NULL, NULL, NULL);
  ecs_require_component(NAMESPACE_ELEMENT_INTERNAL.ECS, NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.system_signature, NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.component_signature);
}

element_comp_physics *sys_physics_add(const ecs_id eid, element_comp_physics *cp) {
  NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.init_info = cp;
  return ecs_add(NAMESPACE_ELEMENT_INTERNAL.ECS, eid, NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.component_signature, NULL);
}

