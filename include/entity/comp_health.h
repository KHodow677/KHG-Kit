#pragma once

#include "khg_ecs/ecs.h"

typedef struct {
  float max_health;
  float current_health;
} health_info;

typedef struct {
  ecs_id id;
} comp_health;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_health;

extern ecs_id HEALTH_COMPONENT_SIGNATURE;
extern health_info NO_HEALTH;
extern health_info *HEALTH_INFO;

void comp_health_register(comp_health *ch);

void sys_health_register(sys_health *sh);
void sys_health_add(ecs_id *eid, health_info *info);

