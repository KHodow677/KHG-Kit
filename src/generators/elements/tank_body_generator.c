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
  generate_physics_box(tb->entity, &tb->physics_info, true, 145.0f, 184.0f, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f), COLLISION_CATEGORY_ENTITY);
  phy_body_set_angle(tb->physics_info.body, angle);
  generate_renderer(&tb->renderer_info, &tb->physics_info, TANK_BODY, 2, tb->entity);
  generate_destroyer(&tb->destroyer_info);
  sys_physics_add(&tb->entity, &tb->physics_info);
  sys_renderer_add(&tb->entity, &tb->renderer_info);
  sys_destroyer_add(&tb->entity, &tb->destroyer_info);
}

void free_tank_body(tank_body *tb) {
  free_physics(&tb->physics_info, false);
  free_renderer(&tb->renderer_info);
  PHYSICS_INFO[tb->entity] = NO_PHYSICS;
  utl_vector_assign(RENDERER_INFO, tb->entity, &NO_RENDERER);
  DESTROYER_INFO[tb->entity] = NO_DESTROYER;
}

