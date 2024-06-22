#include "khg_2d/renderer_2d.h"
#include "khg_math/vec2.h"

typedef struct {
  vec2 mouse_pos;
  bool mouse_click;
  bool mouse_held;
  bool mouse_released;
  bool escape_released;
} input_data;
