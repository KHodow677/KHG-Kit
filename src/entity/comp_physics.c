#include "comp_physics.h"
#include "ecs_setup.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/phy.h"
#include <stdio.h>

ecs_id PHYSICS_COMPONENT;

void comp_physics_setup(comp_physics *cp, cpSpace *sp) {
  cp->time = 0.0f;
  float radius = 5;
  float mass = 1;
  float moment = cpMomentForCircle(mass, 0, radius, cpvzero);
  cp->body = cpSpaceAddBody(sp, cpBodyNew(mass, moment));
  cpBodySetPosition(cp->body, cpv(0, 15));
  cp->shape = cpSpaceAddShape(sp, cpCircleShapeNew(cp->body, radius, cpvzero));
  cpShapeSetFriction(cp->shape, 0.7);
}

void comp_physics_free(comp_physics *cp) {
  cpShapeFree(cp->shape);
  cpBodyFree(cp->body);
}

void comp_physics_register(comp_physics *cp, ecs_ecs *ecs) {
  cp->id = ecs_register_component(ecs, sizeof(comp_physics), NULL, NULL);
  PHYSICS_COMPONENT = cp->id; 
}

void sys_physics_register(sys_physics *sp, comp_physics *cp, ecs_ecs *ecs, physics_info *info) {
  sp->id = ecs_register_system(ecs, sys_physics_update, NULL, NULL, info);
  ecs_require_component(ecs, sp->id, cp->id);
}

ecs_ret sys_physics_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  (void)ecs;
  (void)dt;
  (void)udata;
  physics_info *info = udata;
  for (int id = 0; id < entity_count; id++) {
    comp_physics *cp = ecs_get(ecs, entities[id], PHYSICS_COMPONENT);
    cp->time += dt; 
    cpVect pos = cpBodyGetPosition(info->body);
    cpVect vel = cpBodyGetVelocity(info->body);
    printf( "Time is %f. ballBody is at (%5.2f, %5.2f). It's velocity is (%5.2f, %5.2f)\n", cp->time, pos.x, pos.y, vel.x, vel.y);
    cpSpaceStep(info->space, dt);
  }
  printf("\n");
  return 0;
}
