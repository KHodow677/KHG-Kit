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
  s->comp_physics = sys_physics_add(s->entity);
  s->comp_renderer = sys_renderer_add(s->entity);
  s->comp_destroyer = sys_destroyer_add(s->entity);
  s->comp_stream_spawner = sys_stream_spawner_add(s->entity);
  generate_static_physics_box(s->entity, s->comp_physics, false, 191, 214, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f), COLLISION_CATEGORY_OBJECT);
  phy_body_set_angle(s->comp_physics->body, angle);
  generate_static_renderer_segments(s->comp_renderer, s->comp_renderer, s->comp_physics, phy_v(x, y), SPAWNER, 1, angle);
  generate_destroyer(s->comp_destroyer);
  generate_stream_spawner(s->comp_stream_spawner, 5.0f, 5.0f, phy_v(0.0f, 0.0f), true);
  for (int i = 0; i < path_length; i++) {
    utl_vector_push_back(s->comp_stream_spawner->path, &path[i]);
  }
}

void free_spawner(spawner *s) {
  free_physics(s->comp_physics, false);
  free_renderer(s->comp_renderer);
  free_stream_spawner(s->comp_stream_spawner);
}

