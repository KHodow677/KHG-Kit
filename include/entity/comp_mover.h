#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/map.h"

typedef struct {
  cpBody *body;
  cpVect target_move_pos;
  cpVect target_look_pos;
} mover_info;

typedef struct {
  ecs_id id;
} comp_mover;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_mover;

extern ecs_id MOVER_COMPONENT_SIGNATURE;
extern utl_map *MOVER_INFO_MAP;

void comp_mover_register(comp_mover *cm, ecs_ecs *ecs);

void sys_mover_register(sys_mover *sm, ecs_ecs *ecs);
void sys_mover_add(ecs_ecs *ecs, ecs_id *eid, mover_info *info);
void sys_mover_free(bool need_free);
ecs_ret sys_mover_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata);
