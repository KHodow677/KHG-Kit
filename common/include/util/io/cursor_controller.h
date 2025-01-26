#pragma once

#include "khg_phy/core/phy_vector.h"
#include <stdbool.h>

typedef struct cursor_state cursor_state;
extern struct cursor_state {
  phy_vector2 screen_pos;
  phy_vector2 world_pos;
} CURSOR_STATE;

const bool cursor_button_is_down(unsigned int key);
const bool cursor_button_went_down(unsigned int key);
const bool cursor_button_went_up(unsigned int key);

void update_cursor_controls(void);
