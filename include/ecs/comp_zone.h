#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"

typedef struct comp_zone {
  ecs_id id;
  const nvRigidBody *body;
  float min_x;
  float max_x;
} comp_zone;

typedef struct comp_zone_constructor_info {
  const nvRigidBody *body;
  float min_x;
  float max_x;
} comp_zone_constructor_info;

extern ecs_id ZONE_COMPONENT_SIGNATURE;
extern ecs_id ZONE_SYSTEM_SIGNATURE;

extern comp_zone_constructor_info *ZONE_CONSTRUCTOR_INFO;

void comp_zone_register(void);
void sys_zone_register(void);

comp_zone *sys_zone_add(const ecs_id eid, comp_zone_constructor_info *czci);

