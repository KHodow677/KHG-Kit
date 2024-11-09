#include "scene/scenes/main/light_builder.h"
#include "ecs/comp_light.h"
#include "ecs/comp_physics.h"
#include "ecs/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/vector.h"

void build_light(const float x, const float y, const float light_radius) {
  const ecs_id entity = ecs_create(ECS);
  comp_physics_constructor_info comp_physics_ci = { PHYSICS_BOX, 0.0f, 0.0f, 1.0f, NV_VECTOR2(x, y), 0.0f, false, false };
  const comp_physics *cp = sys_physics_add(entity, &comp_physics_ci);
  comp_light_constructor_info comp_light_ci = { cp->body, (light){ (vec2s){ 0.0, 0.0 }, light_radius }, NV_VECTOR2(0.0f, 0.0f) };
  const comp_light *cl = sys_light_add(entity, &comp_light_ci);
}

