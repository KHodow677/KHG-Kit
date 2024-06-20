#include "khgmath/vec2.h"
#include <math.h>

vec2 vec2CreateSameValues(float values) {
  vec2 vec2;
  vec2.x = values;
  vec2.y = values;
  return vec2;
}

vec2 vec2CreateFromValues(float x, float y) {
  vec2 vec2;
  vec2.x = x;
  vec2.y = y;
  return vec2;
}

vec2 vec2Add(vec2 *vec2_a, vec2 *vec2_b) {
  vec2 vec2_added;
  vec2_added.x = vec2_a->x + vec2_b->x;
  vec2_added.y = vec2_a->y + vec2_b->y;
  return vec2_added;
}

vec2 vec2Subtract(vec2 *vec2_a, vec2 *vec2_b) {
  vec2 vec2_subtracted;
  vec2_subtracted.x = vec2_a->x - vec2_b->x;
  vec2_subtracted.y = vec2_a->y - vec2_b->y;
  return vec2_subtracted;
}

vec2 vec2Negate(vec2 *vec2_a) {
  vec2 vec2_negated;
  vec2_negated.x = -vec2_a->x;
  vec2_negated.y = -vec2_a->y;
  return vec2_negated;
}

vec2 vec2MultiplyNumOnVec2(float num, vec2 *vec2_a) {
  vec2 vec2_multiplied;
  vec2_multiplied.x = num * vec2_a->x;
  vec2_multiplied.y = num * vec2_a->y;
  return vec2_multiplied;
}

vec2 vec2Normalize(vec2 *vec2_a) {
  vec2 vec2_normalized = *vec2_a;
  float x = vec2_a->x;
  float y = vec2_a->y;
  float length = sqrt(x * x + y * y);
  vec2_normalized.x /= length;
  vec2_normalized.y /= length;
  return vec2_normalized;
}

float vec2Dot(vec2 *vec2_a, vec2 *vec2_b) {
  float dot;
  dot = (vec2_a->x * vec2_b->x) + (vec2_a->y * vec2_b->y);
  return dot;
}

vec2 vec2Clamp(vec2 *vec2_a, vec2 *min, vec2 *max) {
  vec2 vec2_clamped;
  vec2_clamped.x = fmaxf(min->x, fminf(vec2_a->x, max->x));
  vec2_clamped.y = fmaxf(min->y, fminf(vec2_a->y, max->y));
  return vec2_clamped;
}