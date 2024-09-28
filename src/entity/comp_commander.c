#include "entity/comp_commander.h"
#include "data_utl/kinematic_utl.h"
#include "entity/comp_mover.h"
#include "entity/comp_physics.h"
#include "entity/indicators.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/shape.h"
#include "khg_phy/vect.h"
#include "khg_utl/queue.h"
#include "khg_utl/vector.h"
#include <stdio.h>

ecs_id COMMANDER_COMPONENT_SIGNATURE;

static ecs_ret sys_commander_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    comp_commander *info = ecs_get(ECS, entities[id], COMMANDER_COMPONENT_SIGNATURE);
    comp_mover *m_info = ecs_get(ECS, entities[id], MOVER_COMPONENT_SIGNATURE);
    selector_info *s_info = utl_vector_at(SELECTOR_INFO, entities[id]);
    comp_physics *p_info = ecs_get(ECS, entities[id], PHYSICS_COMPONENT_SIGNATURE);
    renderer_info *r_info = utl_vector_at(RENDERER_INFO, entities[id]);
    if (s_info->selected && s_info->just_selected) {
      generate_all_indicators(s_info, p_info, r_info, m_info);
    }
    else if (s_info->selected && !s_info->just_selected) {
      if (!phy_v_eql(MOUSE_STATE.right_mouse_click_controls, phy_v(-1.0f, -1.0f))) {
        phy_vect new_pos = phy_v(MOUSE_STATE.right_mouse_click_controls.x, MOUSE_STATE.right_mouse_click_controls.y);
        utl_queue_push(m_info->target_pos_queue, &new_pos);
      }
      bool left_clicked_on = !phy_v_eql(MOUSE_STATE.left_mouse_click_controls, phy_v(-1.0f, -1.0f)) && phy_shape_point_query(p_info->target_shape, MOUSE_STATE.left_mouse_click_controls, NULL) < 0.0f;
      if (left_clicked_on) {
        if (!utl_queue_empty(m_info->target_pos_queue)) {
          while (!utl_queue_empty(m_info->target_pos_queue)) {
            utl_queue_pop(m_info->target_pos_queue);
          }
          phy_vect new_pos = phy_body_get_position(m_info->body_info->body);
          utl_queue_push(m_info->target_pos_queue, &new_pos);
        }
      }
    }
    int count = utl_queue_size(m_info->target_pos_queue);
    if (count != info->point_queue_count) {
      info->point_queue_count = count;
      generate_all_indicators(s_info, p_info, r_info, m_info);
    }
  }
  return 0;
}

void comp_commander_register(comp_commander *cc) {
  cc->id = ecs_register_component(ECS, sizeof(comp_commander), NULL, NULL);
  COMMANDER_COMPONENT_SIGNATURE = cc->id; 
}

void sys_commander_register(sys_commander *sc) {
  sc->id = ecs_register_system(ECS, sys_commander_update, NULL, NULL, NULL);
  ecs_require_component(ECS, sc->id, COMMANDER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, sc->id, MOVER_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, sc->id, SELECTOR_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, sc->id, PHYSICS_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, sc->id, RENDERER_COMPONENT_SIGNATURE);
  sc->ecs = *ECS;
}

comp_commander *sys_commander_add(ecs_id eid) {
  return ecs_add(ECS, eid, COMMANDER_COMPONENT_SIGNATURE, NULL);
}

