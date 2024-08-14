#include "entity/comp_mover.h"
#include "controllers/input/mouse_controller.h"
#include "controllers/elements/element_controller.h"
#include "entity/comp_physics.h"
#include "data_utl/map_utl.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/vect.h"
#include "khg_utl/map.h"
#include <stdio.h>

ecs_id MOVER_COMPONENT_SIGNATURE;
utl_map *MOVER_INFO_MAP = NULL;

void comp_mover_register(comp_mover *cm, ecs_ecs *ecs) {
  cm->id = ecs_register_component(ecs, sizeof(comp_mover), NULL, NULL);
  MOVER_COMPONENT_SIGNATURE = cm->id; 
}

void sys_mover_register(sys_mover *sm, ecs_ecs *ecs) {
  sm->id = ecs_register_system(ecs, sys_mover_update, NULL, NULL, NULL);
  ecs_require_component(ecs, sm->id, MOVER_COMPONENT_SIGNATURE);
  ecs_require_component(ecs, sm->id, PHYSICS_COMPONENT_SIGNATURE);
  sm->ecs = *ecs;
  MOVER_INFO_MAP = utl_map_create(compare_ints, no_deallocator, no_deallocator);
}

void sys_mover_add(ecs_ecs *ecs, ecs_id *eid, mover_info *info) {
  ecs_add(ecs, *eid, MOVER_COMPONENT_SIGNATURE, NULL);
  utl_map_insert(MOVER_INFO_MAP, eid, info);
}

void sys_mover_free(bool need_free) {
  if (need_free) {
    utl_map_deallocate(MOVER_INFO_MAP);
  }
}

ecs_ret sys_mover_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  (void)ecs;
  (void)dt;
  (void)udata;
  if (entity_count == 0) {
    return 0;
  }
  mover_info *info;
  physics_info *p_info;
  for (int id = 0; id < entity_count; id++) {
    info = utl_map_at(MOVER_INFO_MAP, &entities[id]);
    p_info = utl_map_at(PHYSICS_INFO_MAP, &entities[id]);
    if (!cpveql(handle_left_mouse_controls(), cpv(-1.0f, -1.0f))) {
      info->target_move_pos = handle_left_mouse_controls();
    }
    element_target_position(p_info, info->target_move_pos, 300.0f, 16.0f);
  }
  return 0;
}
