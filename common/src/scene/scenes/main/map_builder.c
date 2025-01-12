#include "area/ovr_map.h"
#include "ecs/comp_light.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/core/phy_vector.h"
#include "resources/ovr_map_loader.h"
#include "resources/rig_loader.h"
#include "resources/texture_loader.h"
#include "scene/scenes/main/map_builder.h"

void build_ovr_map(const phy_vector2 pos, const float scale) {
  const ecs_id entity = ecs_create(ECS);
  comp_physics_constructor_info comp_physics_ci = { PHYSICS_BOX, 0.0f, 0.0f, 1.0f, pos, 0.0f, false, false, false };
  comp_physics *cp = sys_physics_add(entity, &comp_physics_ci);
  comp_renderer_constructor_info comp_renderer_ci = { cp->body, cp->shape, NULL_TEXTURE, NULL_RIG, PLAYER_OVR_MAP, 0, 1.0f, false, false };
  comp_renderer *cr = sys_renderer_add(entity, &comp_renderer_ci);
  set_ovr_map_tile_scale(&cr->ovr_map, scale);
  comp_light_constructor_info comp_light_ci = { cp->body, (light){ (vec2s){ 0.0, 0.0 }, 150.0f }, phy_vector2_new(0.0f, 0.0f) };
  comp_light *cl = sys_light_add(entity, &comp_light_ci);
}

