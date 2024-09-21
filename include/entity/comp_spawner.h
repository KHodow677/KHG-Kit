#pragma once

#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id id;
} comp_spawner;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_spawner;

extern ecs_id SPAWNER_COMPONENT_SIGNATURE;

void comp_spawner_register(comp_spawner *cs);

void sys_spawner_register(sys_spawner *ss);
void sys_spawner_add(ecs_id *eid);

