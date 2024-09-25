#include "entity/comp_mover.h"
#include "controllers/elements/element_controller.h"
#include "data_utl/kinematic_utl.h"
#include "entity/comp_physics.h"
#include "entity/comp_selector.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"
#include "khg_utl/queue.h"
#include "khg_utl/vector.h"
#include <stdlib.h>

ecs_id MOVER_COMPONENT_SIGNATURE;
mover_info NO_MOVER = { 0 };
utl_vector *MOVER_INFO = NULL;

static ecs_ret sys_mover_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    mover_info *info = utl_vector_at(MOVER_INFO, entities[id]);
    physics_info *body_info = utl_vector_at(PHYSICS_INFO, info->body_entity);
    if (utl_queue_empty(info->target_pos_queue)) {
      continue;
    }
    element_target_position(body_info, *(phy_vect *)utl_queue_front(info->target_pos_queue), 300.0f, 16.0f);
    if (element_is_at_position_default(body_info, *(phy_vect *)utl_queue_front(info->target_pos_queue))) {
      utl_queue_pop(info->target_pos_queue);
    }
  }
  return 0;
}

void comp_mover_register(comp_mover *cm) {
  cm->id = ecs_register_component(ECS, sizeof(comp_mover), NULL, NULL);
  MOVER_COMPONENT_SIGNATURE = cm->id; 
}

void sys_mover_register(sys_mover *sm) {
  sm->id = ecs_register_system(ECS, sys_mover_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sm->id, MOVER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, sm->id, SELECTOR_COMPONENT_SIGNATURE);
  sm->ecs = *ECS;
  MOVER_INFO = utl_vector_create(sizeof(mover_info));
  for (int i = 0; i < ECS->entity_count; i++) {
    utl_vector_push_back(MOVER_INFO, &NO_MOVER);
  }
}

void sys_mover_add(ecs_id *eid, mover_info *info) {
  ecs_add(ECS, *eid, MOVER_COMPONENT_SIGNATURE, NULL);
  utl_vector_assign(MOVER_INFO, *eid, info);
}

