#include "menus/game_menu_manager.h"
#include "game_manager.h"
#include "menus/spawn_menu.h"

void setup_game_overlay() {
  for (int i = 0; i < NUM_MENUS; i++) {
    GAME_OVERLAY_TRACKER[i] = (game_overlay_info) { 0 };
    GAME_OVERLAY_TRACKER[i].active = false;
  }
  update_game_overlay();
}

void update_game_overlay() {
  spawn_menu_setup();
}

bool mangage_game_overlays() {
  bool res = true;
  if (GAME_OVERLAY_TRACKER[SPAWN_MENU].active) {
    res = res && render_spawn_menu();
  }
  return res;
}

bool point_on_overlay(float x, float y) {
  update_game_overlay();
  for (int i = 0; i < NUM_MENUS; i++) {
    if (!GAME_OVERLAY_TRACKER[i].active) {
      continue;
    }
    float pos_x = GAME_OVERLAY_TRACKER[i].pos_x;
    float pos_y = GAME_OVERLAY_TRACKER[i].pos_y;
    float width = GAME_OVERLAY_TRACKER[i].width;
    float height = GAME_OVERLAY_TRACKER[i].height;
    if (x > pos_x && y > pos_y && x < pos_x + width && y < pos_y + height) {
      return true;
    }
  }
  return false;
}

