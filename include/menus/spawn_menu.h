#pragma once

#include "entity/comp_physics.h"
#include "entity/comp_selector.h"
#include "khg_phy/phy_types.h"
#include <stdbool.h>

typedef enum {
  SPAWN_SELECT_TOP, 
  SPAWN_SELECT_BODY 
} spawning_select_type;

typedef struct {
  phy_vect pos;
  phy_vect linked_pos;
  float ang;
  comp_selector *comp_selector;
  int spawner_tex_id;
  int current_spawn_top_index;
  int current_spawn_body_index;
} spawn_menu_info;

extern spawn_menu_info SPAWN_SETTINGS;

void spawn_menu_setup(void);
bool render_spawn_menu(void);

