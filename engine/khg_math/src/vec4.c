#include "khg_math/vec4.h"

vec4 vec4_create_same_values(float values) {
  return (vec4){ values, values, values, values };
}

vec4 vec4_create_from_values(float x, float y, float z, float w) {
  return (vec4) { x, y, z, w };
}

float vec4_dot(vec4 *vec4_a, vec4 *vec4_b) {
  return (vec4_a->x * vec4_b->x) + (vec4_a->y * vec4_b->y) + (vec4_a->z * vec4_b->z) + (vec4_a->w * vec4_b->w);
}

vec4 vec4_multiply_num_on_vec4(float num, vec4 *vec4_a) {
  return (vec4){ num * vec4_a->x, num * vec4_a->y, num * vec4_a->z, num * vec4_a->w };
}
