#pragma once

#include "mat4.h"
#include "vec3.h"

typedef struct {
  float x;
  float y;
  float z;
  float w;
} quat;

quat quatCreate(float w, float x, float y, float z);
quat quatCreateFromAxisAngle(vec3 *axis, float angle);
quat quatConjugate(quat *quat_a);
quat quatNormalize(quat *quat_a);
quat quatMultiply(quat *quat_a, quat *quat_b);
quat quatMultiplyByVec3(quat *quat_a, vec3 *vec3_a);
quat eulerToQuat(float yaw, float pitch, float roll);
vec3 quatToEuler(quat *quat_a);
mat4 quatToMat4(quat *quat_a);
quat axisAngleToQuat(vec3 *axis, float angle);

