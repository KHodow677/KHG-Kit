#pragma once

#include "khg_ecs/ecs.h"
#include "khg_phy/phy_types.h"
#include <stdbool.h>

typedef enum {
  SPAWN_SELECT_TOP, 
  SPAWN_SELECT_BODY 
} spawning_select_type;

typedef struct {
  phy_body *spawn_body;
  int spawner_tex_id;
  int current_spawn_top_index;
  int current_spawn_body_index;
} spawn_menu_info;

extern spawn_menu_info SPAWN_SETTINGS;

void spawn_menu_setup(void);
bool render_spawn_menu(void);

