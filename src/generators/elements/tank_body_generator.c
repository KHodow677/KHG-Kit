#include "generators/elements/tank_body_generator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_mover.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "game_manager.h"
#include "generators/components/comp_info_generator.h"
#include "generators/components/texture_generator.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/vect.h"
#include "khg_utl/vector.h"

void generate_tank_body(tank_body *tb, float x, float y) {
  generate_physics_box(&tb->physics_info, false, 145.0f, 184.0f, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 0.0f));
  generate_renderer(&tb->renderer_info, &tb->physics_info, TANK_BODY);
  generate_destroyer(&tb->destroyer_info);
  generate_mover(&tb->mover_info, &tb->physics_info);
  tb->entity = ecs_create(ECS);
  sys_physics_add(&tb->entity, &tb->physics_info);
  sys_renderer_add(&tb->entity, &tb->renderer_info);
  sys_destroyer_add(&tb->entity, &tb->destroyer_info);
  sys_mover_add(&tb->entity, &tb->mover_info);
}

void free_tank_body(tank_body *tb) {
  free_physics(&tb->physics_info, false);
  utl_vector_assign(PHYSICS_INFO, tb->entity, &NO_PHYSICS);
  utl_map_erase(RENDERER_INFO_MAP, &tb->entity);
  utl_vector_assign(DESTROYER_INFO, tb->entity, &NO_DESTROYER);
  utl_vector_assign(MOVER_INFO, tb->entity, &NO_MOVER);
}

