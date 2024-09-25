#include "entity/comp_commander.h"
#include "data_utl/kinematic_utl.h"
#include "entity/indicators.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/vect.h"
#include "khg_utl/queue.h"
#include "khg_utl/vector.h"
#include <stdio.h>

ecs_id COMMANDER_COMPONENT_SIGNATURE;
commander_info NO_COMMANDER = { 0 };
utl_vector *COMMANDER_INFO = NULL;

static ecs_ret sys_commander_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    commander_info *info = utl_vector_at(COMMANDER_INFO, entities[id]);
    mover_info *m_info = utl_vector_at(MOVER_INFO, entities[id]);
    physics_info *body_info = utl_vector_at(PHYSICS_INFO, m_info->body_entity);
    selector_info *s_info = utl_vector_at(SELECTOR_INFO, entities[id]);
    physics_info *p_info = utl_vector_at(PHYSICS_INFO, entities[id]);
    renderer_info *r_info = utl_vector_at(RENDERER_INFO, entities[id]);
    if (s_info->selected && s_info->just_selected) {
      generate_all_indicators(s_info, p_info, r_info, m_info);
    }
    else if (s_info->selected && !s_info->just_selected) {
      if (!phy_v_eql(MOUSE_STATE.left_mouse_click_controls, phy_v(-1.0f, -1.0f))) {
        phy_vect new_pos = phy_v(MOUSE_STATE.left_mouse_click_controls.x, MOUSE_STATE.left_mouse_click_controls.y);
        utl_queue_push(m_info->target_pos_queue, &new_pos);
      }
      if (!phy_v_eql(MOUSE_STATE.right_mouse_click_controls, phy_v(-1.0f, -1.0f))) {
        if (!utl_queue_empty(m_info->target_pos_queue)) {
          while (!utl_queue_empty(m_info->target_pos_queue)) {
            utl_queue_pop(m_info->target_pos_queue);
          }
          float body_angle = normalize_angle(phy_body_get_angle(body_info->body));
          bool is_moving = phy_v_length(phy_body_get_velocity(body_info->body)) > 5.0f;
          float offset_dist = is_moving ? 50.0f : 0.0f;
          phy_vect new_pos = phy_v_add(phy_v(offset_dist * sinf(body_angle), offset_dist * -cosf(body_angle)), phy_body_get_position(body_info->body));
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
  COMMANDER_INFO = utl_vector_create(sizeof(commander_info));
  for (int i = 0; i < ECS->entity_count; i++) {
    utl_vector_push_back(COMMANDER_INFO, &NO_COMMANDER);
  }
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

void sys_commander_add(ecs_id *eid, commander_info *info) {
  ecs_add(ECS, *eid, COMMANDER_COMPONENT_SIGNATURE, NULL);
  utl_vector_assign(COMMANDER_INFO, *eid, info);
}

