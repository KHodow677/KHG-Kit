#include "khg_math/vec2.h"
#include <math.h>

vec2 vec2_create_same_values(float values) {
  return (vec2){ values, values };
}

vec2 vec2_create_from_values(float x, float y) {
  return (vec2){ x, y };
}

vec2 vec2_add(vec2 *vec2_a, vec2 *vec2_b) {
  return (vec2){ vec2_a->x + vec2_b->x, vec2_a->y + vec2_b->y };
}

vec2 vec2_subtract(vec2 *vec2_a, vec2 *vec2_b) {
  return (vec2){ vec2_a->x - vec2_b->x, vec2_a->y - vec2_b->y };
}

vec2 vec2_negate(vec2 *vec2_a) {
  return (vec2){ -vec2_a->x, -vec2_a->y };
}

vec2 vec2_multiply_num_on_vec2(float num, vec2 *vec2_a) {
  return (vec2){ num * vec2_a->x, num * vec2_a->y };
}

vec2 vec2_normalize(vec2 *vec2_a) {
  vec2 vec2_normalized = *vec2_a;
  float x = vec2_a->x, y = vec2_a->y;
  float length = sqrt(x * x + y * y);
  vec2_normalized.x /= length;
  vec2_normalized.y /= length;
  return vec2_normalized;
}

float vec2_dot(vec2 *vec2_a, vec2 *vec2_b) {
  return (vec2_a->x * vec2_b->x) + (vec2_a->y * vec2_b->y);
}

vec2 vec2_clamp(vec2 *vec2_a, vec2 *min, vec2 *max) {
  return (vec2){ fmaxf(min->x, fminf(vec2_a->x, max->x)), fmaxf(min->y, fminf(vec2_a->y, max->y)) };
}
