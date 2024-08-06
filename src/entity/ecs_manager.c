#include "ecs_manager.h"
#include "comp_follower.h"
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

ecs_id E1;
ecs_id E2;

void ecs_setup(cpSpace *sp, comp_physics *cp1, comp_renderer *cr1, comp_physics *cp2, comp_renderer *cr2, comp_follower *cf) {
  ECS = ecs_new(1024, NULL);
  comp_physics_setup(cp1, sp, false);
  comp_physics_register(cp1, ECS);
  comp_physics_setup(cp2, sp, false);
  comp_physics_register(cp2, ECS);
  sys_physics_register(&PHYSICS_SYSTEM, ECS);
  
  comp_renderer_setup(cr1, cp1, "Tank-Body-Blue", "png");
  comp_renderer_register(cr1, ECS);
  sys_renderer_register(&RENDERER_SYSTEM, cr1, cp1, ECS, &cr1->info);
  
  E1 = ecs_create(ECS);
  E2 = ecs_create(ECS);
  sys_physics_add(ECS, &E1, cp1->id, &cp1->info);
  sys_physics_add(ECS, &E2, cp2->id, &cp2->info);
  ecs_add(ECS, E1, cr1->id, NULL);
}

void ecs_cleanup(cpSpace *sp, comp_physics *cp1, comp_renderer *cr1, comp_physics *cp2, comp_renderer *cr2) {
  comp_physics_free(cp1);
  comp_renderer_free(cr1);
  comp_physics_free(cp2);
  comp_renderer_free(cr2);
  sys_physics_free(false);
  physics_free(sp);
  ecs_free(ECS);
}

