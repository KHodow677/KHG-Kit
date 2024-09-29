#pragma once

#include "khg_ecs/ecs.h"
#include "khg_utl/queue.h"

typedef struct {
  ecs_id id;
  utl_queue **ext_mover_ref;
  utl_queue **loc_mover_ref;
} comp_copier;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_copier;

extern ecs_id COPIER_COMPONENT_SIGNATURE;

void comp_copier_register(void);

void sys_copier_register(sys_copier *sm);
comp_copier *sys_copier_add(ecs_id eid);

