#include "generators/elements/tank_body_generator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "game_manager.h"
#include "generators/components/comp_info_generator.h"
#include "generators/components/texture_generator.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/vect.h"
#include "khg_utl/vector.h"

void generate_tank_body(tank_body *tb, float x, float y, float angle) {
  tb->entity = ecs_create(ECS);
  tb->comp_physics = sys_physics_add(tb->entity);
  tb->comp_destroyer = sys_destroyer_add(tb->entity);
  generate_physics_box(tb->entity, tb->comp_physics, true, 145.0f, 184.0f, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f), COLLISION_CATEGORY_ENTITY);
  phy_body_set_angle(tb->comp_physics->body, angle);
  generate_renderer(&tb->renderer_info, tb->comp_physics, TANK_BODY, 2, tb->entity);
  generate_destroyer(tb->comp_destroyer);
  sys_renderer_add(&tb->entity, &tb->renderer_info);
}

void free_tank_body(tank_body *tb) {
  free_physics(tb->comp_physics, false);
  free_renderer(&tb->renderer_info);
  utl_vector_assign(RENDERER_INFO, tb->entity, &NO_RENDERER);
}

