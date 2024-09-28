#include "entity/comp_damage.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_targeter.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_utl/vector.h"
#include <stdio.h>
#include <stdlib.h>

ecs_id DAMAGE_COMPONENT_SIGNATURE;
damage_info NO_DAMAGE = { 0 };
damage_info *DAMAGE_INFO = (damage_info[ECS_ENTITY_COUNT]){};

static ecs_ret sys_damage_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    damage_info *info = &DAMAGE_INFO[entities[id]];
    targeter_info *t_info = &TARGETER_INFO[entities[id]];
    if (utl_vector_is_empty(info->target_entities)) {
      continue;
    }
    /*for (int i = 0; i < utl_vector_size(info->target_entities); i++) {*/
    /*  target *hit_target = ((target *)utl_vector_at(info->target_entities, i));*/
    /*  hit_target->health_info->current_health -= info->damage;*/
    /*  if (hit_target->health_info->current_health <= 0.0f) {*/
    /*    hit_target->is_targetable = false;*/
    /*  }*/
    /*}*/
    utl_vector_clear(info->target_entities);
  }
  return 0;
}

void comp_damage_register(comp_damage *cd) {
  cd->id = ecs_register_component(ECS, sizeof(comp_destroyer), NULL, NULL);
  DAMAGE_COMPONENT_SIGNATURE = cd->id; 
}

void sys_damage_register(sys_damage *sd) {
  sd->id = ecs_register_system(ECS, sys_damage_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sd->id, DAMAGE_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, sd->id, TARGETER_COMPONENT_SIGNATURE);
  sd->ecs = *ECS;
  for (int i = 0; i < ECS_ENTITY_COUNT; i++) {
    DAMAGE_INFO[i] = NO_DAMAGE;
  }
}

void sys_damage_add(ecs_id *eid, damage_info *info) {
  ecs_add(ECS, *eid, DAMAGE_COMPONENT_SIGNATURE, NULL);
  DAMAGE_INFO[*eid] = *info;
}

