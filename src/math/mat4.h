#ifndef MAT4_H
#define MAT4_H

#include "vec3.h"

#define MAT4_SIZE 16

typedef struct mat4 {
  float values[MAT4_SIZE];
} mat4;

void mat4Print(mat4 *mat4_a);
mat4 mat4CreateFromValues(float values[MAT4_SIZE]);
mat4 mat4CreateSameValue(float value);
mat4 mat4CreateIdentity(void);
mat4 mat4Multiply(mat4 *mat4_a, mat4 *mat4_b);
mat4 mat4Scale(mat4 *transform, vec3 *scale);
mat4 mat4Translate(mat4 *transform, vec3 *translate);
mat4 mat4Rotate(mat4 *transform, vec3 *axis, float angle);
mat4 mat4Perspective(float fov, float aspect_ratio, float near_plane, float far_plane);
mat4 mat4Orthographic(float left, float right, float bottom, float top, float near_plane, float far_plane);
mat4 mat4Lookat(vec3 *position, vec3 *target, vec3 *up);

#endif
