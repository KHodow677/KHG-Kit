#pragma once

#include "entity/comp_health.h"
#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id id;
  float damage;
  comp_health *target_health;
  bool killed;
} comp_damage;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_damage;

extern ecs_id DAMAGE_COMPONENT_SIGNATURE;

void comp_damage_register(void);

void sys_damage_register(sys_damage *sd);
comp_damage *sys_damage_add(ecs_id eid);

