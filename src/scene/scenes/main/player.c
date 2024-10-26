#include "scene/scenes/main/player.h"
#include "ecs/comp_animator.h"
#include "ecs/comp_light.h"
#include "ecs/comp_physics.h"
#include "ecs/comp_renderer.h"
#include "ecs/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/vect.h"
#include <stdbool.h>

void generate_player(int min_tex_id, int max_tex_id, float x, float y, int render_layer) {
  ecs_id entity = ecs_create(ECS);
  comp_physics_constructor_info comp_physics_ci = { PHYSICS_BOX, 1920.0f, 906.0f, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f) };
  comp_physics *cp = sys_physics_add(entity, &comp_physics_ci);
  comp_renderer_constructor_info comp_renderer_ci = { cp->body, min_tex_id, render_layer };
  comp_renderer *cr = sys_renderer_add(entity, &comp_renderer_ci);
  comp_animator_constructor_info comp_animator_ci = { min_tex_id, max_tex_id, 0.032f, false };
  comp_animator *ca = sys_animator_add(entity, &comp_animator_ci);
  comp_light_constructor_info comp_light_ci = { cp->body, (light){ (vec2s){ 0.0, 0.0 }, 250.0f }, phy_v(0.0f, 0.0f)};
  comp_light *cl = sys_light_add(entity, &comp_light_ci);
}

