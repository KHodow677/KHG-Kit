#include "entity/comp_targeter.h"
#include "data_utl/kinematic_utl.h"
#include "entity/comp_damage.h"
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

static void add_by_first(targeter_info *info, comp_health *h_info) {
  utl_vector_push_back(info->all_list, &h_info);
}


static void add_by_last(targeter_info *info, comp_health *h_info) {
  utl_vector_insert(info->all_list, 0, &h_info);
}


static void add_by_strong(targeter_info *info, comp_health *h_info) {
  if (!utl_vector_is_empty(info->all_list)) {
    for (int i = 0; i < utl_vector_size(info->all_list); i++) {
      comp_health *index_h_info = *(comp_health **)utl_vector_at(info->all_list, i);
      if (h_info->max_health > index_h_info->max_health) {
        utl_vector_insert(info->all_list, i, &h_info);
        break;
      }
    }
  }
  else {
    utl_vector_push_back(info->all_list, &h_info);
  }
}

static void add_by_weak(targeter_info *info, comp_health *h_info) {
  if (!utl_vector_is_empty(info->all_list)) {
    for (int i = 0; i < utl_vector_size(info->all_list); i++) {
      comp_health *index_h_info = *(comp_health **)utl_vector_at(info->all_list, i);
      if (h_info->max_health < index_h_info->max_health) {
        utl_vector_insert(info->all_list, i, &h_info);
        break;
      }
    }
  }
  else {
    utl_vector_push_back(info->all_list, &h_info);
  }
}

static void remove_from(targeter_info *info, comp_health *h_info) {
  for (int i = 0; i < utl_vector_size(info->all_list); i++) {
    comp_health *index_h_info = *(comp_health **)utl_vector_at(info->all_list, i);
    if (h_info == index_h_info) {
      utl_vector_erase(info->all_list, i, 1);
      break;
    }
  }
}

static ecs_ret sys_targeter_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    targeter_info *info = &TARGETER_INFO[entities[id]];
    rotator_info *r_info = &ROTATOR_INFO[entities[id]];
    comp_damage *d_info = ecs_get(ECS, entities[id], DAMAGE_COMPONENT_SIGNATURE);
    if (d_info->killed) {
      remove_from(info, r_info->target_health);
    }
    if (!utl_vector_is_empty(info->all_list)) {
      r_info->target_health = *(comp_health **)utl_vector_front(info->all_list);
      d_info->target_health = *(comp_health **)utl_vector_front(info->all_list);
    }
    else {
      r_info->target_health = NULL;
      d_info->target_health = NULL;
    }
  }
  return 0;
}

bool targeter_sensor_enter(phy_arbiter *arb, phy_space *space, phy_data_pointer udata) {
  phy_shape *sensor, *entity;
  phy_arbiter_get_shapes(arb, &sensor, &entity);
  physics_info *sensor_p_info = phy_shape_get_user_data(sensor);
  physics_info *entity_p_info = phy_shape_get_user_data(entity);
  targeter_info *target_selector = sensor_p_info->targeter_ref;
  comp_health *target_health_info = entity_p_info->health_ref;
  utl_vector_push_back(target_selector->all_list, &target_health_info);
  return true;
}

void targeter_sensor_exit(phy_arbiter *arb, phy_space *space, phy_data_pointer udata) {
  phy_shape *sensor, *entity;
  phy_arbiter_get_shapes(arb, &sensor, &entity);
  physics_info *sensor_p_info = phy_shape_get_user_data(sensor);
  physics_info *entity_p_info = phy_shape_get_user_data(entity);
  targeter_info *target_selector = sensor_p_info->targeter_ref;
  comp_health *target_health_info = entity_p_info->health_ref;
  remove_from(target_selector, target_health_info);
}

void comp_targeter_register(comp_targeter *ct) {
  ct->id = ecs_register_component(ECS, sizeof(comp_targeter), NULL, NULL);
  TARGETER_COMPONENT_SIGNATURE = ct->id; 
}

void sys_targeter_register(sys_targeter *st) {
  st->id = ecs_register_system(ECS, sys_targeter_update, NULL, NULL, NULL);
  ecs_require_component(ECS, st->id, TARGETER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, st->id, ROTATOR_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, st->id, DAMAGE_COMPONENT_SIGNATURE);
  st->ecs = *ECS;
  for (int i = 0; i < ECS_ENTITY_COUNT; i++) {
    TARGETER_INFO[i] = NO_TARGETER;
  }
}

void sys_targeter_add(ecs_id *eid, targeter_info *info) {
  ecs_add(ECS, *eid, TARGETER_COMPONENT_SIGNATURE, NULL);
  TARGETER_INFO[*eid] = *info;
}

