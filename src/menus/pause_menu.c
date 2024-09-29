#include "menus/pause_menu.h"
#include "game_manager.h"
#include "menus/game_menu_manager.h"
#include "menus/menu_utl.h"
#include "scenes/scene_utl.h"

bool handle_pause() {
  bool ret = PAUSED ? !KEYBOARD_STATE.escape_key_went_down : KEYBOARD_STATE.escape_key_went_down;
  if (ret) {
    close_all_game_overlays();
    GAME_OVERLAY_TRACKER[PAUSE_MENU].active = true;
  }
  else {
    GAME_OVERLAY_TRACKER[GAME_INFO_MENU].active = true;
  }
  return ret;
}

void pause_menu_setup() {
  GAME_OVERLAY_TRACKER[PAUSE_MENU].width = 400.0f;
  GAME_OVERLAY_TRACKER[PAUSE_MENU].height = 300.0f;
  GAME_OVERLAY_TRACKER[PAUSE_MENU].padding = 0.0f;
  GAME_OVERLAY_TRACKER[PAUSE_MENU].pos_x = (gfx_get_display_width() - GAME_OVERLAY_TRACKER[PAUSE_MENU].width) / 2.0f; 
  GAME_OVERLAY_TRACKER[PAUSE_MENU].pos_y = (gfx_get_display_height() - GAME_OVERLAY_TRACKER[PAUSE_MENU].height) / 2.0f;
}

bool render_pause_menu(void) {
  if (!GAME_OVERLAY_TRACKER[PAUSE_MENU].active){
    return true;
  }
  float width = GAME_OVERLAY_TRACKER[SPAWN_MENU].width;
  float height = GAME_OVERLAY_TRACKER[SPAWN_MENU].height;
  render_div((gfx_get_display_width() - width) / 2.0f, (gfx_get_display_height() - height) / 2.0f, width, height, 0.0f);
  render_text(LARGE_FONT, width / 2, 60.0f, "It Tanks Two!");
  if (!render_button(width, height, "RESUME", 140.0f)) {
    PAUSED = false;
  }
  if (!render_button(width, height, "Exit", 220.0f)) {
    stm_handle_event(&SCENE_FSM, &(stm_event){ EVENT_SCENE_SWITCH, (void *)(intptr_t)TO_TITLE_SCENE });
  }
  gfx_div_end();
  gfx_pop_style_props();
  return true;
  return true;
}

