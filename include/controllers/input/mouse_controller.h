#pragma once

#include "khg_phy/phy_types.h"

typedef struct mouse_state {
  phy_vect left_mouse_click_controls;
  phy_vect right_mouse_click_controls;
} mouse_state;

void update_mouse_controls(mouse_state *ms);
