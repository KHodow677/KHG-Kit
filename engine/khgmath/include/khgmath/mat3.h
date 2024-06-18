#pragma once

#include "vec3.h"

#define MAT3_SIZE 9

typedef struct mat3 {
    float values[MAT3_SIZE];
} mat3;

void mat3Print(mat3 *mat3_a);
mat3 mat3CreateFromValues(float values[MAT3_SIZE]);
mat3 mat3CreateSameValue(float value);
mat3 mat3CreateIdentity(void);
mat3 mat3Multiply(mat3 *mat3_a, mat3 *mat3_b);
mat3 mat3Scale(mat3 *transform, vec3 *scale);
mat3 mat3Translate(mat3 *transform, vec3 *translate);
mat3 mat3Rotate(mat3 *transform, float angle);
mat3 mat3Orthographic(float left, float right, float bottom, float top);
mat3 mat3Transpose(mat3 *mat3_a);
vec3 mat3TransformVec3(mat3 *mat3_a, vec3 *vec);

