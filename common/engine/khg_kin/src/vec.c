#include "khg_kin/vec.h"
#include <math.h>

kin_vec kin_vec_create(float x, float y) {
  return (kin_vec){x, y};
}

void kin_vec_copy(kin_vec* v1, kin_vec v2) {
  v1->x = v2.x;
  v1->y = v2.y;
}

float kin_vec_sq_length(kin_vec v) {
  return v.x * v.x + v.y * v.y;
}

float kin_vec_length(kin_vec v) {
  return sqrt(v.x * v.x + v.y * v.y);
}

kin_vec kin_vec_perp(kin_vec v) {
  return (kin_vec){-v.y, v.x};
}

kin_vec kin_vec_add(kin_vec v1, kin_vec v2) {
  return (kin_vec){v1.x + v2.x, v1.y + v2.y};
}

kin_vec kin_vec_sub(kin_vec v1, kin_vec v2) {
  return (kin_vec){v1.x - v2.x, v1.y - v2.y};
}

float kin_vec_dot(kin_vec v1, kin_vec v2) {
  return v1.x * v2.x + v1.y * v2.y;
}

float kin_vec_cross(kin_vec v1, kin_vec v2) {
  return v1.x * v2.y - v1.y * v2.x;
}

kin_vec kin_vec_scale(kin_vec v, float f) {
  return (kin_vec){v.x * f, v.y * f};
}

float kin_vec_dist(kin_vec v1, kin_vec v2) {
  return kin_vec_length(kin_vec_sub(v1, v2));
}

kin_vec kin_vec_rot(kin_vec v, kin_vec r) {
  return (kin_vec){ v.x * r.x + v.y * r.y, v.y * r.x - v.x * r.y};
}
