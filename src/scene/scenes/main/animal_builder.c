#include "scene/scenes/main/animal_builder.h"
#include "ecs/comp_animator.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/core/phy_vector.h"
#include <stdbool.h>

void build_animal(const int min_tex_id, const int max_tex_id, const float x, const float y, const int render_layer) {
  const ecs_id entity = ecs_create(ECS);
  comp_physics_constructor_info comp_physics_ci = { PHYSICS_BOX, 0.0f, 0.0f, 1.0f, phy_vector2_new(x, y), 0.0f, false, false };
  const comp_physics *cp = sys_physics_add(entity, &comp_physics_ci);
  comp_renderer_constructor_info comp_renderer_ci = { cp->body, min_tex_id, render_layer, 1.0f, false };
  const comp_renderer *cr = sys_renderer_add(entity, &comp_renderer_ci, NULL);
  comp_animator_constructor_info comp_animator_ci = { min_tex_id, max_tex_id, 0.064f, false };
  const comp_animator *ca = sys_animator_add(entity, &comp_animator_ci);
}
