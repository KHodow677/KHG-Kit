#include "generators/entities/depot_generator.h"
#include "entity/comp_renderer.h"
#include "game_manager.h"
#include "generators/components/comp_info_generator.h"
#include "generators/components/texture_generator.h"
#include "khg_phy/body.h"
#include "khg_phy/vect.h"

void generate_depot(depot *d, float x, float y, float angle) {
  d->entity = ecs_create(ECS);
  d->comp_physics = sys_physics_add(d->entity);
  d->comp_renderer = sys_renderer_add(d->entity);
  d->comp_destroyer = sys_destroyer_add(d->entity);
  generate_static_physics_box(d->entity, d->comp_physics, false, 306, 334, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f), COLLISION_CATEGORY_ENTITY);
  phy_body_set_angle(d->comp_physics->body, angle);
  generate_renderer(d->comp_renderer, d->comp_renderer, d->comp_physics, DEPOT, 4);
  generate_destroyer(d->comp_destroyer);
}

void free_depot(depot *d) {
  free_physics(d->comp_physics, false);
  free_renderer(d->comp_renderer);
}

