#pragma once

#include <stdbool.h>

typedef struct keyboard_state {
  bool space_key_is_down;
  bool space_key_went_down;
} keyboard_state;

bool handle_key_button_is_down(int key);
bool handle_key_button_went_down(int key);

void update_key_controls(keyboard_state *ks);
