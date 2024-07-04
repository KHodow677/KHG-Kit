#pragma once

typedef struct vec4 {
  float x;
  float y;
  float z;
  float w;
} vec4;

vec4 vec4_create_same_values(float values);
vec4 vec4_create_from_values(float x, float y, float z, float w);
float vec4_dot(vec4 *vec4_a, vec4 *vec4_b);
vec4 vec4_multiply_num_on_vec4(float num, vec4 *vec4_a);
