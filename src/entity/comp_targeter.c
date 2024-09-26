#include "entity/comp_targeter.h"
#include "data_utl/kinematic_utl.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include <stdio.h>
#include <stdlib.h>

ecs_id TARGETER_COMPONENT_SIGNATURE;
targeter_info NO_TARGETER = { 0 };
targeter_info *TARGETER_INFO = (targeter_info[ECS_ENTITY_COUNT]){};

static ecs_ret sys_targeter_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    targeter_info *info = &TARGETER_INFO[entities[id]];
  }
  return 0;
}

void comp_targeter_register(comp_targeter *ct) {
  ct->id = ecs_register_component(ECS, sizeof(comp_targeter), NULL, NULL);
  TARGETER_COMPONENT_SIGNATURE = ct->id; 
}

void sys_targeter_register(sys_targeter *st) {
  st->id = ecs_register_system(ECS, sys_targeter_update, NULL, NULL, NULL);
  ecs_require_component(ECS, st->id, TARGETER_COMPONENT_SIGNATURE);
  st->ecs = *ECS;
  for (int i = 0; i < ECS_ENTITY_COUNT; i++) {
    TARGETER_INFO[i] = NO_TARGETER;
  }
}

void sys_targeter_add(ecs_id *eid, targeter_info *info) {
  ecs_add(ECS, *eid, SHOOTER_COMPONENT_SIGNATURE, NULL);
  TARGETER_INFO[*eid] = *info;
}

