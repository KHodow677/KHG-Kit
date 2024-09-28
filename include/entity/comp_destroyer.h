#pragma once

#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id id;
  bool destroy_now;
} comp_destroyer;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_destroyer;

extern ecs_id DESTROYER_COMPONENT_SIGNATURE;

void comp_destroyer_register(comp_destroyer *cd);

void sys_destroyer_register(sys_destroyer *sd);
comp_destroyer *sys_destroyer_add(ecs_id eid);

