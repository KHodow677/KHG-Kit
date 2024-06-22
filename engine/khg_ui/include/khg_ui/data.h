#pragma once

#include "khg_math/vec2.h"
#include <stdbool.h>

typedef struct {
  vec2 mouse_pos;
  bool mouse_click;
  bool mouse_held;
  bool mouse_released;
  bool escape_released;
} input_data;

typedef struct {
  bool slider_being_dragged;
  bool slider_being_dragged_2;
  bool slider_being_dragged_3;
} persistent_data;

typedef struct {
  vec2 widget_size;
} aligned_settings;

