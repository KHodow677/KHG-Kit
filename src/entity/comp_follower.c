#include "entity/comp_follower.h"
#include "data_utl/kinematic_utl.h"
#include "entity/comp_physics.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_utl/vector.h"
#include <stdio.h>

ecs_id FOLLOWER_COMPONENT_SIGNATURE;
follower_info NO_FOLLOWER = { 0 };
utl_vector *FOLLOWER_INFO = NULL;

void comp_follower_register(comp_follower *cf) {
  cf->id = ecs_register_component(ECS, sizeof(comp_follower), NULL, NULL);
  FOLLOWER_COMPONENT_SIGNATURE = cf->id; 
}

void sys_follower_register(sys_follower *sf) {
  sf->id = ecs_register_system(ECS, sys_follower_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sf->id, FOLLOWER_COMPONENT_SIGNATURE);
  sf->ecs = *ECS;
  FOLLOWER_INFO = utl_vector_create(sizeof(follower_info));
  for (int i = 0; i < ECS->entity_count; i++) {
    utl_vector_push_back(FOLLOWER_INFO, &NO_FOLLOWER);
  }
}

void sys_follower_add(ecs_id *eid, follower_info *info) {
  ecs_add(ECS, *eid, FOLLOWER_COMPONENT_SIGNATURE, NULL);
  utl_vector_assign(FOLLOWER_INFO, *eid, info);
}

void sys_follower_free(bool need_free) {
  if (need_free) {
    utl_vector_deallocate(FOLLOWER_INFO);
  }
}

ecs_ret sys_follower_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  return 0;
}
