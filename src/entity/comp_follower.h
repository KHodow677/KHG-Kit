#pragma once

#include "comp_physics.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"

typedef struct {
  comp_physics *cp;
  comp_physics *follow_cp;
} follower_info;

typedef struct {
  ecs_id id;
  cpVect pos_offset;
  cpFloat angle_offset;
  follower_info info;
} comp_follower;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_follower;

extern ecs_id FOLLOWER_COMPONENT_SIGNATURE;

void comp_follower_setup(comp_follower *cf, comp_physics *cp, comp_physics *follow_cp);
void comp_follower_free(comp_follower *cf);
void comp_follower_register(comp_follower *cf, ecs_ecs *ecs);
void sys_follower_register(sys_follower *sr, comp_follower *cf, comp_physics *cp, ecs_ecs *ecs, follower_info *info);
ecs_ret sys_follower_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata);

