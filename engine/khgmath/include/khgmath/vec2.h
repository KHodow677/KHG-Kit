#pragma once

typedef struct vec2 {
  float x;
  float y;
} vec2;

vec2 vec2CreateSameValues(float values);
vec2 vec2CreateFromValues(float x, float y);
vec2 vec2Add(vec2 *vec2_a, vec2 *vec2_b);
vec2 vec2Subtract(vec2 *vec2_a, vec2 *vec2_b);
vec2 vec2Negate(vec2 *vec2_a);
vec2 vec2MultiplyNumOnVec2(float num, vec2 *vec2_a);
vec2 vec2Normalize(vec2 *vec2_a);
float vec2Dot(vec2 *vec2_a, vec2 *vec2_b);
vec2 vec2Clamp(vec2 *vec2_a, vec2 *min, vec2 *max);
