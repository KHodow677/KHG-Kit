#pragma once

#include "khg_ecs/ecs.h"
#include "khg_utl/vector.h"

typedef struct {
  float shoot_cooldown;
} shooter_info;

typedef struct {
  ecs_id id;
} comp_shooter;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_shooter;

extern ecs_id SHOOTER_COMPONENT_SIGNATURE;
extern shooter_info NO_SHOOTER;
extern utl_vector *SHOOTER_INFO;

void comp_shooter_register(comp_shooter *cs);

void sys_shooter_register(sys_shooter *ss);
void sys_shooter_add(ecs_id *eid, shooter_info *info);

ecs_ret sys_shooter_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata);

