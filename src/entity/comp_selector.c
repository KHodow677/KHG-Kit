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

static void swap_render_info_texture(comp_renderer *r_info, int tex_id, int linked_tex_id) {
  r_info->tex_id = tex_id;
  if (r_info->linked_r_info != r_info) {
    r_info->linked_r_info->tex_id = linked_tex_id;
  }
}

static void deselect(comp_selector *info, comp_renderer *r_info, ecs_id id) {
  info->selected = false;
  utl_vector_clear(r_info->indicators);
  swap_render_info_texture(r_info, info->tex_id, info->linked_tex_id);
}

static ecs_ret sys_selector_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    comp_selector *info = ecs_get(ECS, entities[id], SELECTOR_COMPONENT_SIGNATURE);
    comp_physics *p_info = ecs_get(ECS, entities[id], PHYSICS_COMPONENT_SIGNATURE);
    comp_renderer *r_info = ecs_get(ECS, entities[id], RENDERER_COMPONENT_SIGNATURE);
    info->just_selected = false;
    if (KEYBOARD_STATE.escape_key_went_down) {
      deselect(info, r_info, entities[id]);
    }
    if (!phy_v_eql(MOUSE_STATE.left_mouse_click_controls, phy_v(-1.0f, -1.0f))) {
      if (phy_shape_point_query(p_info->target_shape, MOUSE_STATE.left_mouse_click_controls, NULL) < 0.0f) {
        if (!info->selected) {
          for (int i = 0; i < entity_count; i++) {
            comp_selector *info_s = ecs_get(ECS, entities[id], SELECTOR_COMPONENT_SIGNATURE);
            comp_renderer *info_r = ecs_get(ECS, entities[id], RENDERER_COMPONENT_SIGNATURE);
            if (!info_s->selected) {
              continue;
            }
            deselect(info_s, info_r, entities[i]);
          }
          info->selected = true;
          info->just_selected = true;
          if (info->selected_tex_id == info->selected_linked_tex_id) {
            swap_render_info_texture(r_info, info->selected_tex_id, info->selected_linked_tex_id);
          }
          else {
            swap_render_info_texture(r_info, info->tex_id, info->selected_linked_tex_id);
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

void comp_selector_register() {
  SELECTOR_COMPONENT_SIGNATURE = ecs_register_component(ECS, sizeof(comp_selector), NULL, NULL);
}

void sys_selector_register(sys_selector *ss) {
  ss->id = ecs_register_system(ECS, sys_selector_update, NULL, NULL, NULL);
  ecs_require_component(ECS, ss->id, SELECTOR_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, ss->id, PHYSICS_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, ss->id, RENDERER_COMPONENT_SIGNATURE);
  ss->ecs = *ECS;
}

comp_selector *sys_selector_add(ecs_id eid) {
  return ecs_add(ECS, eid, SELECTOR_COMPONENT_SIGNATURE, NULL);
}

