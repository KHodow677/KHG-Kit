#pragma once

#include "entity/comp_selector.h"
#include "khg_phy/phy_types.h"
#include <stdbool.h>

typedef struct {
  int icon_tex_id;
  int cost;
} spawn_selection_item;

typedef struct {
  phy_vect pos;
  phy_vect linked_pos;
  float ang;
  comp_selector *comp_selector;
  int spawner_tex_id;
  int current_spawn_index;
} spawn_menu_info;

extern spawn_selection_item *SPAWN_SELECTIONS;
extern int MAX_SELECTIONS;

void spawn_menu_setup(void);
bool render_spawn_menu(void);

