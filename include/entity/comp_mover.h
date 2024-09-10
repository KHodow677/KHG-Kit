#pragma once

#include "khg_ecs/ecs.h"
#include "khg_utl/queue.h"
#include "khg_utl/vector.h"

typedef struct {
  utl_queue *target_pos_queue;
  ecs_id body_entity;
} mover_info;

typedef struct {
  ecs_id id;
} comp_mover;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_mover;

extern ecs_id MOVER_COMPONENT_SIGNATURE;
extern mover_info NO_MOVER;
extern utl_vector *MOVER_INFO;

void comp_mover_register(comp_mover *cm);

void sys_mover_register(sys_mover *sm);
void sys_mover_add(ecs_id *eid, mover_info *info);

