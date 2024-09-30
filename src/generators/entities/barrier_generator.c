#include "generators/entities/barrier_generator.h"
#include "game_manager.h"
#include "generators/components/comp_info_generator.h"
#include "generators/components/texture_generator.h"
#include "khg_phy/body.h"
#include "khg_phy/vect.h"

void generate_barrier(barrier *b, float x, float y, float angle) {
  b->entity = ecs_create(ECS);
  b->comp_physics = sys_physics_add(b->entity);
  b->comp_renderer = sys_renderer_add(b->entity);
  b->comp_destroyer = sys_destroyer_add(b->entity);
  generate_static_physics_box(b->entity, b->comp_physics, false, 192, 93, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f), COLLISION_CATEGORY_OBJECT);
  phy_body_set_angle(b->comp_physics->body, angle);
  generate_static_renderer_segments(b->comp_renderer, b->comp_renderer, b->comp_physics, phy_v(x, y), BARRIER, 2, angle);
  /*generate_renderer(b->comp_renderer, b->comp_renderer, b->comp_physics, BARRIER, 2);*/
  generate_destroyer(b->comp_destroyer);
}

void free_barrier(barrier *b) {
  free_physics(b->comp_physics, false);
  free_renderer(b->comp_renderer);
}

