#ifndef VEC4_H
#define VEC4_H

typedef struct vec4 {
  float x;
  float y;
  float z;
  float w;
} vec4;

vec4 vec4CreateSameValues(float values);
vec4 vec4CreateFromValues(float x, float y, float z, float w);
float vec4Dot(vec4 *vec4_a, vec4 *vec4_b);
vec4 vec4MultiplyNumOnVec4(float num, vec4 *vec4_a);

#endif /* VEC4_H */
