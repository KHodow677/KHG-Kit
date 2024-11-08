#include "scene/scenes/main/light_builder.h"
#include "ecs/comp_light.h"
#include "ecs/comp_physics.h"
#include "ecs/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/vect.h"

void build_light(const float x, const float y, const float light_radius) {
  const ecs_id entity = ecs_create(ECS);
  comp_physics_constructor_info comp_physics_ci = { PHYSICS_BOX, 1920.0f, 906.0f, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f) };
  const comp_physics *cp = sys_physics_add(entity, &comp_physics_ci);
  comp_light_constructor_info comp_light_ci = { cp->body, (light){ (vec2s){ 0.0, 0.0 }, light_radius }, phy_v(0.0f, 0.0f) };
  const comp_light *cl = sys_light_add(entity, &comp_light_ci);
}

