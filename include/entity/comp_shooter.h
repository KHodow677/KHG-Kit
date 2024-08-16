#pragma once

#include "khg_ecs/ecs.h"
#include "khg_utl/map.h"

typedef struct {
  bool shoot_now;
} shooter_info;

typedef struct {
  ecs_id id;
} comp_shooter;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_shooter;

extern ecs_id SHOOTER_COMPONENT_SIGNATURE;
extern utl_map *SHOOTER_INFO_MAP;

void comp_shooter_register(comp_shooter *cs, ecs_ecs *ecs);

void sys_shooter_register(sys_shooter *ss, ecs_ecs *ecs);
void sys_shooter_add(ecs_ecs *ecs, ecs_id *eid, shooter_info *info);
void sys_shooter_free(bool need_free);
ecs_ret sys_shooter_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata);

