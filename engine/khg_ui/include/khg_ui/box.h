#include "khg_math/vec4.h"

typedef struct {
  vec4 dimensions;
  float aspect;
  char x_center_state;
  char y_center_state;
  char dimensions_state;
} box;

vec4 box_operate(box *b);
