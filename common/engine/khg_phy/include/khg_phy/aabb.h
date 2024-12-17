#pragma once

#include <math.h>

typedef struct {
  float min_x;
  float min_y;
  float max_x;
  float max_y;
} phy_aabb;

static inline phy_aabb nvAABB_merge(phy_aabb a, phy_aabb b) {
  return (phy_aabb){ fminf(a.min_x, b.min_x), fminf(a.min_y, b.min_y), fmaxf(a.max_x, b.max_x), fmaxf(a.max_y, b.max_y) };
}

static inline phy_aabb nvAABB_inflate(phy_aabb aabb, float amount) {
  return (phy_aabb){ aabb.min_x - amount, aabb.min_y - amount, aabb.max_x + amount, aabb.max_y + amount };
}

