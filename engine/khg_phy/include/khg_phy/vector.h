#pragma once

#include <math.h>
#include <stdbool.h>

typedef struct phy_vector2 {
  float x;
  float y;
} phy_vector2;

#define phy_vector2_new(x, y) ((phy_vector2){ (x), (y) })

static const phy_vector2 phy_vector2_zero = { 0.0, 0.0 };

static inline bool phy_vector2_eq(phy_vector2 a, phy_vector2 b) {
  return (a.x == b.x && a.y == b.y);
}

static inline phy_vector2 phy_vector2_add(phy_vector2 a, phy_vector2 b) {
  return phy_vector2_new(a.x + b.x, a.y + b.y);
}

static inline phy_vector2 phy_vector2_sub(phy_vector2 a, phy_vector2 b) {
  return phy_vector2_new(a.x - b.x, a.y - b.y);
}

static inline phy_vector2 phy_vector2_mul(phy_vector2 v, float s) {
  return phy_vector2_new(v.x * s, v.y * s);
}

static inline phy_vector2 phy_vector2_div(phy_vector2 v, float s) {
  return phy_vector2_new(v.x / s, v.y / s);
}

static inline phy_vector2 phy_vector2_neg(phy_vector2 v) {
  return phy_vector2_new(-v.x, -v.y);
}

static inline phy_vector2 phy_vector2_rotate(phy_vector2 v, float a) {
  float c = cosf(a);
  float s = sinf(a);
  return phy_vector2_new(c * v.x - s * v.y, s * v.x + c * v.y);
}

static inline phy_vector2 phy_vector2_perp(phy_vector2 v) {
  return phy_vector2_new(-v.y, v.x);
}

static inline phy_vector2 phy_vector2_perpr(phy_vector2 v) {
  return phy_vector2_new(v.y, -v.x);
}

static inline float phy_vector2_len2(phy_vector2 v) {
  return v.x * v.x + v.y * v.y;
}

static inline float phy_vector2_len(phy_vector2 v) {
  return sqrtf(phy_vector2_len2(v));
}

static inline float phy_vector2_dot(phy_vector2 a, phy_vector2 b) {
  return a.x * b.x + a.y * b.y;
}

static inline float phy_vector2_cross(phy_vector2 a, phy_vector2 b) {
  return a.x * b.y - a.y * b.x;
}

static inline float phy_vector2_dist2(phy_vector2 a, phy_vector2 b) {
  return (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
}

static inline float phy_vector2_dist(phy_vector2 a, phy_vector2 b) {
  return sqrtf(phy_vector2_dist2(a, b));
}

static inline phy_vector2 phy_vector2_normalize(phy_vector2 v) {
  return phy_vector2_div(v, phy_vector2_len(v));
}

static inline phy_vector2 phy_vector2_lerp(phy_vector2 a, phy_vector2 b, float t) {
  return phy_vector2_new((1.0 - t) * a.x + t * b.x, (1.0 - t) * a.y + t * b.y);
}

static inline bool phy_vector2_is_zero(phy_vector2 v) {
  return v.x == 0.0 && v.y == 0.0;
}

