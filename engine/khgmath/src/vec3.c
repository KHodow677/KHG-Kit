#include "khgmath/vec3.h"
#include <math.h>

vec3 vec3_create_same_value(float value) {
  return (vec3){ value, value, value };
}

vec3 vec3_create_from_values(float x, float y, float z) {
  return (vec3) { x, y, z };
}

vec3 vec3_add(vec3 *vec3_a, vec3 *vec3_b) {
  return (vec3){ vec3_a->x + vec3_b->x, vec3_a->y + vec3_b->y, vec3_a->z + vec3_b->z };
}

vec3 vec3_subtract(vec3 *vec3_a, vec3 *vec3_b) {
  return (vec3){ vec3_a->x - vec3_b->x, vec3_a->y - vec3_b->y, vec3_a->z - vec3_b->z };
}

vec3 vec3_negate(vec3 *vec3_a) {
  return (vec3){ -vec3_a->x, -vec3_a->y, -vec3_a->z };
}

vec3 vec3_multiply_num_on_vec3(float num, vec3 *vec3_a) {
  return (vec3){ num * vec3_a->x, num * vec3_a->y, num * vec3_a->z };
}

vec3 vec3_normalize(vec3 *vec3_a) {
  vec3 vec3_normalized = *vec3_a;
  float x = vec3_a->x, y = vec3_a->y, z = vec3_a->z;
  float length = sqrt(x * x + y * y + z * z);
  vec3_normalized.x /= length;
  vec3_normalized.y /= length;
  vec3_normalized.z /= length;
  return vec3_normalized;
}

vec3 vec3_cross(vec3 *vec3_a, vec3 *vec3_b) {
  return (vec3) { (vec3_a->y * vec3_b->z) - (vec3_a->z * vec3_b->y), (vec3_a->z * vec3_b->x) - (vec3_a->x * vec3_b->z), (vec3_a->x * vec3_b->y) - (vec3_a->y * vec3_b->x) };
}

float vec3_dot(vec3 *vec3_a, vec3 *vec3_b) {
  return (vec3_a->x * vec3_b->x) + (vec3_a->y * vec3_b->y) + (vec3_a->z * vec3_b->z);
}
