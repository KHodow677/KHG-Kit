#include "entity/comp_rotator.h"
#include "controllers/input/mouse_controller.h"
#include "controllers/elements/element_controller.h"
#include "entity/comp_physics.h"
#include "data_utl/map_utl.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/vect.h"
#include "khg_utl/map.h"
#include <stdio.h>

ecs_id ROTATOR_COMPONENT_SIGNATURE;
utl_map *ROTATOR_INFO_MAP = NULL;

void comp_rotator_register(comp_rotator *cr) {
  cr->id = ecs_register_component(ECS, sizeof(comp_rotator), NULL, NULL);
  ROTATOR_COMPONENT_SIGNATURE = cr->id; 
}

void sys_rotator_register(sys_rotator *sr) {
  sr->id = ecs_register_system(ECS, sys_rotator_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sr->id, ROTATOR_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, sr->id, PHYSICS_COMPONENT_SIGNATURE);
  sr->ecs = *ECS;
  ROTATOR_INFO_MAP = utl_map_create(compare_ints, no_deallocator, no_deallocator);
}

void sys_rotator_add(ecs_id *eid, rotator_info *info) {
  ecs_add(ECS, *eid, ROTATOR_COMPONENT_SIGNATURE, NULL);
  utl_map_insert(ROTATOR_INFO_MAP, eid, info);
}

void sys_rotator_free(bool need_free) {
  if (need_free) {
    utl_map_deallocate(ROTATOR_INFO_MAP);
  }
}

ecs_ret sys_rotator_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  rotator_info *info;
  physics_info *p_info;
  for (int id = 0; id < entity_count; id++) {
    info = utl_map_at(ROTATOR_INFO_MAP, &entities[id]);
    p_info = utl_vector_at(PHYSICS_INFO, entities[id]);
    if (!phy_v_eql(handle_right_mouse_controls(), phy_v(-1.0f, -1.0f))) {
      info->target_look_pos = handle_right_mouse_controls();
      p_info->is_locked_on = false;
    }
    element_lock_on_position(p_info, info->target_look_pos, 16.0f);
  }
  return 0;
}

