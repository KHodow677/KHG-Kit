#pragma once

#include <stdbool.h>

typedef struct keyboard_state {
  bool w_key_is_down;
  bool w_key_went_down;
  bool a_key_is_down;
  bool a_key_went_down;
  bool s_key_is_down;
  bool s_key_went_down;
  bool d_key_is_down;
  bool d_key_went_down;
  bool space_key_is_down;
  bool space_key_went_down;
  bool escape_key_is_down;
  bool escape_key_went_down;
} keyboard_state;

extern keyboard_state KEYBOARD_STATE;

bool handle_key_button_is_down(int key);
bool handle_key_button_went_down(int key);

void update_key_controls(keyboard_state *ks);

