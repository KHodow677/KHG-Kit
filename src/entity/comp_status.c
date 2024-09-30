#include "entity/comp_status.h"
#include "data_utl/camera_utl.h"
#include "entity/comp_physics.h"
#include "game_manager.h"
#include "khg_ecs/ecs.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/ui.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include <stdio.h>

ecs_id STATUS_COMPONENT_SIGNATURE;

static ecs_ret sys_status_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  for (int id = 0; id < entity_count; id++) {
    comp_status *info = ecs_get(ECS, entities[id], STATUS_COMPONENT_SIGNATURE);
    comp_physics *p_info = ecs_get(ECS, entities[id], PHYSICS_COMPONENT_SIGNATURE);
    phy_vect pos = phy_v_add(phy_body_get_position(p_info->body), phy_v(0, -64));
    phy_vect screen_pos = world_to_screen(pos.x, pos.y);
    phy_vect bar_size = phy_v(info->bar_width * CAMERA.zoom, info->bar_height * CAMERA.zoom);
    int bar_count = 0;
    if (screen_pos.x > 0.0f + bar_size.x / 2.0f && screen_pos.x < gfx_get_display_width() - bar_size.x / 2.0f) {
      if (info->tracks_health) {
        comp_health *h_info = ecs_get(ECS, entities[id], HEALTH_COMPONENT_SIGNATURE);
        gfx_element_props props = gfx_get_theme().slider_props;
        props.margin_top = 0;
        props.margin_left = 0;
        props.margin_right = 0;
        props.border_width = 5.0f * CAMERA.zoom;
        props.color = gfx_no_color;
        props.text_color = gfx_red;
        props.border_color = gfx_black;
        gfx_push_style_props(props);
        gfx_set_ptr_x(screen_pos.x - bar_size.x / 2.0f);
        gfx_set_ptr_y(screen_pos.y - bar_size.y / 2.0f);
        gfx_clickable_item_state progress = gfx_progress_bar_int(h_info->current_health, 0, h_info->max_health, bar_size.x, bar_size.y);
        gfx_clear_style_props();
        bar_count++;
      }
    }
  }
  return 0;
}

void comp_status_register() {
  STATUS_COMPONENT_SIGNATURE = ecs_register_component(ECS, sizeof(comp_status), NULL, NULL);
}

void sys_status_register(sys_status *ss) {
  ss->id = ecs_register_system(ECS, sys_status_update, NULL, NULL, NULL);
  ecs_require_component(ECS, ss->id, STATUS_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, ss->id, PHYSICS_COMPONENT_SIGNATURE);
  ecs_require_component(ECS, ss->id, RENDERER_COMPONENT_SIGNATURE);
  ss->ecs = *ECS;
}

comp_status *sys_status_add(ecs_id eid) {
  return ecs_add(ECS, eid, STATUS_COMPONENT_SIGNATURE, NULL);
}

