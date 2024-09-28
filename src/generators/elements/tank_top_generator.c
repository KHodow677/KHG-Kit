#include "generators/elements/tank_top_generator.h"
#include "entity/comp_commander.h"
#include "entity/comp_damage.h"
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
  tt->comp_physics = sys_physics_add(tt->entity);
  tt->comp_renderer = sys_renderer_add(tt->entity);
  tt->comp_destroyer = sys_destroyer_add(tt->entity);
  tt->comp_mover = sys_mover_add(tt->entity);
  tt->comp_rotator = sys_rotator_add(tt->entity);
  tt->comp_shooter = sys_shooter_add(tt->entity);
  tt->comp_selector = sys_selector_add(tt->entity);
  tt->comp_commander = sys_commander_add(tt->entity);
  tt->comp_damage = sys_damage_add(tt->entity);
  generate_physics_pivot(tt->entity, tt->comp_physics, tb->comp_physics, false, 102.0f, 209.0f, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 55.5f), COLLISION_CATEGORY_ENTITY);
  phy_body_set_angle(tt->comp_physics->body, angle);
  generate_renderer(tt->comp_renderer, tb->comp_renderer, tt->comp_physics, TANK_TOP, 2);
  generate_destroyer(tt->comp_destroyer);
  generate_mover(tt->comp_mover, tb->comp_physics, 300.0f, 16.0f, NULL, 0);
  generate_rotator(tt->comp_rotator, tt->comp_physics, angle);
  generate_shooter(tt->comp_shooter, 155.0f, 1.0f);
  generate_selector(tt->comp_selector, TANK_TOP, TANK_BODY, TANK_TOP_OUTLINE, TANK_BODY_OUTLINE);
  generate_commander(tt->comp_commander, tt->comp_mover);
  generate_targeter(&tt->targeter_info, tb->comp_physics, tt->comp_physics, 400.0f);
  generate_damage(tt->comp_damage, 500.0f);
  sys_targeter_add(&tt->entity, &tt->targeter_info);
  tt->comp_physics->targeter_ref = &TARGETER_INFO[tt->entity];
}

void free_tank_top(tank_top *tt) {
  free_physics(tt->comp_physics, true);
  free_renderer(tt->comp_renderer);
  free_mover(tt->comp_mover);
  free_targeter(&tt->targeter_info);
  TARGETER_INFO[tt->entity] = NO_TARGETER;
}

