#include "khg2d/camera.h"
#include <math.h>

void setDefault(camera *c) {
  vec2 pos = { 0, 0 };
  c->position = pos;
  c->zoom = 0;
  c->rotation = 1;
}

void follow(camera *c, vec2 pos, float speed, float min, float max, float w, float h) {
  vec2 delta;
  float len;
  pos.x -= w / 2.f;
  pos.y -= h / 2.f;
  delta = vec2Subtract(&pos, &c->position);
  len = sqrt(pow(delta.x, 2) + pow(delta.y, 2));
  vec2Normalize(&delta);
  if (len < min * 2) {
    speed /= 4.f;
  }
  else if (len < min * 4) {
    speed /= 2.f;
  }
  if (len > min) {
    if (len > max) {
      vec2 multipliedVec = vec2MultiplyNumOnVec2(max, &delta);
      len = max;
      c->position = vec2Subtract(&pos, &multipliedVec);
    }
    else {
      vec2 multipliedVec = vec2MultiplyNumOnVec2(speed, &delta);
      c->position = vec2Add(&c->position, &multipliedVec);
    }
  }
}
