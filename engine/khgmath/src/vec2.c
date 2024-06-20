#include "khgmath/vec2.h"
#include <math.h>

vec2 vec2CreateSameValues(float values) {
  return (vec2){ values, values };
}

vec2 vec2CreateFromValues(float x, float y) {
  return (vec2){ x, y };
}

vec2 vec2Add(vec2 *vec2_a, vec2 *vec2_b) {
  return (vec2){ vec2_a->x + vec2_b->x, vec2_a->y + vec2_b->y };
}

vec2 vec2Subtract(vec2 *vec2_a, vec2 *vec2_b) {
  return (vec2){ vec2_a->x - vec2_b->x, vec2_a->y - vec2_b->y };
}

vec2 vec2Negate(vec2 *vec2_a) {
  return (vec2){ -vec2_a->x, -vec2_a->y };
}

vec2 vec2MultiplyNumOnVec2(float num, vec2 *vec2_a) {
  return (vec2){ num * vec2_a->x, num * vec2_a->y };
}

vec2 vec2Normalize(vec2 *vec2_a) {
  vec2 vec2_normalized = *vec2_a;
  float x = vec2_a->x, y = vec2_a->y;
  float length = sqrt(x * x + y * y);
  vec2_normalized.x /= length;
  vec2_normalized.y /= length;
  return vec2_normalized;
}

float vec2Dot(vec2 *vec2_a, vec2 *vec2_b) {
  return (vec2_a->x * vec2_b->x) + (vec2_a->y * vec2_b->y);
}

vec2 vec2Clamp(vec2 *vec2_a, vec2 *min, vec2 *max) {
  return (vec2){ fmaxf(min->x, fminf(vec2_a->x, max->x)), fmaxf(min->y, fminf(vec2_a->y, max->y)) };
}
