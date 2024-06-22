#pragma once

#include "khg_math/vec2.h"

typedef struct {
  vec2 position;
  float rotation;
  float zoom;
} camera;

void set_default(camera *c);
void follow(camera *c, vec2 pos, float speed, float min, float max, float w, float h);
