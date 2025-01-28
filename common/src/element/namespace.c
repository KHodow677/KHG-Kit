#define NAMESPACE_ELEMENT_IMPL

#include "element/namespace.h"
#include "element/comp_light.h"
#include "element/comp_physics.h"
#include "element/comp_render.h"
#include "element/comp_tile.h"
#include "element/ecs_manager.h"

element_namespace NAMESPACE_ELEMENT_INTERNAL = {
  .ecs_setup = ecs_setup,
  .ecs_cleanup = ecs_cleanup,
  .comp_light_register = comp_light_register,
  .sys_light_register = sys_light_register,
  .sys_light_add = sys_light_add,
  .comp_physics_register = comp_physics_register,
  .sys_physics_register = sys_physics_register,
  .sys_physics_add = sys_physics_add,
  .comp_render_register = comp_render_register,
  .sys_render_register = sys_render_register,
  .sys_render_add = sys_render_add,
  .comp_tile_register = comp_tile_register,
  .sys_tile_register = sys_tile_register,
  .sys_tile_add = sys_tile_add,
  .ENTITY_COUNT = 1024,
  .ECS = NULL,
  .LIGHT_INFO = { 0 },
  .PHYSICS_INFO = { 0 },
  .RENDER_INFO = { 0 },
  .TILE_INFO = { 0 }
};

element_namespace *NAMESPACE_ELEMENT(void) {
  return &NAMESPACE_ELEMENT_INTERNAL;
}

