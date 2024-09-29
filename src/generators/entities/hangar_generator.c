#include "generators/entities/hangar_generator.h"
#include "entity/comp_renderer.h"
#include "entity/comp_selector.h"
#include "entity/comp_spawn.h"
#include "game_manager.h"
#include "generators/components/comp_info_generator.h"
#include "generators/components/texture_generator.h"
#include "khg_phy/body.h"
#include "khg_phy/vect.h"

void generate_hangar(hangar *h, float x, float y, float angle) {
  h->entity = ecs_create(ECS);
  h->comp_physics = sys_physics_add(h->entity);
  h->comp_renderer = sys_renderer_add(h->entity);
  h->comp_destroyer = sys_destroyer_add(h->entity);
  h->comp_selector = sys_selector_add(h->entity);
  h->comp_spawn = sys_spawn_add(h->entity);
  generate_static_physics_box(h->entity, h->comp_physics, false, 306, 334, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f), COLLISION_CATEGORY_OBJECT);
  phy_body_set_angle(h->comp_physics->body, angle);
  generate_renderer(h->comp_renderer, h->comp_renderer, h->comp_physics, HANGAR, 4);
  generate_destroyer(h->comp_destroyer);
  generate_selector(h->comp_selector, HANGAR, HANGAR, HANGAR_OUTLINE, HANGAR_OUTLINE);
  generate_spawn(h->comp_spawn, phy_v(x, y), phy_v(-x, -y));
}

void free_hangar(hangar *h) {
  free_physics(h->comp_physics, false);
  free_renderer(h->comp_renderer);
}

