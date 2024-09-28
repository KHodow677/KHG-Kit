#pragma once

#include "entity/comp_health.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"

typedef struct {
  phy_body *body;
  health_info *target_health;
} rotator_info;

typedef struct {
  ecs_id id;
} comp_rotator;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_rotator;

extern ecs_id ROTATOR_COMPONENT_SIGNATURE;
extern rotator_info NO_ROTATOR;
extern rotator_info *ROTATOR_INFO;

void comp_rotator_register(comp_rotator *cr);

void sys_rotator_register(sys_rotator *sr);
void sys_rotator_add(ecs_id *eid, rotator_info *info);

