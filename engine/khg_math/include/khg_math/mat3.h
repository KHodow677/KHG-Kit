#pragma once

#include "khg_math/vec3.h"

#define MAT3_SIZE 9

typedef struct mat3 {
    float values[MAT3_SIZE];
} mat3;

void mat3_print(mat3 *mat3_a);
mat3 mat3_create_from_values(float values[MAT3_SIZE]);
mat3 mat3_create_same_value(float value);
mat3 mat3_create_identity(void);
mat3 mat3_multiply(mat3 *mat3_a, mat3 *mat3_b);
mat3 mat3_scale(mat3 *transform, vec3 *scale);
mat3 mat3_translate(mat3 *transform, vec3 *translate);
mat3 mat3_rotate(mat3 *transform, float angle);
mat3 mat3_orthographic(float left, float right, float bottom, float top);
mat3 mat3_transpose(mat3 *mat3_a);
vec3 mat3_transform_vec3(mat3 *mat3_a, vec3 *vec);

