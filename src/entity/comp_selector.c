#include "entity/comp_selector.h"
#include "controllers/input/mouse_controller.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_phy/shape.h"
#include "khg_phy/vect.h"
#include "khg_utl/vector.h"

ecs_id SELECTOR_COMPONENT_SIGNATURE;
selector_info NO_SELECTOR = { 0 };
utl_vector *SELECTOR_INFO = NULL;

static void swap_render_info_texture(renderer_info *r_info, ecs_id current_id, int tex_id, int linked_tex_id) {
  r_info->tex_id = tex_id;
  if (r_info->linked_ent != current_id) {
    renderer_info *linked_r_info = utl_vector_at(RENDERER_INFO, r_info->linked_ent);
    linked_r_info->tex_id = linked_tex_id;
  }
}

static void deselect(selector_info *info, renderer_info *r_info, ecs_id id) {
  info->selected = false;
  utl_vector_clear(r_info->indicators);
  swap_render_info_texture(r_info, id, info->tex_id, info->linked_tex_id);
}

static ecs_ret sys_selector_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    selector_info *info = utl_vector_at(SELECTOR_INFO, entities[id]);
    comp_physics *p_info = ecs_get(ECS, entities[id], PHYSICS_COMPONENT_SIGNATURE);
    renderer_info *r_info = utl_vector_at(RENDERER_INFO, entities[id]);
    info->just_selected = false;
    if (KEYBOARD_STATE.escape_key_went_down) {
      deselect(info, r_info, entities[id]);
    }
    if (!phy_v_eql(MOUSE_STATE.left_mouse_click_controls, phy_v(-1.0f, -1.0f))) {
      if (phy_shape_point_query(p_info->target_shape, MOUSE_STATE.left_mouse_click_controls, NULL) < 0.0f) {
        if (!info->selected) {
          for (int i = 0; i < entity_count; i++) {
            selector_info *info_s = utl_vector_at(SELECTOR_INFO, entities[i]);
            renderer_info *info_r = utl_vector_at(RENDERER_INFO, entities[i]);
            if (!info_s->selected) {
              continue;
            }
            deselect(info_s, info_r, entities[i]);
          }
          info->selected = true;
          info->just_selected = true;
          if (info->selected_tex_id == info->selected_linked_tex_id) {
            swap_render_info_texture(r_info, entities[id], info->selected_tex_id, info->selected_linked_tex_id);
          }
          else {
            swap_render_info_texture(r_info, entities[id], info->tex_id, info->selected_linked_tex_id);
          }
        }
      }
      else {
        deselect(info, r_info, entities[id]);
      }
    }
  }
  return 0;
}

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

