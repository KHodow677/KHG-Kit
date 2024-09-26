#pragma once

#include "khg_ecs/ecs.h"

typedef struct {
  bool destroy_now;
} destroyer_info;

typedef struct {
  ecs_id id;
} comp_destroyer;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_destroyer;

extern ecs_id DESTROYER_COMPONENT_SIGNATURE;
extern destroyer_info NO_DESTROYER;
extern destroyer_info *DESTROYER_INFO;

void comp_destroyer_register(comp_destroyer *cd);

void sys_destroyer_register(sys_destroyer *sd);
void sys_destroyer_add(ecs_id *eid, destroyer_info *info);

