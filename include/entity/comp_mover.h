#pragma once

#include "khg_ecs/ecs.h"
#include "khg_utl/queue.h"

typedef struct {
  ecs_id id;
  utl_queue *target_pos_queue;
  ecs_id body_entity;
  float max_vel;
  float max_ang_vel;
} comp_mover;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_mover;

extern ecs_id MOVER_COMPONENT_SIGNATURE;

void comp_mover_register(comp_mover *cm);

void sys_mover_register(sys_mover *sm);
comp_mover *sys_mover_add(ecs_id eid);

