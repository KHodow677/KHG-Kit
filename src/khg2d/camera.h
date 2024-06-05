#pragma once

#include "../math/vec2.h"

typedef struct camera {
  vec2 position;
  float rotation;
  float zoom;
} camera;

void setDefault(camera *c);
void follow(camera *c, vec2 pos, float speed, float min, float max, float w, float h);
