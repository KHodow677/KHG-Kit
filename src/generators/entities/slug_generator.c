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
  generate_physics_circle(s->entity, &s->physics_info, true, 60, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f), COLLISION_CATEGORY_ENTITY);
  phy_body_set_angle(s->physics_info.body, angle);
  generate_renderer(&s->renderer_info, &s->physics_info, SLUG_1_0, 9, s->entity);
  generate_destroyer(&s->destroyer_info);
  generate_animator(&s->animator_info, SLUG_1_0, SLUG_1_28, 0.032f, false);
  generate_health(&s->health_info, &s->physics_info, 500.0f, 500.0f);
  generate_mover(&s->mover_info, s->entity, 150.0f, 8.0f, init_path, init_path_length);
  sys_physics_add(&s->entity, &s->physics_info);
  sys_renderer_add(&s->entity, &s->renderer_info);
  sys_destroyer_add(&s->entity, &s->destroyer_info);
  sys_animator_add(&s->entity, &s->animator_info);
  sys_mover_add(&s->entity, &s->mover_info);
  sys_health_add(&s->entity, &s->health_info);
}

void free_slug(slug *s) {
  s->destroyer_info.destroy_now = true;
  free_physics(&s->physics_info, false);
  free_renderer(&s->renderer_info);
  free_mover(&s->mover_info);
  PHYSICS_INFO[s->entity] = NO_PHYSICS;
  utl_vector_assign(RENDERER_INFO, s->entity, &NO_RENDERER);
  DESTROYER_INFO[s->entity] = NO_DESTROYER;
  ANIMATOR_INFO[s->entity] = NO_ANIMATOR;
  MOVER_INFO[s->entity] = NO_MOVER;
}

