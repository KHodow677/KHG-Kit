#define NAMESPACE_ELEMENT_USE

#include "element/namespace.h"
#include "khg_ecs/ecs.h"
#include "khg_kin/namespace.h"
#include "scene/builders/light_builder.h"

void build_light(const float x, const float y, const float light_radius) {
  const ecs_id entity = ecs_create(NAMESPACE_ELEMENT()->ECS);
  comp_physics comp_physics_ci = { .init_pos = (kin_vec){ x, y }, .init_ang = 0.0f };
  comp_physics *cp = NAMESPACE_ELEMENT()->sys_physics_add(entity, &comp_physics_ci);
  comp_light comp_light_ci = { .light = (light){ (kin_vec){ 0.0, 0.0 }, light_radius }, .offset = (kin_vec){ 0.0f, 0.0f } };
  comp_light *cl = NAMESPACE_ELEMENT()->sys_light_add(entity, &comp_light_ci);
}

