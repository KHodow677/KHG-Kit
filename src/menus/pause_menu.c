#include "menus/pause_menu.h"
#include "game_manager.h"
#include "menus/game_menu_manager.h"
#include "menus/menu_utl.h"
#include "scenes/scene_utl.h"
#include <stdio.h>

bool handle_pause() {
  bool ret = PAUSED ? !KEYBOARD_STATE.escape_key_went_down : KEYBOARD_STATE.escape_key_went_down;
  if (ret) {
    close_all_game_overlays();
    GAME_OVERLAY_TRACKER[PAUSE_MENU].active = true;
  }
  else {
    GAME_OVERLAY_TRACKER[GAME_INFO_MENU].active = true;
    GAME_OVERLAY_TRACKER[PAUSE_MENU].active = false;
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
  if (GAME_INFO.lives > 0) {
    float width = GAME_OVERLAY_TRACKER[PAUSE_MENU].width;
    float height = GAME_OVERLAY_TRACKER[PAUSE_MENU].height;
    render_div((gfx_get_display_width() - width) / 2.0f, (gfx_get_display_height() - height) / 2.0f, width, height, 0.0f);
    render_text(LARGE_FONT, width / 2, 60.0f, "Game Paused");
    if (!render_button(width, height, "Resume", 140.0f)) {
      PAUSED = false;
    }
    if (!render_button(width, height, "Exit", 220.0f)) {
      stm_handle_event(&SCENE_FSM, &(stm_event){ EVENT_SCENE_SWITCH, (void *)(intptr_t)TO_TITLE_SCENE });
    }
    gfx_div_end();
    gfx_pop_style_props();
    return true;
  }
  else {
    float width = GAME_OVERLAY_TRACKER[PAUSE_MENU].width;
    float height = GAME_OVERLAY_TRACKER[PAUSE_MENU].height;
    render_div((gfx_get_display_width() - width) / 2.0f, (gfx_get_display_height() - height) / 2.0f, width, height, 0.0f);
    render_text(LARGE_FONT, width / 2, 60.0f, "Out of Lives");
    char time[20]; 
    int minutes = (int)(GAME_INFO.time / 60);
    int seconds = (int)(GAME_INFO.time) % 60;
    snprintf(time, sizeof(time), "Time: %02d:%02d", minutes, seconds);
    render_text(LARGE_FONT, width / 2, 140, time);
    if (!render_button(width, height, "Exit", 220.0f)) {
      stm_handle_event(&SCENE_FSM, &(stm_event){ EVENT_SCENE_SWITCH, (void *)(intptr_t)TO_TITLE_SCENE });
    }
    gfx_div_end();
    gfx_pop_style_props();
    return true;
  }
}

