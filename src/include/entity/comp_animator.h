#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/map.h"

typedef struct {
  cpBody *body;
} animator_info;

typedef struct {
  ecs_id id;
} comp_animator;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_animator;

extern ecs_id ANIMATOR_COMPONENT_SIGNATURE;
extern map *ANIMATOR_INFO_MAP;

void comp_animator_register(comp_animator *ca, ecs_ecs *ecs);

void sys_animator_register(sys_animator *sa, ecs_ecs *ecs);
void sys_animator_add(ecs_ecs *ecs, ecs_id *eid, animator_info *info);
void sys_animator_free(bool need_free);
ecs_ret sys_animator_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata);

