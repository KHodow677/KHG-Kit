#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/map.h"

typedef struct {
  cpBody *body;
  cpVect target_move_pos;
  cpVect target_look_pos;
} rotator_info;

typedef struct {
  ecs_id id;
} comp_rotator;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_rotator;

extern ecs_id ROTATER_COMPONENT_SIGNATURE;
extern utl_map *ROTATER_INFO_MAP;

void comp_rotator_register(comp_rotator *cr, ecs_ecs *ecs);

void sys_rotator_register(sys_rotator *sr, ecs_ecs *ecs);
void sys_rotator_add(ecs_ecs *ecs, ecs_id *eid, rotator_info *info);
void sys_rotator_free(bool need_free);
ecs_ret sys_rotator_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata);
