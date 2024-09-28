#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"

typedef struct {
  ecs_id id;
  phy_body *body;
  float max_health;
  float current_health;
} comp_health;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_health;

extern ecs_id HEALTH_COMPONENT_SIGNATURE;

void comp_health_register(void);

void sys_health_register(sys_health *sh);
comp_health *sys_health_add(ecs_id eid);

