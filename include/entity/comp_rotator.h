#pragma once

#include "entity/comp_health.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"

typedef struct {
  ecs_id id;
  phy_body *body;
  comp_health *target_health;
} comp_rotator;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_rotator;

extern ecs_id ROTATOR_COMPONENT_SIGNATURE;

void comp_rotator_register(void);

void sys_rotator_register(sys_rotator *sr);
comp_rotator *sys_rotator_add(ecs_id eid);

