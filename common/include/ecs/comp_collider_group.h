#pragma once

#include "area/collider.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id id;
  unsigned int area_id;
  area_colliders colliders;
} comp_collider_group;

typedef struct {
  bool enabled;
  unsigned int area_id;
} comp_collider_group_constructor_info;

extern ecs_id COLLIDER_GROUP_COMPONENT_SIGNATURE;
extern ecs_id COLLIDER_GROUP_SYSTEM_SIGNATURE;

extern comp_collider_group_constructor_info *COLLIDER_GROUP_CONSTRUCTOR_INFO;

void comp_collider_group_register(void);
void sys_collider_group_register(void);

comp_collider_group *sys_collider_group_add(const ecs_id eid, comp_collider_group_constructor_info *ccgci);

