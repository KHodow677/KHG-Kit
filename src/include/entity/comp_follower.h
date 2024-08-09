#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/map.h"

typedef struct {
  cpBody *body;
  cpBody *target_body;
} follower_info;

typedef struct {
  ecs_id id;
} comp_follower;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_follower;

extern ecs_id FOLLOWER_COMPONENT_SIGNATURE;
extern map *FOLLOWER_INFO_MAP;

void comp_follower_register(comp_follower *cf, ecs_ecs *ecs);

void sys_follower_register(sys_follower *sf, ecs_ecs *ecs);
void sys_follower_add(ecs_ecs *ecs, ecs_id *eid, follower_info *info);
void sys_follower_free(bool need_free);
ecs_ret sys_follower_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata);

