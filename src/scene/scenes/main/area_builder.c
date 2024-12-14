#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/core/phy_vector.h"
#include "resources/area_loader.h"
#include "resources/rig_loader.h"
#include "resources/texture_loader.h"
#include "scene/scenes/main/area_builder.h"

void build_area(size_t area_id, int render_layer) {
  ecs_id entity = ecs_create(ECS);
  comp_physics_constructor_info comp_physics_ci = { PHYSICS_BOX, 300.0f, 256.0f, 1.0f, phy_vector2_new(0, 0), 0.0f, true, true };
  comp_physics *cp = sys_physics_add(entity, &comp_physics_ci);
  comp_renderer_constructor_info comp_renderer_ci = { cp->body, PLAYER_BODY, NO_RIG_ID, DUNGEON_0, render_layer, 1.0f, false };
  comp_renderer *cr = sys_renderer_add(entity, &comp_renderer_ci);
}

