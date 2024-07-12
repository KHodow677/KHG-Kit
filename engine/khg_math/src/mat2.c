#include "khg_math/mat2.h"
#include "khg_math/vec2.h"
#include <math.h>

mat2 mat2_radians(float radians) {
  float c = cosf(radians);
  float s = sinf(radians);
  return (mat2) { c, -s, s, c };
}

void mat2_set(mat2 *matrix, float radians) {
  float cos = cosf(radians);
  float sin = sinf(radians);
  matrix->m00 = cos;
  matrix->m01 = -sin;
  matrix->m10 = sin;
  matrix->m11 = cos;
}

mat2 mat2_transpose(mat2 *matrix) {
  return (mat2){ matrix->m00, matrix->m10, matrix->m01, matrix->m11 };
}
 
vec2 mat2_multiply_vec2(mat2 *matrix, vec2 *vector) {
  return (vec2){ matrix->m00 * vector->x + matrix->m01 * vector->y, matrix->m10 * vector->x + matrix->m11 * vector->y };
}
