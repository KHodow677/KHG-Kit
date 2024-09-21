#include "entity/comp_selector.h"
#include "controllers/input/mouse_controller.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/indicators.h"
#include "game_manager.h"
#include "generators/components/texture_generator.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/shape.h"
#include "khg_phy/vect.h"
#include "khg_utl/vector.h"
#include <stdio.h>

static void swap_render_info_texture(renderer_info *r_info, ecs_id current_id, int tex_id, int linked_tex_id) {
  r_info->tex_id = tex_id;
  if (r_info->linked_ent != current_id) {
    renderer_info *linked_r_info = utl_vector_at(RENDERER_INFO, r_info->linked_ent);
    linked_r_info->tex_id = linked_tex_id;
  }
}

static ecs_ret sys_selector_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  selector_info *info;
  physics_info *p_info;
  renderer_info *r_info;
  mover_info *m_info;
  for (int id = 0; id < entity_count; id++) {
    info = utl_vector_at(SELECTOR_INFO, entities[id]);
    p_info = utl_vector_at(PHYSICS_INFO, entities[id]);
    r_info = utl_vector_at(RENDERER_INFO, entities[id]);
    m_info = utl_vector_at(MOVER_INFO, entities[id]);
    info->just_selected = false;
    if (!phy_v_eql(MOUSE_STATE.left_mouse_click_controls, phy_v(-1.0f, -1.0f))) {
      if (!p_info->target_body || !p_info->target_shape) {
        continue;
      }
      if (phy_shape_point_query(p_info->target_shape, MOUSE_STATE.left_mouse_click_controls, NULL) < 0.0f) {
        if (!info->selected) {
          for (int i = 0; i < entity_count; i++) {
            selector_info *info_s = utl_vector_at(SELECTOR_INFO, entities[i]);
            renderer_info *info_r = utl_vector_at(RENDERER_INFO, entities[i]);
            if (!info_s->selected) {
              continue;
            }
            info_s->selected = false;
            utl_vector_clear(info_r->indicators);
          }
          info->selected = true;
          info->just_selected = true;
          generate_all_indicators(info, p_info, r_info, m_info);
          swap_render_info_texture(r_info, entities[id], TANK_TOP_OUTLINE, TANK_BODY_OUTLINE);
        }
        else {
          info->selected = false;
          utl_vector_clear(r_info->indicators);
          swap_render_info_texture(r_info, entities[id], TANK_TOP, TANK_BODY);
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
  ecs_require_component(ECS, ss->id, MOVER_COMPONENT_SIGNATURE);
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

