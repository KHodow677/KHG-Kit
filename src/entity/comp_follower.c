#include "entity/comp_follower.h"
#include "data_utl/kinematic_utl.h"
#include "entity/comp_physics.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
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
  follower_info *info;
  for (int id = 0; id < entity_count; id++) {
    info = utl_vector_at(FOLLOWER_INFO, entities[id]);
    phy_vect target_pos = phy_body_get_position(info->target_body);
    phy_vect pos = phy_body_get_position(info->body);
    printf("TARGET -> x: %f y: %f\n", target_pos.x, target_pos.y);
    printf("CURRENT -> x: %f y: %f\n", pos.x, pos.y);
    /*if (info->follow_ang) {*/
    /*  float target_ang_vel = phy_body_get_angular_velocity(info->target_body) + phy_body_get_angular_velocity(info->body);*/
    /*  phy_body_set_angular_velocity(info->body, target_ang_vel);*/
    /*}*/
    /*if (info->follow_pos) {*/
    /*  phy_vect target_vel = phy_v_add(phy_body_get_velocity(info->target_body), phy_body_get_velocity(info->body));*/
    /*  phy_body_set_velocity(info->body, target_vel);*/
    /*}*/
  }
  return 0;
}
