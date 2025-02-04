#define NAMESPACE_KIN_IMPL

#include "khg_kin/namespace.h"
#include <math.h>

kin_vec vec_create(float x, float y) {
  return (kin_vec){ x, y };
}

void vec_copy(kin_vec *vec1, kin_vec vec2) {
  vec1->x = vec2.x;
  vec1->y = vec2.y;
}

float vec_sq_length(kin_vec vec) {
  return vec.x * vec.x + vec.y * vec.y;
}

float vec_length(kin_vec vec) {
  return sqrt(vec.x * vec.x + vec.y * vec.y);
}

kin_vec vec_perp(kin_vec vec) {
  return (kin_vec){-vec.y, vec.x};
}

kin_vec vec_add(kin_vec vec1, kin_vec vec2) {
  return (kin_vec){vec1.x + vec2.x, vec1.y + vec2.y};
}

kin_vec vec_sub(kin_vec vec1, kin_vec vec2) {
  return (kin_vec){vec1.x - vec2.x, vec1.y - vec2.y};
}

float vec_dot(kin_vec vec1, kin_vec vec2) {
  return vec1.x * vec2.x + vec1.y * vec2.y;
}

float vec_cross(kin_vec vec1, kin_vec vec2) {
  return vec1.x * vec2.y - vec1.y * vec2.x;
}

kin_vec vec_scale(kin_vec vec, float scale) {
  return (kin_vec){vec.x * scale, vec.y * scale};
}

float vec_dist(kin_vec vec1, kin_vec vec2) {
  return vec_length(vec_sub(vec1, vec2));
}

kin_vec vec_rot(kin_vec vec, kin_vec ang) {
  return (kin_vec){ vec.x * ang.x + vec.y * ang.y, vec.y * ang.x - vec.x * ang.y};
}
