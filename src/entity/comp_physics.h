#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"

typedef struct {
  cpBody *body;
  cpShape *shape;
  cpSpace *space;
} physics_info;

typedef struct {
  ecs_id id;
  float time;
  physics_info info;
} comp_physics;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_physics;

extern ecs_id PHYSICS_COMPONENT_SIGNATURE;

void comp_physics_setup(comp_physics *cp, cpSpace *space);
void comp_physics_free(comp_physics *cp);
void comp_physics_register(comp_physics *cp, ecs_ecs *ecs);
void sys_physics_register(sys_physics *sp, comp_physics *cp, ecs_ecs *ecs, physics_info *info);
ecs_ret sys_physics_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata);

