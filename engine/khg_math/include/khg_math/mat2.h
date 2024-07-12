#pragma once

#include "khg_math/vec2.h"

typedef struct mat2 {
  float m00;
  float m01;
  float m10;
  float m11;
} mat2;

mat2 mat2_radians(float radians);
void mat2_set(mat2 *matrix, float radians);
mat2 mat2_transpose(mat2 *matrix);
vec2 mat2_multiply_vec2(mat2 *matrix, vec2 *vector);
