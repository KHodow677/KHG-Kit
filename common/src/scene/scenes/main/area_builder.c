#include "ecs/comp_collider_group.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/core/phy_vector.h"
#include "resources/rig_loader.h"
#include "resources/texture_loader.h"
#include "scene/scenes/main/area_builder.h"

void build_area(unsigned int area_id, unsigned int render_layer, bool is_background) {
  ecs_id entity = ecs_create(ECS);
  comp_physics_constructor_info comp_physics_ci = { PHYSICS_BOX, 300.0f, 256.0f, 1.0f, phy_vector2_new(0, 0), 0.0f, false, false, false };
  comp_physics *cp = sys_physics_add(entity, &comp_physics_ci);
  comp_renderer_constructor_info comp_renderer_ci = { cp->body, cp->shape, PLAYER_BODY, NO_RIG_ID, area_id, render_layer, 1.0f, false, false };
  comp_renderer *cr = sys_renderer_add(entity, &comp_renderer_ci);
  if (!is_background) {
    comp_collider_group_constructor_info comp_collider_group_ci = { true, area_id };
    comp_collider_group *ccg = sys_collider_group_add(entity, &comp_collider_group_ci);
  }
}

