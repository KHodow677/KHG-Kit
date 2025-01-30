#define NAMESPACE_ELEMENT_IMPL

#include "element/namespace.h"

void ecs_setup() {
  NAMESPACE_ELEMENT_INTERNAL.ECS = ecs_new(NAMESPACE_ELEMENT_INTERNAL.ENTITY_COUNT, NULL);
  NAMESPACE_ELEMENT_INTERNAL.comp_light_register();
  NAMESPACE_ELEMENT_INTERNAL.comp_physics_register();
  NAMESPACE_ELEMENT_INTERNAL.comp_render_register();
  NAMESPACE_ELEMENT_INTERNAL.comp_tile_register();
  NAMESPACE_ELEMENT_INTERNAL.sys_light_register();
  NAMESPACE_ELEMENT_INTERNAL.sys_physics_register();
  NAMESPACE_ELEMENT_INTERNAL.sys_render_register();
  NAMESPACE_ELEMENT_INTERNAL.sys_tile_register();
}

void ecs_cleanup() {
  ecs_free(NAMESPACE_ELEMENT_INTERNAL.ECS);
}

