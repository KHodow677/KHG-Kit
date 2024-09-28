#pragma once

#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id id;
} comp_spawn;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_spawn;

extern ecs_id SPAWNER_COMPONENT_SIGNATURE;

void comp_spawn_register(comp_spawn *cs);

void sys_spawn_register(sys_spawn *ss);
comp_spawn *sys_spawn_add(ecs_id eid);

