#pragma once

#include "khg_ecs/ecs.h"
#include "khg_utl/vector.h"

typedef struct {
  float damage;
  utl_vector *target_entities;
} damage_info;

typedef struct {
  ecs_id id;
} comp_damage;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_damage;

extern ecs_id DAMAGE_COMPONENT_SIGNATURE;
extern damage_info NO_DAMAGE;
extern damage_info *DAMAGE_INFO;

void comp_damage_register(comp_damage *cd);

void sys_damage_register(sys_damage *sd);
void sys_damage_add(ecs_id *eid, damage_info *info);

