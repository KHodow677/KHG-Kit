#include "generators/entities/spawner_generator.h"
#include "entity/comp_stream_spawner.h"
#include "game_manager.h"
#include "generators/components/comp_info_generator.h"
#include "generators/components/texture_generator.h"
#include "khg_phy/body.h"
#include "khg_phy/vect.h"
#include "khg_utl/vector.h"

void generate_spawner(spawner *s, float x, float y, float angle, phy_vect *path, int path_length) {
  s->entity = ecs_create(ECS);
  generate_static_physics_box(s->entity, &s->physics_info, false, 191, 214, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f), COLLISION_CATEGORY_OBJECT);
  phy_body_set_angle(s->physics_info.body, angle);
  generate_static_renderer_segments(&s->renderer_info, &s->physics_info, phy_v(x, y), SPAWNER, 1, s->entity, angle);
  generate_destroyer(&s->destroyer_info);
  generate_stream_spawner(&s->stream_spawner_info, 5, phy_v(0.0f, 0.0f));
  for (int i = 0; i < path_length; i++) {
    utl_vector_push_back(s->stream_spawner_info.path, &path[i]);
  }
  sys_physics_add(&s->entity, &s->physics_info);
  sys_renderer_add(&s->entity, &s->renderer_info);
  sys_destroyer_add(&s->entity, &s->destroyer_info);
  sys_stream_spawner_add(&s->entity, &s->stream_spawner_info);
}

void free_spawner(spawner *s) {
  free_physics(&s->physics_info, false);
  free_renderer(&s->renderer_info);
  free_stream_spawner(&s->stream_spawner_info);
  PHYSICS_INFO[s->entity] = NO_PHYSICS;
  utl_vector_assign(RENDERER_INFO, s->entity, &NO_RENDERER);
  DESTROYER_INFO[s->entity] = NO_DESTROYER;
  utl_vector_assign(STREAM_SPAWNER_INFO, s->entity, &NO_STREAM_SPAWNER);
}

