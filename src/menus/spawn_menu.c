#include "menus/spawn_menu.h"
#include "entity/entity.h"
#include "game_manager.h"
#include "generators/components/texture_generator.h"
#include "menus/game_menu_manager.h"
#include "menus/menu_utl.h"
#include "spawners/spawn_berserker.h"
#include "spawners/spawn_berserker_clone.h"
#include "khg_gfx/elements.h"
#include <math.h>

spawn_selection_item *SPAWN_SELECTIONS = (spawn_selection_item[]) {(spawn_selection_item){ BERSERKER_ICON_SMALL, 100 }, (spawn_selection_item){ BERSERKER_ICON_SMALL, 100 }, (spawn_selection_item){ BERSERKER_ICON_SMALL, 100 }};
int MAX_SELECTIONS = 3;

void spawn_menu_setup() {
  GAME_OVERLAY_TRACKER[SPAWN_MENU].width = 400.0f;
  GAME_OVERLAY_TRACKER[SPAWN_MENU].height = 475.0f;
  GAME_OVERLAY_TRACKER[SPAWN_MENU].padding = 20.0f;
  GAME_OVERLAY_TRACKER[SPAWN_MENU].pos_x = GAME_OVERLAY_TRACKER[SPAWN_MENU].padding; 
  GAME_OVERLAY_TRACKER[SPAWN_MENU].pos_y = gfx_get_display_height() - GAME_OVERLAY_TRACKER[SPAWN_MENU].height - GAME_OVERLAY_TRACKER[SPAWN_MENU].padding;
}

bool render_spawn_menu() {
  float pos_x = GAME_OVERLAY_TRACKER[SPAWN_MENU].pos_x;
  float pos_y = GAME_OVERLAY_TRACKER[SPAWN_MENU].pos_y;
  float width = GAME_OVERLAY_TRACKER[SPAWN_MENU].width;
  float height = GAME_OVERLAY_TRACKER[SPAWN_MENU].height;
  render_div(pos_x, pos_y, width, height, 20.0f);
  render_header_text(width, height, "Hangar", 20.0f);
  render_big_icon(width, height, HANGAR_ICON_BIG, 70.0f);
  render_body_text(width, height, "Select Tank", 220.0f);
  render_small_icon_buttons(width, height, 250.0f, 20.0f);
  /*gfx_element_props props = gfx_get_theme().slider_props;*/
  /*props.margin_top = 0;*/
  /*props.margin_left = 0;*/
  /*props.margin_right = 0;*/
  /*props.border_width = 4.0f;*/
  /*props.color = gfx_green;*/
  /*props.text_color = gfx_white;*/
  /*props.border_color = gfx_black;*/
  /*gfx_push_style_props(props);*/
  /*gfx_set_ptr_x((width - 200) / 2.0f);*/
  /*gfx_set_ptr_y(400);*/
  /*gfx_clickable_item_state progress = gfx_progress_bar_int(TEST_VAL, 0, 500, 200, 50);*/
  /*gfx_pop_style_props();*/
  if (!render_button(width, height, "SPAWN", 400.0f)) {
    if (SPAWN_SELECTIONS[SPAWN_SETTINGS.current_spawn_index].icon_tex_id == BERSERKER_ICON_SMALL) {
      generic_entity *bkr = spawn_berserker(SPAWN_SETTINGS.pos.x, SPAWN_SETTINGS.pos.y, SPAWN_SETTINGS.ang);
      spawn_berserker_clone(bkr->berserker.top.comp_physics, SPAWN_SETTINGS.linked_pos.x, SPAWN_SETTINGS.linked_pos.y, SPAWN_SETTINGS.ang + M_PI);
      SPAWN_SETTINGS.comp_selector->should_deselect = true;
      GAME_INFO.cash -= SPAWN_SELECTIONS[SPAWN_SETTINGS.current_spawn_index].cost;
    }
  }
  gfx_div_end();
  gfx_clear_style_props();
  return true;
}

