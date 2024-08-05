#include "ecs_setup.h"
#include "comp_physics.h"
#include "khg_phy/phy_types.h"
#include <stdlib.h>
#include <stdio.h>

int ecs_setup(cpSpace *sp) {
  comp_physics physics_component;
  sys_physics physics_system;
  ecs_ecs *ecs = ecs_new(1024, NULL);
  comp_physics_setup(&physics_component, sp);
  comp_physics_register(&physics_component, ecs);
  physics_info info = { physics_component.body, physics_component.shape, sp };
  sys_physics_register(&physics_system, &physics_component, ecs, &info);
  ecs_id e1 = ecs_create(ecs);
  ecs_add(ecs, e1, physics_component.id, NULL);
  printf("Executing system\n");
  ecs_update_system(ecs, physics_system.id, 1.0f / 60.0f);
  ecs_update_system(ecs, physics_system.id, 1.0f / 60.0f);
  ecs_update_system(ecs, physics_system.id, 1.0f / 60.0f);
  ecs_update_system(ecs, physics_system.id, 1.0f / 60.0f);
  comp_physics_free(&physics_component);
  ecs_free(ecs);
  return 0;
}

