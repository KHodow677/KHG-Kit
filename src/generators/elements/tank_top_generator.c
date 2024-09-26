#include "generators/elements/tank_top_generator.h"
#include "entity/comp_commander.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_rotator.h"
#include "entity/comp_shooter.h"
#include "entity/comp_targeter.h"
#include "game_manager.h"
#include "generators/components/comp_info_generator.h"
#include "generators/components/texture_generator.h"
#include "generators/elements/tank_body_generator.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/vect.h"

void generate_tank_top(tank_top *tt, tank_body *tb, float x, float y, float angle) {
  tt->entity = ecs_create(ECS);
  generate_physics_pivot(tt->entity, &tt->physics_info, &tb->physics_info, false, 102.0f, 209.0f, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 55.5f), COLLISION_CATEGORY_ENTITY);
  phy_body_set_angle(tt->physics_info.body, angle);
  generate_renderer(&tt->renderer_info, &tt->physics_info, TANK_TOP, 2, tb->entity);
  generate_destroyer(&tt->destroyer_info);
  generate_mover(&tt->mover_info, tb->entity, 300.0f, 16.0f, NULL, 0);
  generate_rotator(&tt->rotator_info, &tt->physics_info, angle);
  generate_shooter(&tt->shooter_info, 145.0f);
  generate_selector(&tt->selector_info, TANK_TOP, TANK_BODY, TANK_TOP_OUTLINE, TANK_BODY_OUTLINE);
  generate_commander(&tt->commander_info, &tt->mover_info);
  generate_targeter(&tt->targeter_info, &tb->physics_info, 256.0f);
  sys_physics_add(&tt->entity, &tt->physics_info);
  sys_renderer_add(&tt->entity, &tt->renderer_info);
  sys_destroyer_add(&tt->entity, &tt->destroyer_info);
  sys_mover_add(&tt->entity, &tt->mover_info);
  sys_rotator_add(&tt->entity, &tt->rotator_info);
  sys_shooter_add(&tt->entity, &tt->shooter_info);
  sys_selector_add(&tt->entity, &tt->selector_info);
  sys_commander_add(&tt->entity, &tt->commander_info);
}

void free_tank_top(tank_top *tt) {
  free_physics(&tt->physics_info, true);
  free_renderer(&tt->renderer_info);
  free_mover(&tt->mover_info);
  free_targeter(&tt->targeter_info);
  PHYSICS_INFO[tt->entity] = NO_PHYSICS;
  utl_vector_assign(RENDERER_INFO, tt->entity, &NO_RENDERER);
  DESTROYER_INFO[tt->entity] = NO_DESTROYER;
  MOVER_INFO[tt->entity] = NO_MOVER;
  utl_vector_assign(ROTATOR_INFO, tt->entity, &NO_ROTATOR);
  utl_vector_assign(SHOOTER_INFO, tt->entity, &NO_SHOOTER);
  utl_vector_assign(SELECTOR_INFO, tt->entity, &NO_SELECTOR);
  TARGETER_INFO[tt->entity] = NO_TARGETER;
}

