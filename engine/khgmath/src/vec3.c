#include "khgmath/vec3.h"
#include <math.h>

vec3 vec3CreateSameValue(float value) {
  vec3 vec3;
  vec3.x = value;
  vec3.y = value;
  vec3.z = value;
  return vec3;
}

vec3 vec3CreateFromValues(float x, float y, float z) {
  vec3 vec3;
  vec3.x = x;
  vec3.y = y;
  vec3.z = z;
  return vec3;
}

vec3 vec3Add(vec3 *vec3_a, vec3 *vec3_b) {
  vec3 vec3_added;
  vec3_added.x = vec3_a->x + vec3_b->x;
  vec3_added.y = vec3_a->y + vec3_b->y;
  vec3_added.z = vec3_a->z + vec3_b->z;
  return vec3_added;
}

vec3 vec3Subtract(vec3 *vec3_a, vec3 *vec3_b) {
  vec3 vec3_subtracted;
  vec3_subtracted.x = vec3_a->x - vec3_b->x;
  vec3_subtracted.y = vec3_a->y - vec3_b->y;
  vec3_subtracted.z = vec3_a->z - vec3_b->z;
  return vec3_subtracted;
}

vec3 vec3Negate(vec3 *vec3_a) {
  vec3 vec3_negated;
  vec3_negated.x = -vec3_a->x;
  vec3_negated.y = -vec3_a->y;
  vec3_negated.z = -vec3_a->z;
  return vec3_negated;
}

vec3 vec3MultiplyNumOnVec3(float num, vec3 *vec3_a) {
  vec3 vec3_multiplied;
  vec3_multiplied.x = num * vec3_a->x;
  vec3_multiplied.y = num * vec3_a->y;
  vec3_multiplied.z = num * vec3_a->z;
  return vec3_multiplied;
}

vec3 vec3Normalize(vec3 *vec3_a) {
  vec3 vec3_normalized = *vec3_a;
  float x = vec3_a->x;
  float y = vec3_a->y;
  float z = vec3_a->z;
  float length = sqrt(x * x + y * y + z * z);
  vec3_normalized.x /= length;
  vec3_normalized.y /= length;
  vec3_normalized.z /= length;
  return vec3_normalized;
}

vec3 vec3Cross(vec3 *vec3_a, vec3 *vec3_b) {
  vec3 vec3_crossed;
  vec3_crossed.x = (vec3_a->y * vec3_b->z) - (vec3_a->z * vec3_b->y);
  vec3_crossed.y = (vec3_a->z * vec3_b->x) - (vec3_a->x * vec3_b->z);
  vec3_crossed.z = (vec3_a->x * vec3_b->y) - (vec3_a->y * vec3_b->x);
  return vec3_crossed;
}

float vec3Dot(vec3 *vec3_a, vec3 *vec3_b) {
  float dot;
  dot = (vec3_a->x * vec3_b->x) + (vec3_a->y * vec3_b->y) + (vec3_a->z * vec3_b->z);
  return dot;
}
