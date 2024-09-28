#pragma once

#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id id;
  int point_queue_count;
} comp_commander;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_commander;

extern ecs_id COMMANDER_COMPONENT_SIGNATURE;

void comp_commander_register(void);

void sys_commander_register(sys_commander *sc);
comp_commander *sys_commander_add(ecs_id eid);

