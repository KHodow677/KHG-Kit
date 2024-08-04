#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"

typedef struct {
  ecs_id id;
  float x, y;
} comp_physics;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_physics;

void comp_physics_setup(comp_physics *cp, cpSpace *space);
void comp_physics_register(comp_physics *cp, ecs_ecs *ecs);
ecs_ret sys_physics_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata);

