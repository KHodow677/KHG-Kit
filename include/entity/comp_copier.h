#pragma once

#include "entity/comp_mover.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id id;
  comp_mover *ext_mover_ref;
} comp_copier;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_copier;

extern ecs_id COPIER_COMPONENT_SIGNATURE;

void comp_copier_register(void);

void sys_copier_register(sys_copier *sm);
comp_copier *sys_copier_add(ecs_id eid);

