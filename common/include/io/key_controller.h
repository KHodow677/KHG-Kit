#pragma once

#include "GLFW/glfw3.h"
#include <stdbool.h>

extern bool KEYBOARD_STATE[GLFW_KEY_LAST];

const bool key_button_is_down(int key);
const bool key_button_went_down(int key);
const bool key_button_went_up(int key);

void update_key_controls();

