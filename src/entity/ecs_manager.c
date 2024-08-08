#include "entity/ecs_manager.h"
#include "entity/comp_follower.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "generators/elements/tank_body_generator.h"
#include "generators/elements/tank_top_generator.h"
#include "physics/physics_setup.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"
#include <stdlib.h>
#include <stdio.h>

ecs_ecs *ECS;

sys_physics PHYSICS_SYSTEM = { 0 };
sys_renderer RENDERER_SYSTEM = { 0 };
sys_follower FOLLOWER_SYSTEM = { 0 };

ecs_id E1;
ecs_id E2;

void ecs_setup(cpSpace *sp, comp_physics *cp, comp_renderer *cr, comp_follower *cf, tank_body *tb, tank_top *tt) {
  ECS = ecs_new(1024, NULL);
  comp_physics_register(cp, ECS);
  comp_renderer_register(cr, ECS);
  comp_follower_register(cf, ECS);
  sys_physics_register(&PHYSICS_SYSTEM, ECS);
  sys_renderer_register(&RENDERER_SYSTEM, ECS);
  sys_follower_register(&FOLLOWER_SYSTEM, ECS);

  generate_tank_body(tb, ECS, sp);
  generate_tank_top(tt, ECS, sp, tb);
}

void ecs_cleanup(cpSpace *sp, tank_body *tb, tank_top *tt) {
  free_tank_body(tb);
  free_tank_top(tt);
  sys_physics_free(false);
  sys_renderer_free(false);
  sys_follower_free(false);
  physics_free(sp);
  ecs_free(ECS);
}

