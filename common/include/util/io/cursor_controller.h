#pragma once

#include "khg_kin/namespace.h"
#include <stdbool.h>

typedef struct cursor_state cursor_state;
extern struct cursor_state {
  kin_vec screen_pos;
  kin_vec world_pos;
} CURSOR_STATE;

const bool cursor_button_is_down(unsigned int key);
const bool cursor_button_went_down(unsigned int key);
const bool cursor_button_went_up(unsigned int key);

void update_cursor_controls(void);

