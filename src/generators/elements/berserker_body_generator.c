#include "generators/elements/berserker_body_generator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "game_manager.h"
#include "generators/components/comp_info_generator.h"
#include "generators/components/texture_generator.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/vect.h"

void generate_berserker_body(berserker_body *bb, float x, float y, float angle) {
  bb->entity = ecs_create(ECS);
  bb->comp_physics = sys_physics_add(bb->entity);
  bb->comp_renderer = sys_renderer_add(bb->entity);
  bb->comp_destroyer = sys_destroyer_add(bb->entity);
  generate_physics_box(bb->entity, bb->comp_physics, true, 145.0f, 184.0f, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f), COLLISION_CATEGORY_ENTITY);
  phy_body_set_angle(bb->comp_physics->body, angle);
  generate_renderer(bb->comp_renderer, bb->comp_renderer, bb->comp_physics, TANK_BODY, 2);
  generate_destroyer(bb->comp_destroyer);
}

void free_berserker_body(berserker_body *bb) {
  free_physics(bb->comp_physics, false);
  free_renderer(bb->comp_renderer);
}

