#include <khgmath/mat3.h>
#include <math.h>
#include <stdio.h>

void mat3Print(mat3 *mat3_a) {
  printf("%f %f %f\n", mat3_a->values[0], mat3_a->values[3], mat3_a->values[6]);
  printf("%f %f %f\n", mat3_a->values[1], mat3_a->values[4], mat3_a->values[7]);
  printf("%f %f %f\n", mat3_a->values[2], mat3_a->values[5], mat3_a->values[8]);
  printf("\n");
}

mat3 mat3CreateFromValues(float values[MAT3_SIZE]) {
  mat3 mat3;
  size_t i;
  for (i = 0; i < MAT3_SIZE; i++) {
    mat3.values[i] = values[i];
  }
  return mat3;
}

mat3 mat3CreateSameValue(float value) {
  mat3 mat3;
  size_t i;
  for (i = 0; i < MAT3_SIZE; i++) {
    mat3.values[i] = value;
  }
  return mat3;
}

mat3 mat3CreateIdentity(void) {
  mat3 mat3 = mat3CreateSameValue(0.0f);
  mat3.values[0] = 1.0f;
  mat3.values[4] = 1.0f;
  mat3.values[8] = 1.0f;
  return mat3;
}

mat3 mat3Multiply(mat3 *mat3_a, mat3 *mat3_b) {
  mat3 mat3;
  size_t row, column;
  for (row = 0; row < 3; row++) {
    for (column = 0; column < 3; column++) {
      mat3.values[row * 3 + column] =
        mat3_a->values[0 * 3 + column] * mat3_b->values[row * 3 + 0] +
        mat3_a->values[1 * 3 + column] * mat3_b->values[row * 3 + 1] +
        mat3_a->values[2 * 3 + column] * mat3_b->values[row * 3 + 2];
    }
  }
  return mat3;
}

mat3 mat3Scale(mat3 *transform, vec3 *scale) {
  mat3 transform_scale = mat3CreateIdentity();
  transform_scale.values[0] = scale->x;
  transform_scale.values[4] = scale->y;
  transform_scale.values[8] = scale->z;
  transform_scale = mat3Multiply(transform, &transform_scale);
  return transform_scale;
}

mat3 mat3Translate(mat3 *transform, vec3 *translate) {
  mat3 transform_translate = mat3CreateIdentity();
  transform_translate.values[6] = translate->x;
  transform_translate.values[7] = translate->y;
  transform_translate.values[8] = translate->z;
  transform_translate = mat3Multiply(transform, &transform_translate);
  return transform_translate;
}

mat3 mat3Rotate(mat3 *transform, float angle) {
  mat3 transform_rotate = mat3CreateIdentity();
  float s_angle = sin(angle);
  float c_angle = cos(angle);
  transform_rotate.values[0] = c_angle;
  transform_rotate.values[1] = -s_angle;
  transform_rotate.values[3] = s_angle;
  transform_rotate.values[4] = c_angle;
  transform_rotate = mat3Multiply(transform, &transform_rotate);
  return transform_rotate;
}

mat3 mat3Orthographic(float left, float right, float bottom, float top) {
  mat3 orthographic = mat3CreateSameValue(0.0f);
  orthographic.values[0] = 2.0f / (right - left);
  orthographic.values[4] = 2.0f / (top - bottom);
  orthographic.values[6] = -(right + left) / (right - left);
  orthographic.values[7] = -(top + bottom) / (top - bottom);
  orthographic.values[8] = 1.0f;
  return orthographic;
}

mat3 mat3Transpose(mat3 *mat3_a) {
    mat3 transpose;
    transpose.values[0] = mat3_a->values[0];
    transpose.values[1] = mat3_a->values[3];
    transpose.values[2] = mat3_a->values[6];
    transpose.values[3] = mat3_a->values[1];
    transpose.values[4] = mat3_a->values[4];
    transpose.values[5] = mat3_a->values[7];
    transpose.values[6] = mat3_a->values[2];
    transpose.values[7] = mat3_a->values[5];
    transpose.values[8] = mat3_a->values[8];
    return transpose;
}

vec3 mat3TransformVec3(mat3 *mat3_a, vec3 *vec) {
    vec3 result;
    result.x = mat3_a->values[0] * vec->x + mat3_a->values[3] * vec->y + mat3_a->values[6] * vec->z;
    result.y = mat3_a->values[1] * vec->x + mat3_a->values[4] * vec->y + mat3_a->values[7] * vec->z;
    result.z = mat3_a->values[2] * vec->x + mat3_a->values[5] * vec->y + mat3_a->values[8] * vec->z;
    return result;
}
