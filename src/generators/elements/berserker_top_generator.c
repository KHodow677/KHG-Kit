#include "generators/elements/berserker_top_generator.h"
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
#include "generators/elements/berserker_body_generator.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include <math.h>

void generate_berserker_top(berserker_top *bt, berserker_body *bb, float x, float y, float angle) {
  bt->entity = ecs_create(ECS);
  bt->comp_physics = sys_physics_add(bt->entity);
  bt->comp_renderer = sys_renderer_add(bt->entity);
  bt->comp_destroyer = sys_destroyer_add(bt->entity);
  bt->comp_mover = sys_mover_add(bt->entity);
  bt->comp_rotator = sys_rotator_add(bt->entity);
  bt->comp_shooter = sys_shooter_add(bt->entity);
  bt->comp_selector = sys_selector_add(bt->entity);
  bt->comp_commander = sys_commander_add(bt->entity);
  bt->comp_targeter = sys_targeter_add(bt->entity);
  bt->comp_damage = sys_damage_add(bt->entity);
  generate_physics_pivot(bt->entity, bt->comp_physics, bb->comp_physics, false, 102.0f, 209.0f, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 55.5f), COLLISION_CATEGORY_ENTITY);
  phy_body_set_angle(bt->comp_physics->body, angle);
  generate_renderer(bt->comp_renderer, bb->comp_renderer, bt->comp_physics, TANK_TOP, 3);
  generate_destroyer(bt->comp_destroyer);
  phy_vect *move_forward = (phy_vect[]){ phy_v(x + 256 * sinf(angle), y + 256 * -cosf(angle)) };
  generate_mover(bt->comp_mover, bb->comp_physics, 300.0f, 16.0f, move_forward, 1);
  generate_rotator(bt->comp_rotator, bt->comp_physics, angle);
  generate_shooter(bt->comp_shooter, 155.0f, 1.0f);
  generate_selector(bt->comp_selector, TANK_TOP, TANK_BODY, TANK_TOP_OUTLINE, TANK_BODY_OUTLINE);
  generate_commander(bt->comp_commander, bt->comp_mover);
  generate_targeter(bt->comp_targeter, bb->comp_physics, bt->comp_physics, 640.0f);
  generate_damage(bt->comp_damage, 250.0f);
  bt->comp_physics->targeter_ref = bt->comp_targeter;
}

void free_berserker_top(berserker_top *bt) {
  free_physics(bt->comp_physics, true);
  free_renderer(bt->comp_renderer);
  free_mover(bt->comp_mover);
  free_targeter(bt->comp_targeter);
  free_selector(bt->comp_selector);
}

