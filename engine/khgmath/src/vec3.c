#include "khgmath/vec3.h"
#include <math.h>

vec3 vec3CreateSameValue(float value) {
  return (vec3){ value, value, value };
}

vec3 vec3CreateFromValues(float x, float y, float z) {
  return (vec3) { x, y, z };
}

vec3 vec3Add(vec3 *vec3_a, vec3 *vec3_b) {
  return (vec3){ vec3_a->x + vec3_b->x, vec3_a->y + vec3_b->y, vec3_a->z + vec3_b->z };
}

vec3 vec3Subtract(vec3 *vec3_a, vec3 *vec3_b) {
  return (vec3){ vec3_a->x - vec3_b->x, vec3_a->y - vec3_b->y, vec3_a->z - vec3_b->z };
}

vec3 vec3Negate(vec3 *vec3_a) {
  return (vec3){ -vec3_a->x, -vec3_a->y, -vec3_a->z };
}

vec3 vec3MultiplyNumOnVec3(float num, vec3 *vec3_a) {
  return (vec3){ num * vec3_a->x, num * vec3_a->y, num * vec3_a->z };
}

vec3 vec3Normalize(vec3 *vec3_a) {
  vec3 vec3_normalized = *vec3_a;
  float x = vec3_a->x, y = vec3_a->y, z = vec3_a->z;
  float length = sqrt(x * x + y * y + z * z);
  vec3_normalized.x /= length;
  vec3_normalized.y /= length;
  vec3_normalized.z /= length;
  return vec3_normalized;
}

vec3 vec3Cross(vec3 *vec3_a, vec3 *vec3_b) {
  return (vec3) { (vec3_a->y * vec3_b->z) - (vec3_a->z * vec3_b->y), (vec3_a->z * vec3_b->x) - (vec3_a->x * vec3_b->z), (vec3_a->x * vec3_b->y) - (vec3_a->y * vec3_b->x) };
}

float vec3Dot(vec3 *vec3_a, vec3 *vec3_b) {
  return (vec3_a->x * vec3_b->x) + (vec3_a->y * vec3_b->y) + (vec3_a->z * vec3_b->z);
}
