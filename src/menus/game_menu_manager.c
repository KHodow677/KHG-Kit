#include "menus/game_menu_manager.h"
#include "game_manager.h"
#include <stdio.h>

void mangage_game_overlays() {
  if (GAME_OVERLAY_TRACKER[SPAWN_MENU]) {
    printf("Spawn Menu\n");
  }
}

