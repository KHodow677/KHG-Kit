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

ecs_id E1;
ecs_id E2;

void ecs_setup(cpSpace *sp, comp_physics *cp1, comp_renderer *cr1, physics_info *pi1, physics_info *pi2, renderer_info *ri1, renderer_info *ri2) {
  ECS = ecs_new(1024, NULL);
  info_physics_setup(pi1, sp, false);
  info_physics_setup(pi2, sp, false);
  comp_physics_register(cp1, ECS);
  sys_physics_register(&PHYSICS_SYSTEM, ECS);

  info_renderer_setup(ri1, pi1, "Tank-Body-Blue", "png"); 
  info_renderer_setup(ri2, pi2, "Tank-Top-Blue", "png"); 
  comp_renderer_register(cr1, ECS);
  sys_renderer_register(&RENDERER_SYSTEM, ECS);
  
  E1 = ecs_create(ECS);
  E2 = ecs_create(ECS);
  sys_physics_add(ECS, &E1, pi1);
  sys_physics_add(ECS, &E2, pi2);
  sys_renderer_add(ECS, &E1, ri1);
  sys_renderer_add(ECS, &E2, ri2);
  ecs_add(ECS, E1, cr1->id, NULL);
}

void ecs_cleanup(cpSpace *sp, comp_physics *cp1, comp_renderer *cr1, physics_info *pi1, physics_info *pi2, renderer_info *ri1, renderer_info *ri2) {
  info_physics_free(pi1);
  info_physics_free(pi2);
  info_renderer_free(ri1);
  info_renderer_free(ri2);
  sys_physics_free(false);
  physics_free(sp);
  ecs_free(ECS);
}

