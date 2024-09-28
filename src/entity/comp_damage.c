#include "entity/comp_damage.h"
#include "entity/comp_shooter.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include <stdio.h>
#include <stdlib.h>

ecs_id DAMAGE_COMPONENT_SIGNATURE;

static ecs_ret sys_damage_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    comp_damage *info = ecs_get(ECS, entities[id], DAMAGE_COMPONENT_SIGNATURE);
    comp_shooter *s_info = ecs_get(ECS, entities[id], SHOOTER_COMPONENT_SIGNATURE);
    info->killed = false;
    if (info->target_health == NULL && !s_info->shot) {
      continue;
    }
    /*if (s_info->shot) {*/
    /*  info->target_health->current_health -= info->damage;*/
    /*  info->killed = info->target_health->current_health <= 0.0f;*/
    /*}*/
  }
  return 0;
}

void comp_damage_register(comp_damage *cd) {
  cd->id = ecs_register_component(ECS, sizeof(comp_damage), NULL, NULL);
  DAMAGE_COMPONENT_SIGNATURE = cd->id; 
}

void sys_damage_register(sys_damage *sd) {
  sd->id = ecs_register_system(ECS, sys_damage_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sd->id, DAMAGE_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, sd->id, SHOOTER_COMPONENT_SIGNATURE);
  sd->ecs = *ECS;
}

comp_damage *sys_damage_add(ecs_id eid) {
  return ecs_add(ECS, eid, DAMAGE_COMPONENT_SIGNATURE, NULL);
}

