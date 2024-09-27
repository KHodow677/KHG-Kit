#include "entity/comp_targeter.h"
#include "data_utl/kinematic_utl.h"
#include "entity/comp_health.h"
#include "entity/comp_physics.h"
#include "entity/comp_rotator.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/arbiter.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/shape.h"
#include "khg_utl/vector.h"
#include <stdio.h>
#include <stdlib.h>

ecs_id TARGETER_COMPONENT_SIGNATURE;
targeter_info NO_TARGETER = { 0 };
targeter_info *TARGETER_INFO = (targeter_info[ECS_ENTITY_COUNT]){};

static ecs_ret sys_targeter_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    targeter_info *info = &TARGETER_INFO[entities[id]];
    rotator_info *r_info = &ROTATOR_INFO[entities[id]];
    for (int i = 0; i < utl_vector_size(info->all_list); i++) {
      target *tgt = utl_vector_at(info->all_list, i);
      if (tgt->is_targetable && !tgt->is_in_target_lists) {
        tgt->is_in_target_lists = true;
        handle_target_lists_add(info, tgt);
      }
      else if (!tgt->is_targetable && tgt->is_in_target_lists) {
        tgt->is_in_target_lists = true;
        handle_target_lists_remove(info, tgt);
      }
    }
    if (info->mode == TARGET_FIRST) {
      info->current_list = info->first_list;
    }
    else if (info->mode == TARGET_LAST) {
      info->current_list = info->last_list;
    }
    else if (info->mode == TARGET_STRONG) {
      info->current_list = info->strong_list;
    }
    else if (info->mode == TARGET_WEAK) {
      info->current_list = info->weak_list;
    }
    if (!utl_vector_is_empty(info->current_list)) {
      ecs_id id = ((target *)utl_vector_front(info->current_list))->eid;
      r_info->target_aim_body = PHYSICS_INFO[id].body;
    }
    else {
      r_info->target_aim_body = NULL;
    }
  }
  return 0;
}

bool targeter_sensor_enter(phy_arbiter *arb, phy_space *space, phy_data_pointer udata) {
  phy_shape *sensor, *entity;
  phy_arbiter_get_shapes(arb, &sensor, &entity);
  physics_info *sensor_p_info = phy_shape_get_user_data(sensor);
  physics_info *entity_p_info = phy_shape_get_user_data(entity);
  targeter_info *target_selector = &TARGETER_INFO[sensor_p_info->eid];
  target new_target = (target){ entity_p_info->eid, true, false };
  utl_vector_push_back(target_selector->all_list, &new_target);
  return true;
}

void targeter_sensor_exit(phy_arbiter *arb, phy_space *space, phy_data_pointer udata) {
  phy_shape *sensor, *entity;
  phy_arbiter_get_shapes(arb, &sensor, &entity);
  physics_info *sensor_p_info = phy_shape_get_user_data(sensor);
  physics_info *entity_p_info = phy_shape_get_user_data(entity);
  targeter_info *target_selector = &TARGETER_INFO[sensor_p_info->eid];
  for (int i = 0; i < utl_vector_size(target_selector->all_list); i++) {
    target *tgt = utl_vector_at(target_selector->all_list, i);
    if (entity_p_info->eid == tgt->eid) {
      handle_target_lists_remove(target_selector, tgt);
      utl_vector_erase(target_selector->all_list, i, 1);
      break;
    }
  }
}

void handle_target_lists_add(targeter_info *info, target *tgt) {
  utl_vector_push_back(info->first_list, tgt);
  utl_vector_insert(info->last_list, 0, tgt);
  if (!utl_vector_is_empty(info->strong_list)) {
    health_info *new_h_info = &HEALTH_INFO[tgt->eid];
    for (int i = 0; i < utl_vector_size(info->strong_list); i++) {
      health_info *index_h_info = &HEALTH_INFO[((target *)utl_vector_at(info->strong_list, i))->eid];
      if (new_h_info->max_health > index_h_info->max_health) {
        utl_vector_insert(info->strong_list, i, tgt);
        break;
      }
    }
  }
  else {
    utl_vector_push_back(info->strong_list, tgt);
  }
  if (!utl_vector_is_empty(info->weak_list)) {
    health_info *new_h_info = &HEALTH_INFO[tgt->eid];
    for (int i = 0; i < utl_vector_size(info->weak_list); i++) {
      health_info *index_h_info = &HEALTH_INFO[((target *)utl_vector_at(info->weak_list, i))->eid];
      if (new_h_info->max_health < index_h_info->max_health) {
        utl_vector_insert(info->weak_list, i, tgt);
        break;
      }
    }
  }
  else {
    utl_vector_push_back(info->weak_list, tgt);
  }
}

void handle_target_lists_remove(targeter_info *info, target *tgt) {
  for (int i = 0; i < utl_vector_size(info->first_list); i++) {
    if (tgt->eid == ((target *)utl_vector_at(info->first_list, i))->eid) {
      utl_vector_erase(info->first_list, i, 1);
      break;
    }
  }
  for (int i = 0; i < utl_vector_size(info->last_list); i++) {
    if (tgt->eid == ((target *)utl_vector_at(info->last_list, i))->eid) {
      utl_vector_erase(info->last_list, i, 1);
      break;
    }
  }
  for (int i = 0; i < utl_vector_size(info->strong_list); i++) {
    if (tgt->eid == ((target *)utl_vector_at(info->strong_list, i))->eid) {
      utl_vector_erase(info->strong_list, i, 1);
      break;
    }
  }
  for (int i = 0; i < utl_vector_size(info->weak_list); i++) {
    if (tgt->eid == ((target *)utl_vector_at(info->weak_list, i))->eid) {
      utl_vector_erase(info->weak_list, i, 1);
      break;
    }
  }
}

void comp_targeter_register(comp_targeter *ct) {
  ct->id = ecs_register_component(ECS, sizeof(comp_targeter), NULL, NULL);
  TARGETER_COMPONENT_SIGNATURE = ct->id; 
}

void sys_targeter_register(sys_targeter *st) {
  st->id = ecs_register_system(ECS, sys_targeter_update, NULL, NULL, NULL);
  ecs_require_component(ECS, st->id, TARGETER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, st->id, ROTATOR_COMPONENT_SIGNATURE);
  st->ecs = *ECS;
  for (int i = 0; i < ECS_ENTITY_COUNT; i++) {
    TARGETER_INFO[i] = NO_TARGETER;
  }
}

void sys_targeter_add(ecs_id *eid, targeter_info *info) {
  ecs_add(ECS, *eid, TARGETER_COMPONENT_SIGNATURE, NULL);
  TARGETER_INFO[*eid] = *info;
}

