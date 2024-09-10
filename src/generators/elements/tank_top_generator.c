#include "generators/elements/tank_top_generator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_shooter.h"
#include "game_manager.h"
#include "generators/components/comp_info_generator.h"
#include "generators/components/texture_generator.h"
#include "generators/elements/tank_body_generator.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/vect.h"

void generate_tank_top(tank_top *tt, tank_body *tb, float x, float y) {
  generate_physics_pivot(&tt->physics_info, &tb->physics_info, false, 102.0f, 209.0f, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 55.5f));
  generate_renderer(&tt->renderer_info, &tt->physics_info, TANK_TOP, 2);
  generate_destroyer(&tt->destroyer_info);
  generate_rotator(&tt->rotator_info, &tt->physics_info);
  generate_shooter(&tt->shooter_info);
  generate_selector(&tt->selector_info);
  tt->entity = ecs_create(ECS);
  sys_physics_add(&tt->entity, &tt->physics_info);
  sys_renderer_add(&tt->entity, &tt->renderer_info);
  sys_destroyer_add(&tt->entity, &tt->destroyer_info);
  sys_rotator_add(&tt->entity, &tt->rotator_info);
  sys_shooter_add(&tt->entity, &tt->shooter_info);
  sys_selector_add(&tt->entity, &tt->selector_info);
}

void free_tank_top(tank_top *tt) {
  free_physics(&tt->physics_info, true);
  free_renderer(&tt->renderer_info);
  utl_vector_assign(PHYSICS_INFO, tt->entity, &NO_PHYSICS);
  utl_vector_assign(RENDERER_INFO, tt->entity, &NO_RENDERER);
  utl_vector_assign(DESTROYER_INFO, tt->entity, &NO_DESTROYER);
  utl_map_erase(ROTATOR_INFO_MAP, &tt->entity);
  utl_vector_assign(SHOOTER_INFO, tt->entity, &NO_SHOOTER);
  utl_vector_assign(SELECTOR_INFO, tt->entity, &NO_SELECTOR);
}

