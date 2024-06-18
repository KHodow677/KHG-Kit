#pragma once

typedef struct vec3 {
  float x;
  float y;
  float z;
} vec3;

vec3 vec3CreateSameValue(float value);
vec3 vec3CreateFromValues(float x, float y, float z);
vec3 vec3Add(vec3 *vec3_a, vec3 *vec3_b);
vec3 vec3Subtract(vec3 *vec3_a, vec3 *vec3_b);
vec3 vec3Negate(vec3 *vec3_a);
vec3 vec3MultiplyNumOnVec3(float num, vec3 *vec3_a);
vec3 vec3Normalize(vec3 *vec3_a);
vec3 vec3Cross(vec3 *vec3_a, vec3 *vec3_b);
float vec3Dot(vec3 *vec3_a, vec3 *vec3_b);

