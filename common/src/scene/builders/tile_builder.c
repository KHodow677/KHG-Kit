#include "area/ovr_tile.h"
#include "ecs/comp_light.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/comp_tile.h"
#include "ecs/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/core/phy_vector.h"
#include "resources/rig_loader.h"
#include "resources/texture_loader.h"
#include "scene/builders/tile_builder.h"

void build_ovr_tile(const unsigned int tile_id, const phy_vector2 pos) {
  const ecs_id entity = ecs_create(ECS);
  comp_physics_constructor_info comp_physics_ci = { PHYSICS_BOX, 0.0f, 0.0f, 1.0f, ovr_tile_pos_to_world_pos(pos), 0.0f, false, false, false };
  comp_physics *cp = sys_physics_add(entity, &comp_physics_ci);
  comp_renderer_constructor_info comp_renderer_ci = { cp->body, cp->shape, NULL_TEXTURE, NULL_RIG, (ovr_tile_info){ tile_id, pos }, 0, 1.0f, false, false };
  comp_renderer *cr = sys_renderer_add(entity, &comp_renderer_ci);
  comp_tile_constructor_info comp_tile_ci = { (ovr_tile_info){ tile_id, pos } };
  comp_tile *ct = sys_tile_add(entity, &comp_tile_ci);
  comp_light_constructor_info comp_light_ci = { cp->body, (light){ (vec2s){ 0.0, 0.0 }, 50.0f }, phy_vector2_new(0.0f, 0.0f) };
  comp_light *cl = sys_light_add(entity, &comp_light_ci);
}

