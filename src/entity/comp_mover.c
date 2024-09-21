#include "entity/comp_mover.h"
#include "controllers/input/mouse_controller.h"
#include "controllers/elements/element_controller.h"
#include "data_utl/kinematic_utl.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_selector.h"
#include "entity/indicators.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include "khg_utl/queue.h"
#include "khg_utl/vector.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

ecs_id MOVER_COMPONENT_SIGNATURE;
mover_info NO_MOVER = { 0 };
utl_vector *MOVER_INFO = NULL;

static ecs_ret sys_mover_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  mover_info *info;
  physics_info *body_info;
  selector_info *s_info;
  physics_info *p_info;
  renderer_info *r_info;
  for (int id = 0; id < entity_count; id++) {
    info = utl_vector_at(MOVER_INFO, entities[id]);
    body_info = utl_vector_at(PHYSICS_INFO, info->body_entity);
    s_info = utl_vector_at(SELECTOR_INFO, entities[id]);
    p_info = utl_vector_at(PHYSICS_INFO, entities[id]);
    r_info = utl_vector_at(RENDERER_INFO, entities[id]);
    if (s_info->selected && !s_info->just_selected) {
      if (!phy_v_eql(MOUSE_STATE.left_mouse_click_controls, phy_v(-1.0f, -1.0f))) {
        phy_vect new_pos = phy_v(MOUSE_STATE.left_mouse_click_controls.x, MOUSE_STATE.left_mouse_click_controls.y);
        utl_queue_push(info->target_pos_queue, &new_pos);
        generate_all_indicators(s_info, p_info, r_info, info);
      }
      if (!phy_v_eql(MOUSE_STATE.right_mouse_click_controls, phy_v(-1.0f, -1.0f))) {
        if (!utl_queue_empty(info->target_pos_queue)) {
          while (!utl_queue_empty(info->target_pos_queue)) {
            utl_queue_pop(info->target_pos_queue);
          }
          float body_angle = normalize_angle(phy_body_get_angle(body_info->body));
          bool is_moving = phy_v_length(phy_body_get_velocity(body_info->body)) > 5.0f;
          float offset_dist = is_moving ? 50.0f : 0.0f;
          phy_vect new_pos = phy_v_add(phy_v(offset_dist * sinf(body_angle), offset_dist * -cosf(body_angle)), phy_body_get_position(body_info->body));
          utl_queue_push(info->target_pos_queue, &new_pos);
          generate_all_indicators(s_info, p_info, r_info, info);
        }
      }
    }
    if (utl_queue_empty(info->target_pos_queue)) {
      continue;
    }
    element_target_position(body_info, *(phy_vect *)utl_queue_front(info->target_pos_queue), 300.0f, 16.0f);
    if (element_is_at_position_default(body_info, *(phy_vect *)utl_queue_front(info->target_pos_queue))) {
      utl_queue_pop(info->target_pos_queue);
      if (s_info->selected && !s_info->just_selected) {
        generate_all_indicators(s_info, p_info, r_info, info);
      }
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
  ecs_require_component(ECS, sm->id, PHYSICS_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, sm->id, RENDERER_COMPONENT_SIGNATURE);
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

