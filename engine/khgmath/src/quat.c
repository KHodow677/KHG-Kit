#include "khgmath/math.h"
#include "khgmath/quat.h"
#include <math.h>

quat quatCreate(float w, float x, float y, float z) {
  quat quat;
  quat.w = w;
  quat.x = x;
  quat.y = y;
  quat.z = z;
  return quat;
}

quat quatCreateFromAxisAngle(vec3 *axis, float angle) {
  float half_angle = angle * 0.5;
  float sin_half_angle = sin(half_angle);
  return (quat){ cos(half_angle), axis->x * sin_half_angle, axis->y * sin_half_angle, axis->z * sin_half_angle };
}

quat quatConjugate(quat *quat_a) {
  return (quat) { quat_a->w, -quat_a->x, -quat_a->y, -quat_a->z };
}

quat quatNormalize(quat *quat_a) {
  quat quat_normalized = *quat_a;
  float x = quat_a->x, y = quat_a->y, z = quat_a->z, w = quat_a->w;
  float length = sqrt(x * x + y * y + z * z + w * w);
  quat_normalized.x /= length;
  quat_normalized.y /= length;
  quat_normalized.z /= length;
  quat_normalized.w /= length;
  return quat_normalized;
}

quat quatMultiply(quat *quat_a, quat *quat_b) {
  float x = quat_a->x * quat_b->w + quat_a->w * quat_b->x + quat_a->y * quat_b->z - quat_a->z * quat_b->y;
  float y = quat_a->y * quat_b->w + quat_a->w * quat_b->y + quat_a->z * quat_b->x - quat_a->x * quat_b->z;
  float z = quat_a->z * quat_b->w + quat_a->w * quat_b->z + quat_a->x * quat_b->y - quat_a->y * quat_b->x;
  float w = quat_a->w * quat_b->w - quat_a->x * quat_b->x - quat_a->y * quat_b->y - quat_a->z * quat_b->z;
  return (quat){ w, x, y, z };
}

quat quatMultiplyByVec3(quat *quat_a, vec3 *vec3_a) {
  float x = quat_a->w * vec3_a->x + quat_a->y * vec3_a->z - quat_a->z * vec3_a->y;
  float y = quat_a->w * vec3_a->y + quat_a->z * vec3_a->x - quat_a->x * vec3_a->z;
  float z = quat_a->w * vec3_a->z + quat_a->x * vec3_a->y - quat_a->y * vec3_a->x;
  float w = -quat_a->x * vec3_a->x - quat_a->y * vec3_a->y - quat_a->z * vec3_a->z;
  return (quat){ w, x, y, z };
}

quat eulerToQuat(float yaw, float pitch, float roll) {
  quat quat;
  float c_yaw = cos(yaw * 0.5);
  float s_yaw = sin(yaw * 0.5);
  float c_pitch = cos(pitch * 0.5);
  float s_pitch = sin(pitch * 0.5);
  float c_roll = cos(roll * 0.5);
  float s_roll = sin(roll * 0.5);
  quat.w = c_yaw * c_pitch * c_roll + s_yaw * s_pitch * s_roll;
  quat.x = c_yaw * c_pitch * s_roll - s_yaw * s_pitch * c_roll;
  quat.y = c_yaw * s_pitch * c_roll + s_yaw * c_pitch * s_roll;
  quat.z = s_yaw * c_pitch * c_roll - c_yaw * s_pitch * s_roll;
  return quat;
}

vec3 quatToEuler(quat *quat_a) {
  vec3 euler;
  float sin_yaw = 2 * (quat_a->w * quat_a->z + quat_a->x * quat_a->y);
  float cos_yaw = 1 - 2 * (quat_a->y * quat_a->y + quat_a->z * quat_a->z);
  float sin_pitch = sqrtf(1 + 2 * (quat_a->w * quat_a->y - quat_a->x * quat_a->z));
  float cos_pitch = sqrtf(1 - 2 * (quat_a->w * quat_a->y - quat_a->x * quat_a->z));
  float sin_roll = 2 * (quat_a->w * quat_a->x + quat_a->y * quat_a->z);
  float cos_roll = 1 - 2 * (quat_a->x * quat_a->x + quat_a->y * quat_a->y);
  euler.x = atan2f(sin_yaw, cos_yaw);
  euler.y = atan2f(sin_pitch, cos_pitch) - PI * 0.5;
  euler.z = atan2f(sin_roll, cos_roll);
  return euler;
}

mat4 quatToMat4(quat *quat_a) {
  mat4 mat4;
  float x = quat_a->x;
  float y = quat_a->y;
  float z = quat_a->z;
  float w = quat_a->w;
  mat4.values[0] = 1 - 2 * (y * y) - 2 * (z * z);
  mat4.values[1] = 2 * x * y + 2 * w * z;
  mat4.values[2] = 2 * x * z - 2 * w * y;
  mat4.values[3] = 0;
  mat4.values[4] = 2 * x * y - 2 * w * z;
  mat4.values[5] = 1 - 2 * (x * x) - 2 * (z * z);
  mat4.values[6] = 2 * y * z + 2 * w * x;
  mat4.values[7] = 0;
  mat4.values[8] = 2 * x * z + 2 * w * y;
  mat4.values[9] = 2 * y * z - 2 * w * x;
  mat4.values[10] = 1 - 2 * (x * x) - 2 * (y * y);
  mat4.values[11] = 0;
  mat4.values[12] = 0;
  mat4.values[13] = 0;
  mat4.values[14] = 0;
  mat4.values[15] = 1;
  return mat4;
}

quat axisAngleToQuat(vec3 *axis, float angle) {
  float sin_half_angle = sin(angle * 0.5);
  return (quat){ axis->x * sin_half_angle, axis->y * sin_half_angle, axis->z * sin_half_angle, cos(angle * 0.5) };
}

