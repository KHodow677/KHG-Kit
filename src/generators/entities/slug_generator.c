#include "generators/entities/slug_generator.h"
#include "entity/comp_health.h"
#include "generators/components/comp_info_generator.h"
#include "game_manager.h"
#include "generators/components/texture_generator.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"

void generate_slug(slug *s, float x, float y, float angle, phy_vect *init_path, int init_path_length) {
  s->entity = ecs_create(ECS);
  s->comp_physics = sys_physics_add(s->entity);
  s->comp_renderer = sys_renderer_add(s->entity);
  s->comp_destroyer = sys_destroyer_add(s->entity);
  s->comp_animator = sys_animator_add(s->entity);
  s->comp_mover = sys_mover_add(s->entity);
  s->comp_health = sys_health_add(s->entity);
  generate_physics_circle(s->entity, s->comp_physics, true, 60, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f), COLLISION_CATEGORY_ENTITY);
  phy_body_set_angle(s->comp_physics->body, angle);
  generate_renderer(s->comp_renderer, s->comp_renderer, s->comp_physics, SLUG_1_0, 2);
  generate_destroyer(s->comp_destroyer);
  generate_animator(s->comp_animator, SLUG_1_0, SLUG_1_28, 0.032f, false);
  generate_health(s->comp_health, s->comp_physics, 500.0f, 500.0f);
  generate_mover(s->comp_mover, s->comp_physics, 150.0f, 8.0f, init_path, init_path_length);
  s->comp_physics->health_ref = s->comp_health;
}

void free_slug(slug *s) {
  s->comp_destroyer->destroy_now = true;
  free_physics(s->comp_physics, false);
  free_renderer(s->comp_renderer);
  free_mover(s->comp_mover);
}

