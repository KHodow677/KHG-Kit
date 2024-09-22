#include "entity/comp_rotator.h"
#include "controllers/input/mouse_controller.h"
#include "controllers/elements/element_controller.h"
#include "entity/comp_physics.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/vect.h"
#include <stdio.h>

ecs_id ROTATOR_COMPONENT_SIGNATURE;
rotator_info NO_ROTATOR = { 0 };
utl_vector *ROTATOR_INFO = NULL;

void comp_rotator_register(comp_rotator *cr) {
  cr->id = ecs_register_component(ECS, sizeof(comp_rotator), NULL, NULL);
  ROTATOR_COMPONENT_SIGNATURE = cr->id; 
}

void sys_rotator_register(sys_rotator *sr) {
  sr->id = ecs_register_system(ECS, sys_rotator_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sr->id, ROTATOR_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, sr->id, PHYSICS_COMPONENT_SIGNATURE);
  sr->ecs = *ECS;
  ROTATOR_INFO = utl_vector_create(sizeof(selector_info));
  for (int i = 0; i < ECS->entity_count; i++) {
    utl_vector_push_back(ROTATOR_INFO, &NO_ROTATOR);
  }
}

void sys_rotator_add(ecs_id *eid, rotator_info *info) {
  ecs_add(ECS, *eid, ROTATOR_COMPONENT_SIGNATURE, NULL);
  utl_vector_assign(ROTATOR_INFO, *eid, info);
}

ecs_ret sys_rotator_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    rotator_info *info = utl_vector_at(ROTATOR_INFO, entities[id]);
    physics_info *p_info = utl_vector_at(PHYSICS_INFO, entities[id]);
    if (!phy_v_eql(MOUSE_STATE.right_mouse_click_controls, phy_v(-1.0f, -1.0f))) {
      info->target_look_pos = MOUSE_STATE.right_mouse_click_controls;
      p_info->is_locked_on = false;
    }
    element_lock_on_position(p_info, info->target_look_pos, 16.0f);
  }
  return 0;
}

