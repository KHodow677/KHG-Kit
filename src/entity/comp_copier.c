#include "entity/comp_copier.h"
#include "data_utl/kinematic_utl.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_utl/queue.h"
#include <stdio.h>
#include <stdlib.h>

ecs_id COPIER_COMPONENT_SIGNATURE;

static ecs_ret sys_copier_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    comp_copier *info = ecs_get(ECS, entities[id], COPIER_COMPONENT_SIGNATURE);
    printf("%zu\n", utl_queue_size(info->ext_mover_ref->target_pos_queue));
  }
  return 0;
}

void comp_copier_register() {
  COPIER_COMPONENT_SIGNATURE = ecs_register_component(ECS, sizeof(comp_copier), NULL, NULL);
}

void sys_copier_register(sys_copier *sm) {
  sm->id = ecs_register_system(ECS, sys_copier_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sm->id, COPIER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, sm->id, MOVER_COMPONENT_SIGNATURE);
  sm->ecs = *ECS;
}

comp_copier *sys_copier_add(ecs_id eid) {
  return ecs_add(ECS, eid, COPIER_COMPONENT_SIGNATURE, NULL);
}

