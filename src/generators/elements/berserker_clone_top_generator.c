#include "generators/elements/berserker_clone_top_generator.h"
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
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include <math.h>

void generate_berserker_clone_top(berserker_clone_top *bct, berserker_clone_body *bcb, float x, float y, float angle) {
  bct->entity = ecs_create(ECS);
  bct->comp_physics = sys_physics_add(bct->entity);
  bct->comp_renderer = sys_renderer_add(bct->entity);
  bct->comp_destroyer = sys_destroyer_add(bct->entity);
  bct->comp_mover = sys_mover_add(bct->entity);
  bct->comp_rotator = sys_rotator_add(bct->entity);
  bct->comp_shooter = sys_shooter_add(bct->entity);
  bct->comp_selector = sys_selector_add(bct->entity);
  bct->comp_commander = sys_commander_add(bct->entity);
  bct->comp_targeter = sys_targeter_add(bct->entity);
  bct->comp_damage = sys_damage_add(bct->entity);
  generate_physics_pivot(bct->entity, bct->comp_physics, bcb->comp_physics, false, 102.0f, 209.0f, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 55.5f), COLLISION_CATEGORY_ENTITY);
  phy_body_set_angle(bct->comp_physics->body, angle);
  generate_renderer(bct->comp_renderer, bcb->comp_renderer, bct->comp_physics, TANK_TOP, 3);
  generate_destroyer(bct->comp_destroyer);
  phy_vect *move_forward = (phy_vect[]){ phy_v(x + 256 * sinf(angle), y + 256 * -cosf(angle)) };
  generate_mover(bct->comp_mover, bcb->comp_physics, 300.0f, 16.0f, move_forward, 1);
  generate_rotator(bct->comp_rotator, bct->comp_physics, angle);
  generate_shooter(bct->comp_shooter, 155.0f, 1.0f);
  generate_selector(bct->comp_selector, TANK_TOP, TANK_BODY, TANK_TOP_OUTLINE, TANK_BODY_OUTLINE);
  generate_commander(bct->comp_commander, bct->comp_mover);
  generate_targeter(bct->comp_targeter, bcb->comp_physics, bct->comp_physics, 640.0f);
  generate_damage(bct->comp_damage, 500.0f);
  bct->comp_physics->targeter_ref = bct->comp_targeter;
}

void free_berserker_clone_top(berserker_clone_top *bct) {
  free_physics(bct->comp_physics, true);
  free_renderer(bct->comp_renderer);
  free_mover(bct->comp_mover);
  free_targeter(bct->comp_targeter);
}

