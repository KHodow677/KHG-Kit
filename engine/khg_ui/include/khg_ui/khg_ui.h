#pragma once

#include "khg_2d/renderer_2d.h"
#include "khg_math/vec2.h"

struct renderer_ui {
  struct internal {
    struct input_data {
      vec2 mouse_pos;
      bool mouse_click;
      bool mouse_held;
      bool mouse_released;
      bool escape_released;
    };
    struct widget {
      string text2
    }
  };
  struct align_settings {
    bool slider_being_dragged;
    bool slider_being_dragged2;
    bool slider_being_dragged3;
  }

}