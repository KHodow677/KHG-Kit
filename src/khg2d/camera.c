#include "khg2d.h"

void setDefault(camera *c) {
  c = &(camera){ (vec2){ 0, 0}, 0, 1 };
}

void follow(camera *c, vec2 pos, float speed, float min, float max, float w, float h) {
  pos.x -= w / 2.f;
  pos.y -= h / 2.f;
  vec2 delta = vec2Subtract(&pos, &c->position);
  float len = vec2Length(&delta);
  vec2Normalize(&delta);
  if (len < min * 2) {
    speed /= 4.f;
  }
  else if (len < min * 4) {
    speed /= 2.f;
  }
  if (len > min) {
    if (len > max) {
      len = max;
      vec2 multipliedVec = vec2MultiplyNumOnVec2(max, &delta);
      c->position = vec2Subtract(&pos, &multipliedVec);
    }
    else {
      vec2 multipliedVec = vec2MultiplyNumOnVec2(speed, &delta);
      c->position = vec2Add(&c->position, &multipliedVec);
    }
  }
}
