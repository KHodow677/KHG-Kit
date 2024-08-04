#include "comp_physics.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/phy.h"
#include <stdio.h>

void comp_physics_setup(comp_physics *cp, cpSpace *space) {
  cp->x = 0;
  cp->y = 0;
  float radius = 5;
  float mass = 1;
  float moment = cpMomentForCircle(mass, 0, radius, cpvzero);
  cpBody *ballBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));
  cpBodySetPosition(ballBody, cpv(0, 15));
  cpShape *ballShape = cpSpaceAddShape(space, cpCircleShapeNew(ballBody, radius, cpvzero));
  cpShapeSetFriction(ballShape, 0.7);
}

void comp_physics_register(comp_physics *cp, ecs_ecs *ecs) {
  cp->id = ecs_register_component(ecs, sizeof(comp_physics), NULL, NULL);
}

void comp_physics_register_with_sys_physics(comp_physics *cp, sys_physics *sp, ecs_ecs *ecs) {
  ecs_require_component(ecs, sp->id, cp->id);
}

void sys_physics_register(sys_physics *sp, ecs_ecs *ecs) {
  sp->id = ecs_register_system(ecs, sys_physics_update, NULL, NULL, NULL);
}

ecs_ret sys_physics_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  (void)ecs;
  (void)dt;
  (void)udata;
  for (int id = 0; id < entity_count; id++) {
    printf("%u ", entities[id]);
  }
  printf("\n");
  return 0;
}

