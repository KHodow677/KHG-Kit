#define NAMESPACE_ELEMENT_USE
#define NAMESPACE_TASKING_USE

#include "element/namespace.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/core/phy_vector.h"
#include "scene/builders/tile_builder.h"
#include "tasking/namespace.h"
#include "util/ovr_tile.h"

void build_ovr_tile(const char *tile_id, const phy_vector2 pos) {
  const unsigned int id = NAMESPACE_TASKING()->get_tile_id(tile_id);
  const ovr_tile tile = NAMESPACE_TASKING()->get_tile_data(id);
  const ecs_id entity = ecs_create(NAMESPACE_ELEMENT()->ECS);
  comp_physics comp_physics_ci = { .init_pos = ovr_tile_pos_to_world_pos(pos), .init_size = phy_vector2_new(0.0f, 0.0f), .init_ang = 0.0f };
  comp_physics *cp = NAMESPACE_ELEMENT()->sys_physics_add(entity, &comp_physics_ci);
  comp_render comp_renderer_ci = { .tex_id_loc = NAMESPACE_TASKING()->get_texture_id("EMPTY_TEXTURE"), .ovr_tile = (ovr_tile_info){ NAMESPACE_TASKING()->get_tile_id(tile_id), pos }, .render_layer = 0, .parallax_value = 1.0f, .flipped = false };
  comp_render *cr = NAMESPACE_ELEMENT()->sys_render_add(entity, &comp_renderer_ci);
  comp_tile comp_tile_ci = { .loaded = tile.tile_id, .tile = (ovr_tile_info){ NAMESPACE_TASKING()->get_tile_id(tile_id), pos } };
  comp_tile *ct = NAMESPACE_ELEMENT()->sys_tile_add(entity, &comp_tile_ci);
}

void build_random_tile(const phy_vector2 pos) {
  build_ovr_tile(NAMESPACE_TASKING()->get_random_tile_name(), pos);
}
