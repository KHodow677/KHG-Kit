#include "generators/elements/turret_top_generator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_rotator.h"
#include "entity/comp_shooter.h"
#include "game_manager.h"
#include "generators/components/comp_info_generator.h"
#include "generators/components/texture_generator.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/vect.h"

void generate_turret_top(turret_top *tt, turret_base *tb, float x, float y, float angle) {
  tt->entity = ecs_create(ECS);
  generate_physics_pivot(&tt->physics_info, &tb->physics_info, false, 85.0f, 133.0f, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 25.5f));
  phy_body_set_angle(tt->physics_info.body, angle);
  generate_renderer(&tt->renderer_info, &tt->physics_info, TURRET_TOP, 3, tb->entity);
  generate_destroyer(&tt->destroyer_info);
  generate_rotator(&tt->rotator_info, &tt->physics_info, angle);
  generate_shooter(&tt->shooter_info, 107.0f);
  sys_physics_add(&tt->entity, &tt->physics_info);
  sys_renderer_add(&tt->entity, &tt->renderer_info);
  sys_destroyer_add(&tt->entity, &tt->destroyer_info);
  sys_rotator_add(&tt->entity, &tt->rotator_info);
  sys_shooter_add(&tt->entity, &tt->shooter_info);
}

void free_turret_top(turret_top *tt) {
  free_physics(&tt->physics_info, true);
  free_renderer(&tt->renderer_info);
  utl_vector_assign(PHYSICS_INFO, tt->entity, &NO_PHYSICS);
  utl_vector_assign(RENDERER_INFO, tt->entity, &NO_RENDERER);
  utl_vector_assign(DESTROYER_INFO, tt->entity, &NO_DESTROYER);
  utl_vector_assign(ROTATOR_INFO, tt->entity, &NO_ROTATOR);
  utl_vector_assign(SHOOTER_INFO, tt->entity, &NO_SHOOTER);
}

