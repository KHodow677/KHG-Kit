#include "generators/elements/berserker_clone_body_generator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "game_manager.h"
#include "generators/components/comp_info_generator.h"
#include "generators/components/texture_generator.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/vect.h"

void generate_berserker_clone_body(berserker_clone_body *bcb, float x, float y, float angle) {
  bcb->entity = ecs_create(ECS);
  bcb->comp_physics = sys_physics_add(bcb->entity);
  bcb->comp_renderer = sys_renderer_add(bcb->entity);
  bcb->comp_destroyer = sys_destroyer_add(bcb->entity);
  generate_physics_box(bcb->entity, bcb->comp_physics, false, 145.0f, 184.0f, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f), COLLISION_CATEGORY_ENTITY);
  phy_body_set_angle(bcb->comp_physics->body, angle);
  bcb->comp_physics->move_enabled = false;
  bcb->comp_physics->rotate_enabled = false;
  generate_renderer(bcb->comp_renderer, bcb->comp_renderer, bcb->comp_physics, TANK_BODY, 2);
  generate_destroyer(bcb->comp_destroyer);
}

void free_berserker_clone_body(berserker_clone_body *bcb) {
  free_physics(bcb->comp_physics, false);
  free_renderer(bcb->comp_renderer);
}

