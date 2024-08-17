#include "entity/comp_mover.h"
#include "controllers/input/mouse_controller.h"
#include "controllers/elements/element_controller.h"
#include "data_utl/thread_utl.h"
#include "entity/comp_physics.h"
#include "entity/ecs_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/vect.h"
#include "khg_utl/map.h"
#include "khg_utl/vector.h"
#include <stdio.h>

ecs_id MOVER_COMPONENT_SIGNATURE;
mover_info NO_MOVER = { 0 };
utl_vector *MOVER_INFO = NULL;

void comp_mover_register(comp_mover *cm, ecs_ecs *ecs) {
  cm->id = ecs_register_component(ecs, sizeof(comp_mover), NULL, NULL);
  MOVER_COMPONENT_SIGNATURE = cm->id; 
}

void sys_mover_register(sys_mover *sm, ecs_ecs *ecs) {
  sm->id = ecs_register_system(ecs, sys_mover_update, NULL, NULL, NULL);
  ecs_require_component(ecs, sm->id, MOVER_COMPONENT_SIGNATURE);
  ecs_require_component(ecs, sm->id, PHYSICS_COMPONENT_SIGNATURE);
  sm->ecs = *ecs;
  MOVER_INFO = utl_vector_create(sizeof(mover_info));
  for (int i = 0; i < ECS->entity_count; i++) {
    utl_vector_push_back(MOVER_INFO, &NO_MOVER);
  }
}

void sys_mover_add(ecs_ecs *ecs, ecs_id *eid, mover_info *info) {
  ecs_add(ecs, *eid, MOVER_COMPONENT_SIGNATURE, NULL);
  utl_vector_assign(MOVER_INFO, *eid, info);
}

void sys_mover_free(bool need_free) {
  if (need_free) {
    utl_vector_deallocate(MOVER_INFO);
  }
}

void *update_mover_entities(void *arg) {
  thread_data *data = (thread_data *)arg;
  mover_info *info;
  physics_info *p_info;
  for (int id = data->start; id < data->end; id++) {
    info = utl_vector_at(MOVER_INFO, data->entities[id]);
    p_info = utl_map_at(PHYSICS_INFO_MAP, &data->entities[id]);
    if (!cpveql(handle_left_mouse_controls(), cpv(-1.0f, -1.0f))) {
      info->target_move_pos = handle_left_mouse_controls();
    }
    element_target_position(p_info, info->target_move_pos, 300.0f, 16.0f);
  }
  return NULL;
}

ecs_ret sys_mover_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  run_thread_update(entities, entity_count, update_mover_entities);
  return 0;
}

