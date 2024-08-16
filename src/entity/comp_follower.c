#include "entity/comp_follower.h"
#include "data_utl/kinematic_utl.h"
#include "entity/comp_physics.h"
#include "data_utl/map_utl.h"
#include "entity/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include "khg_utl/map.h"
#include <stdio.h>

ecs_id FOLLOWER_COMPONENT_SIGNATURE;
utl_map *FOLLOWER_INFO_MAP = NULL;

void comp_follower_register(comp_follower *cf, ecs_ecs *ecs) {
  cf->id = ecs_register_component(ecs, sizeof(comp_follower), NULL, NULL);
  FOLLOWER_COMPONENT_SIGNATURE = cf->id; 
}

void sys_follower_register(sys_follower *sf, ecs_ecs *ecs) {
  sf->id = ecs_register_system(ecs, sys_follower_update, NULL, NULL, NULL);
  ecs_require_component(ecs, sf->id, FOLLOWER_COMPONENT_SIGNATURE);
  ecs_require_component(ecs, sf->id, PHYSICS_COMPONENT_SIGNATURE);
  sf->ecs = *ecs;
  sf->current_degree = 1;
  FOLLOWER_INFO_MAP = utl_map_create(compare_ints, no_deallocator, no_deallocator);
}

void sys_follower_add(ecs_ecs *ecs, ecs_id *eid, follower_info *info) {
  ecs_add(ecs, *eid, FOLLOWER_COMPONENT_SIGNATURE, NULL);
  utl_map_insert(FOLLOWER_INFO_MAP, eid, info);
}

void sys_follower_free(bool need_free) {
  if (need_free) {
    utl_map_deallocate(FOLLOWER_INFO_MAP);
  }
}

ecs_ret sys_follower_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  (void)ecs;
  (void)dt;
  (void)udata;
  if (entity_count == 0) {
    return 0;
  }
  follower_info *info = utl_map_at(FOLLOWER_INFO_MAP, &entities[0]);
  int current_degree = FOLLOWER_SYSTEM.current_degree;
  for (int id = 0; id < entity_count; id++) {
    info = utl_map_at(FOLLOWER_INFO_MAP, &entities[id]);
    if (info->degree != current_degree) {
      continue;
    }
    if (info->follow_ang) {
      float target_ang_vel = cpBodyGetAngularVelocity(info->target_body) + cpBodyGetAngularVelocity(info->body);
      cpBodySetAngularVelocity(info->body, target_ang_vel);
    }
    if (info->follow_pos) {
      phy_vect target_vel = cpvadd(cpBodyGetVelocity(info->target_body), cpBodyGetVelocity(info->body));
      cpBodySetVelocity(info->body, target_vel);
    }
  }
  return 0;
}
