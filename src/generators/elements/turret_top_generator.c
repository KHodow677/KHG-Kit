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
  tt->comp_physics = sys_physics_add(tt->entity);
  tt->comp_renderer = sys_renderer_add(tt->entity);
  tt->comp_destroyer = sys_destroyer_add(tt->entity);
  tt->comp_rotator = sys_rotator_add(tt->entity);
  tt->comp_shooter = sys_shooter_add(tt->entity);
  tt->comp_targeter = sys_targeter_add(tt->entity);
  tt->comp_damage = sys_damage_add(tt->entity);
  generate_physics_pivot(tt->entity, tt->comp_physics, tb->comp_physics, false, 85.0f, 133.0f, 1.0f, phy_v(x, y), 0.0f, phy_v(0.0f, 25.5f), COLLISION_CATEGORY_OBJECT);
  phy_body_set_angle(tt->comp_physics->body, angle);
  generate_renderer(tt->comp_renderer, tb->comp_renderer, tt->comp_physics, TURRET_TOP, 6);
  generate_destroyer(tt->comp_destroyer);
  generate_rotator(tt->comp_rotator, tt->comp_physics, angle);
  generate_shooter(tt->comp_shooter, 112.0f, 0.25f);
  generate_targeter(tt->comp_targeter, tb->comp_physics, tt->comp_physics, 448.0f);
  generate_damage(tt->comp_damage, 50.0f);
  tt->comp_physics->targeter_ref = tt->comp_targeter;
}

void free_turret_top(turret_top *tt) {
  free_physics(tt->comp_physics, true);
  free_renderer(tt->comp_renderer);
  free_targeter(tt->comp_targeter);
}

