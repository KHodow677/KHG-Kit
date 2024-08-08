#include "entity/ecs_manager.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "generators/elements/tank_body_generator.h"
#include "physics/physics_setup.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"
#include <stdlib.h>
#include <stdio.h>

ecs_ecs *ECS;

sys_physics PHYSICS_SYSTEM = { 0 };
sys_renderer RENDERER_SYSTEM = { 0 };

ecs_id E1;
ecs_id E2;

void ecs_setup(cpSpace *sp, comp_physics *cp1, comp_renderer *cr1, tank_body *tb) {
  ECS = ecs_new(1024, NULL);
  comp_physics_register(cp1, ECS);
  comp_renderer_register(cr1, ECS);
  sys_physics_register(&PHYSICS_SYSTEM, ECS);
  sys_renderer_register(&RENDERER_SYSTEM, ECS);

  generate_tank_body(tb, ECS, sp);
}

void ecs_cleanup(cpSpace *sp, tank_body *tb) {
  free_tank_body(tb);
  sys_physics_free(false);
  sys_renderer_free(false);
  physics_free(sp);
  ecs_free(ECS);
}

