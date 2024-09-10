#include "entity/comp_selector.h"
#include "controllers/input/mouse_controller.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "game_manager.h"
#include "generators/components/texture_generator.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/shape.h"
#include "khg_phy/vect.h"
#include "khg_utl/vector.h"
#include <stdio.h>

static void generate_outline(renderer_info *r_info, int tex, phy_vect pos, float ang, bool follow, bool is_target_body) {
  indicator ind = { OUTLINE, follow, is_target_body, tex, pos, ang };
  utl_vector_push_back(r_info->indicators, &ind);
}

static ecs_ret sys_selector_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  selector_info *info;
  physics_info *p_info;
  renderer_info *r_info;
  for (int id = 0; id < entity_count; id++) {
    info = utl_vector_at(SELECTOR_INFO, entities[id]);
    p_info = utl_vector_at(PHYSICS_INFO, entities[id]);
    r_info = utl_vector_at(RENDERER_INFO, entities[id]);
    info->just_selected = false;
    if (!phy_v_eql(MOUSE_STATE.left_mouse_click_controls, phy_v(-1.0f, -1.0f))) {
      if (!p_info->target_body || !p_info->target_shape) {
        continue;
      }
      if (phy_shape_point_query(p_info->target_shape, MOUSE_STATE.left_mouse_click_controls, NULL) < 0.0f) {
        if (!info->selected) {
          info->selected = true;
          info->just_selected = true;
          phy_vect pos = phy_body_get_position(p_info->target_body);
          float ang = phy_body_get_angle(p_info->target_body);
          generate_outline(r_info, TANK_BODY_OUTLINE, pos, ang, true, true);
          generate_outline(r_info, TANK_TOP_OUTLINE, pos, ang, true, false);
        }
        else {
          info->selected = false;
          utl_vector_clear(r_info->indicators);
        }
      }
    }
  }
  return 0;
}

ecs_id SELECTOR_COMPONENT_SIGNATURE;
selector_info NO_SELECTOR = { 0 };
utl_vector *SELECTOR_INFO = NULL;

void comp_selector_register(comp_selector *cs) {
  cs->id = ecs_register_component(ECS, sizeof(comp_selector), NULL, NULL);
  SELECTOR_COMPONENT_SIGNATURE = cs->id; 
}

void sys_selector_register(sys_selector *ss) {
  ss->id = ecs_register_system(ECS, sys_selector_update, NULL, NULL, NULL);
  ecs_require_component(ECS, ss->id, SELECTOR_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, ss->id, PHYSICS_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, ss->id, RENDERER_COMPONENT_SIGNATURE);
  ss->ecs = *ECS;
  SELECTOR_INFO = utl_vector_create(sizeof(selector_info));
  for (int i = 0; i < ECS->entity_count; i++) {
    utl_vector_push_back(SELECTOR_INFO, &NO_SELECTOR);
  }
}

void sys_selector_add(ecs_id *eid, selector_info *info) {
  ecs_add(ECS, *eid, SELECTOR_COMPONENT_SIGNATURE, NULL);
  utl_vector_assign(SELECTOR_INFO, *eid, info);
}

