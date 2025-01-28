#define NAMESPACE_ELEMENT_USE

#include "element/namespace.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/core/phy_vector.h"
#include "khg_utl/random.h"
#include "scene/builders/tile_builder.h"
#include "loading/namespace.h"
#include "util/ovr_tile.h"

void build_ovr_tile(const unsigned int tile_id, const phy_vector2 pos) {
  const ecs_id entity = ecs_create(NAMESPACE_ELEMENT()->ECS);
  element_comp_physics comp_physics_ci = { .init_pos = ovr_tile_pos_to_world_pos(pos), .init_size = phy_vector2_new(0.0f, 0.0f), .init_ang = 0.0f };
  element_comp_physics *cp = NAMESPACE_ELEMENT()->sys_physics_add(entity, &comp_physics_ci);
  element_comp_render comp_renderer_ci = { .tex_id = EMPTY_TEXTURE, .ovr_tile = (ovr_tile_info){ tile_id, pos }, .render_layer = 0, .parallax_value = 1.0f, .flipped = false };
  element_comp_render *cr = NAMESPACE_ELEMENT()->sys_render_add(entity, &comp_renderer_ci);
  comp_tile comp_tile_ci = { .tile = (ovr_tile_info){ tile_id, pos } };
  comp_tile *ct = NAMESPACE_ELEMENT()->sys_tile_add(entity, &comp_tile_ci);
  element_comp_light comp_light = { .light = (light){ phy_vector2_new(0.0, 0.0), 50.0f }, .offset = phy_vector2_new(0.0f, 0.0f) };
  element_comp_light *cl = NAMESPACE_ELEMENT()->sys_light_add(entity, &comp_light);
}

void build_random_tile(const phy_vector2 pos) {
  unsigned int option = utl_random_randint(PLAINS_CLEARING_0, PLAINS_DENSE_9);
  build_ovr_tile(option, pos);
}

