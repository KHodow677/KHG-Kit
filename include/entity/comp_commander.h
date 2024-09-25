#pragma once

#include "khg_ecs/ecs.h"
#include "khg_utl/vector.h"

typedef struct {
  int point_queue_count;
} commander_info;

typedef struct {
  ecs_id id;
} comp_commander;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_commander;

extern ecs_id COMMANDER_COMPONENT_SIGNATURE;
extern commander_info NO_COMMANDER;
extern utl_vector *COMMANDER_INFO;

void comp_commander_register(comp_commander *cc);

void sys_commander_register(sys_commander *sc);
void sys_commander_add(ecs_id *eid, commander_info *info);

