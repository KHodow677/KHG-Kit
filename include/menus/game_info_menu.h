#pragma once

#include <stdbool.h>

typedef struct game_info {
  int lives;
  int cash;
  float time;
} game_info;

void game_info_menu_setup(void);
bool render_game_info_menu(void);

