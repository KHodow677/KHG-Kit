#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/vector.h"

typedef struct {
  phy_body *body;
  phy_body *target_body;
  phy_constraint *pivot;
  bool follow_pos;
  bool follow_ang;
} follower_info;

typedef struct {
  ecs_id id;
} comp_follower;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_follower;

extern ecs_id FOLLOWER_COMPONENT_SIGNATURE;
extern follower_info NO_FOLLOWER;
extern utl_vector *FOLLOWER_INFO;

void comp_follower_register(comp_follower *cf);

void sys_follower_register(sys_follower *sf);
void sys_follower_add(ecs_id *eid, follower_info *info);
void sys_follower_free(bool need_free);

ecs_ret sys_follower_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata);

