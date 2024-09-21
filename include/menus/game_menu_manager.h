#pragma once

#include <stdbool.h>

typedef enum {
  SPAWN_MENU,
  NUM_MENUS
} game_overlay_types;

bool mangage_game_overlays();

