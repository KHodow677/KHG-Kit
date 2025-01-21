#include "ecs/comp_tile.h"
#include "area/ovr_tile.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/ecs_manager.h"

ecs_id TILE_COMPONENT_SIGNATURE;
ecs_id TILE_SYSTEM_SIGNATURE;

comp_tile_constructor_info *TILE_CONSTRUCTOR_INFO = NULL;

static ecs_ret sys_tile_update(ecs_ecs *ecs, ecs_id *entities, const unsigned int entity_count, const ecs_dt dt, void *udata) {
  for (unsigned int id = 0; id < entity_count; id++) {
  }
  return 0;
}

static void comp_tile_constructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr, void *args) {
  comp_tile *info = ptr;
  const comp_tile_constructor_info *constructor_info = TILE_CONSTRUCTOR_INFO;
  if (info && constructor_info) {
    info->tile = constructor_info->tile;
    add_ovr_tile_elements(&info->tile);
  }
}

static void comp_tile_destructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr) {
  comp_tile *info = ptr;
  if (info) {
    remove_ovr_tile_elements(&info->tile);
  }
}

void comp_tile_register() {
  TILE_COMPONENT_SIGNATURE = ecs_register_component(ECS, sizeof(comp_tile), comp_tile_constructor, comp_tile_destructor);
}

void sys_tile_register() {
  TILE_SYSTEM_SIGNATURE = ecs_register_system(ECS, sys_tile_update, NULL, NULL, NULL);
  ecs_require_component(ECS, TILE_SYSTEM_SIGNATURE, TILE_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, TILE_SYSTEM_SIGNATURE, PHYSICS_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, TILE_SYSTEM_SIGNATURE, RENDERER_COMPONENT_SIGNATURE);
}

comp_tile *sys_tile_add(const ecs_id eid, comp_tile_constructor_info *clci) {
  TILE_CONSTRUCTOR_INFO = clci;
  return ecs_add(ECS, eid, TILE_COMPONENT_SIGNATURE, NULL);
}

