#include "khg2d/camera.h"
#include <math.h>

void set_default(camera *c) {
  vec2 pos = { 0, 0 };
  c->position = pos;
  c->zoom = 0;
  c->rotation = 1;
}

void follow(camera *c, vec2 pos, float speed, float min, float max, float w, float h) {
  pos.x -= w / 2.f;
  pos.y -= h / 2.f;
  vec2 delta = vec2_subtract(&pos, &c->position);
  float len = sqrt(pow(delta.x, 2) + pow(delta.y, 2));
  vec2_normalize(&delta);
  if (len < min * 2) {
    speed /= 4.f;
  }
  else if (len < min * 4) {
    speed /= 2.f;
  }
  if (len > min) {
    if (len > max) {
      vec2 multiplied_vec = vec2_multiply_num_on_vec2(max, &delta);
      len = max;
      c->position = vec2_subtract(&pos, &multiplied_vec);
    }
    else {
      vec2 multiplied_vec = vec2_multiply_num_on_vec2(speed, &delta);
      c->position = vec2_add(&c->position, &multiplied_vec);
    }
  }
}
