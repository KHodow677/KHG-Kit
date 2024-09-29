#pragma once

#include <stdbool.h>

typedef enum {
  SPAWN_MENU,
  NUM_MENUS
} game_overlay_type;

typedef struct {
  bool active;
  int pos_x;
  int pos_y;
  int width;
  int height;
  int padding;
} game_overlay_info;

void setup_game_overlay(void);
void close_all_game_overlays(void);
void update_game_overlay(void);
bool mangage_game_overlays(void);

bool point_on_overlay(float x, float y);

