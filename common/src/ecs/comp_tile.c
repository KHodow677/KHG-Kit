#include "ecs/comp_tile.h"
#include "area/ovr_tile.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/ecs_manager.h"
#include "game.h"
#include "io/cursor_controller.h"
#include "khg_gfx/elements.h"
#include "khg_phy/core/phy_vector.h"
#include <stdio.h>

ecs_id TILE_COMPONENT_SIGNATURE;
ecs_id TILE_SYSTEM_SIGNATURE;

comp_tile_constructor_info *TILE_CONSTRUCTOR_INFO = NULL;

static bool is_within_tile(const phy_vector2 tile_center, const phy_vector2 test_position, const float threshold, float tile_size) {
  tile_size *= (gfx_get_display_width() / SCREEN_WIDTH);
  tile_size *= (gfx_get_display_height() / SCREEN_HEIGHT);
  const float dist = phy_vector2_dist(test_position, tile_center);
  const float edge_radius = tile_size / 2.0f * threshold;
  return dist < edge_radius;
}

static ecs_ret sys_tile_update(ecs_ecs *ecs, ecs_id *entities, const unsigned int entity_count, const ecs_dt dt, void *udata) {
  for (unsigned int id = 0; id < entity_count; id++) {
    comp_physics *p_info = ecs_get(ECS, entities[id], PHYSICS_COMPONENT_SIGNATURE);
    bool is_hovered = is_within_tile(phy_rigid_body_get_position(p_info->body), CURSOR_STATE.world_pos, 0.9f, get_ovr_tile_size());
    // printf("Hovered: %i\n", is_hovered);
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

