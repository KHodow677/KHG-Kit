#include "generators/entities/slug_generator.h"
#include "generators/components/comp_info_generator.h"
#include "game_manager.h"
#include "generators/components/texture_generator.h"
#include "khg_phy/vect.h"

void generate_slug(slug *s, float x, float y, float angle) {
  s->entity = ecs_create(ECS);
  generate_physics_box(&s->physics_info, false, 80, 80, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f));
  generate_renderer(&s->renderer_info, &s->physics_info, SLUG_1_0, 9, s->entity);
  generate_destroyer(&s->destroyer_info);
  generate_animator(&s->animator_info, SLUG_1_0, SLUG_1_28, 0.032f, false);
  sys_physics_add(&s->entity, &s->physics_info);
  sys_renderer_add(&s->entity, &s->renderer_info);
  sys_destroyer_add(&s->entity, &s->destroyer_info);
  sys_animator_add(&s->entity, &s->animator_info);
}

void free_slug(slug *s) {
  s->destroyer_info.destroy_now = true;
  free_physics(&s->physics_info, false);
  free_renderer(&s->renderer_info);
  utl_vector_assign(PHYSICS_INFO, s->entity, &NO_PHYSICS);
  utl_vector_assign(RENDERER_INFO, s->entity, &NO_RENDERER);
  utl_vector_assign(DESTROYER_INFO, s->entity, &NO_DESTROYER);
  utl_vector_assign(ANIMATOR_INFO, s->entity, &NO_ANIMATOR);
}


