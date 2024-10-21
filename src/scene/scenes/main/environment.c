#include "scene/scenes/main/environment.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/vect.h"
#include <stdbool.h>

void generate_environment_element(int tex_id, float x, float y, float angle, bool is_animated, int render_layer) {
  ecs_id entity = ecs_create(ECS);
  comp_physics_constructor_info comp_physics_ci = { PHYSICS_BOX, 1920.0f, 906.0f, 1.0f, phy_v(x, y), angle, phy_v(0.0f, 0.0f) };
  comp_physics *cp = sys_physics_add(entity, &comp_physics_ci);
  comp_renderer_constructor_info comp_renderer_ci = { cp->body, tex_id, render_layer };
  comp_renderer *cr = sys_renderer_add(entity, &comp_renderer_ci);
}

