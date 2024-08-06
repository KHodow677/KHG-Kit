#include "ecs_manager.h"
#include "comp_physics.h"
#include "comp_renderer.h"
#include "../physics/physics_setup.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"
#include <stdlib.h>
#include <stdio.h>

ecs_ecs *ECS;

sys_physics PHYSICS_SYSTEM = { 0 };
sys_renderer RENDERER_SYSTEM = { 0 };

void ecs_setup(cpSpace *sp, comp_physics *cp, comp_renderer *cr) {
  ECS = ecs_new(1024, NULL);
  comp_physics_setup(cp, sp);
  comp_physics_register(cp, ECS);
  sys_physics_register(&PHYSICS_SYSTEM, cp, ECS, &cp->info);
  comp_renderer_setup(cr, cp);
  comp_renderer_register(cr, ECS);
  sys_renderer_register(&RENDERER_SYSTEM, cr, cp, ECS, &cr->info);
  ecs_id e1 = ecs_create(ECS);
  ecs_add(ECS, e1, cr->id, NULL);
  ecs_add(ECS, e1, cp->id, NULL);
}

void ecs_cleanup(cpSpace *sp, comp_physics *cp, comp_renderer *cr) {
  comp_physics_free(cp);
  comp_renderer_free(cr);
  physics_free(sp);
  ecs_free(ECS);
}

