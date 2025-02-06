#define NAMESPACE_ELEMENT_IMPL
#define NAMESPACE_TASKING_USE

#include "element/namespace.h"
#include "game.h"
#include "khg_gfx/elements.h"
#include "khg_phy/core/phy_vector.h"
#include "tasking/namespace.h"
#include "util/io/cursor_controller.h"
#include "util/ovr_tile.h"

static bool is_within_tile(const phy_vector2 tile_center, const phy_vector2 test_position, const float threshold, float tile_size) {
  tile_size *= (gfx_get_display_width() / SCREEN_WIDTH);
  tile_size *= (gfx_get_display_height() / SCREEN_HEIGHT);
  const float dist = phy_vector2_dist(test_position, tile_center);
  const float edge_radius = tile_size / 2.0f * threshold;
  return dist < edge_radius;
}

static ecs_ret sys_tile_update(ecs_ecs *ecs, ecs_id *entities, const unsigned int entity_count, const ecs_dt dt, void *udata) {
  for (unsigned int id = 0; id < entity_count; id++) {
    comp_physics *p_info = ecs_get(NAMESPACE_ELEMENT_INTERNAL.ECS, entities[id], NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.component_signature);
    comp_tile *info = ecs_get(NAMESPACE_ELEMENT_INTERNAL.ECS, entities[id], NAMESPACE_ELEMENT_INTERNAL.TILE_INFO.component_signature);
    if (!info->loaded && NAMESPACE_TASKING()->get_tile_data(info->tile.tile_id).tile_id) {
      add_ovr_tile_elements(&info->tile);
      info->loaded = true;
    }
    bool is_hovered = is_within_tile(phy_rigid_body_get_position(p_info->body), CURSOR_STATE.world_pos, 0.9f, get_ovr_tile_size());
  }
  return 0;
}

static void comp_tile_constructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr, void *args) {
  comp_tile *info = ptr;
  const comp_tile *constructor_info = NAMESPACE_ELEMENT_INTERNAL.TILE_INFO.init_info;
  if (info && constructor_info) {
    info->tile = constructor_info->tile;
    if (info->loaded) {
      add_ovr_tile_elements(&info->tile);
    }
  }
}

static void comp_tile_destructor(ecs_ecs *ecs, const ecs_id entity_id, void *ptr) {
  comp_tile *info = ptr;
  if (info) {
    remove_ovr_tile_elements(&info->tile);
  }
}

void comp_tile_register() {
  NAMESPACE_ELEMENT_INTERNAL.TILE_INFO.component_signature = ecs_register_component(NAMESPACE_ELEMENT_INTERNAL.ECS, sizeof(comp_tile), comp_tile_constructor, comp_tile_destructor);
}

void sys_tile_register() {
  NAMESPACE_ELEMENT_INTERNAL.TILE_INFO.system_signature = ecs_register_system(NAMESPACE_ELEMENT_INTERNAL.ECS, sys_tile_update, NULL, NULL, NULL);
  ecs_require_component(NAMESPACE_ELEMENT_INTERNAL.ECS, NAMESPACE_ELEMENT_INTERNAL.TILE_INFO.system_signature, NAMESPACE_ELEMENT_INTERNAL.TILE_INFO.component_signature);
  ecs_require_component(NAMESPACE_ELEMENT_INTERNAL.ECS, NAMESPACE_ELEMENT_INTERNAL.TILE_INFO.system_signature, NAMESPACE_ELEMENT_INTERNAL.PHYSICS_INFO.component_signature);
  ecs_require_component(NAMESPACE_ELEMENT_INTERNAL.ECS, NAMESPACE_ELEMENT_INTERNAL.TILE_INFO.system_signature, NAMESPACE_ELEMENT_INTERNAL.RENDER_INFO.component_signature);
}

comp_tile *sys_tile_add(const ecs_id eid, comp_tile *ct) {
  NAMESPACE_ELEMENT_INTERNAL.TILE_INFO.init_info = ct;
  return ecs_add(NAMESPACE_ELEMENT_INTERNAL.ECS, eid, NAMESPACE_ELEMENT_INTERNAL.TILE_INFO.component_signature, NULL);
}

