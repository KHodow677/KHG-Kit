#include "menus/game_menu_manager.h"
#include "game_manager.h"
#include "menus/spawn_menu.h"

bool mangage_game_overlays() {
  bool res = true;
  if (GAME_OVERLAY_TRACKER[SPAWN_MENU] == true) {
    res = res && render_spawn_menu();
  }
  return res;
}

