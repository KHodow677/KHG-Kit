#pragma once

#include "khg_phy/core/phy_constants.h"
#include "khg_phy/core/phy_vector.h"
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct phy_transform {
  phy_vector2 position;
  float angle;
} phy_transform;

static phy_vector2 phy_convex_hull_pivot;

static inline uint64_t phy_u32_pair(uint32_t x, uint32_t y) {
  return (uint64_t)x << 32 | y;
}

static inline uint32_t phy_u32_hash(uint32_t x) {
  x = ((x >> 16) ^ x) * 0x45d9f3b;
  x = ((x >> 16) ^ x) * 0x45d9f3b;
  x = (x >> 16) ^ x;
  return x;
}

static inline float phy_fclamp(float value, float min_value, float max_value) {
  return fminf(fmaxf(value, min_value), max_value);
}

static inline phy_vector2 phy_calc_relative_velocity(phy_vector2 linear_velocity_a, float angular_velocity_a, phy_vector2 ra, phy_vector2 linear_velocity_b, float angular_velocity_b, phy_vector2 rb) {
  phy_vector2 ra_perp = phy_vector2_perp(ra);
  phy_vector2 rb_perp = phy_vector2_perp(rb);
  return phy_vector2_sub(phy_vector2_add(linear_velocity_b, phy_vector2_mul(rb_perp, angular_velocity_b)), phy_vector2_add(linear_velocity_a, phy_vector2_mul(ra_perp, angular_velocity_a)));
}

static inline float phy_calc_mass_k(phy_vector2 normal, phy_vector2 ra, phy_vector2 rb, float invmass_a, float invmass_b, float invinertia_a, float invinertia_b) {
  phy_vector2 ra_perp = phy_vector2_perp(ra);
  phy_vector2 rb_perp = phy_vector2_perp(rb);
  float ran = phy_vector2_dot(ra_perp, normal);
  float rbn = phy_vector2_dot(rb_perp, normal);
  ran *= ran;
  rbn *= rbn;
  return (invmass_a + invmass_b) + ((ran * invinertia_a) + (rbn * invinertia_b));
}

static inline float phy_circle_area(float radius) {
  return (float)PHY_PI * (radius * radius);
}

static inline float phy_circle_inertia(float mass, float radius, phy_vector2 offset) {
  return 0.5 * mass * (radius * radius) + mass * phy_vector2_len2(offset);
}

static inline float phy_polygon_area(phy_vector2 *vertices, size_t num_vertices) {
  float area = 0.0f;
  size_t j = num_vertices - 1;
  for (size_t i = 0; i < num_vertices; i++) {
    phy_vector2 va = vertices[i];
    phy_vector2 vb = vertices[j];
    area += (vb.x + va.x) * (vb.y - va.y);
    j = i;
  }
  return fabsf(area / 2.0f);
}

static inline float phy_polygon_inertia(float mass, phy_vector2 *vertices, size_t num_vertices) {
  float sum1 = 0.0;
  float sum2 = 0.0;
  for (size_t i = 0; i < num_vertices; i++) {
    phy_vector2 v1 = vertices[i];
    phy_vector2 v2 = vertices[(i + 1) % num_vertices];
    float a = phy_vector2_cross(v2, v1);
    float b = phy_vector2_dot(v1, v1) + phy_vector2_dot(v1, v2) + phy_vector2_dot(v2, v2);
    sum1 += a * b;
    sum2 += a;
  }
  return (mass * sum1) / (6.0 * sum2);
}

static inline phy_vector2 phy_polygon_centroid(phy_vector2 *vertices, size_t num_vertices) {
  phy_vector2 sum = phy_vector2_zero;
  for (size_t i = 0; i < num_vertices; i++) {
    sum = phy_vector2_add(sum, vertices[i]);
  }
  return phy_vector2_div(sum, (float)num_vertices);
}

static inline int phy_triangle_winding(phy_vector2 vertices[3]) {
  phy_vector2 ba = phy_vector2_sub(vertices[1], vertices[0]);
  phy_vector2 ca = phy_vector2_sub(vertices[2], vertices[0]);
  float z = phy_vector2_cross(ba, ca);
  if (z < 0.0) {
    return -1;
  }
  else if (z > 0.0) {
    return 1;
  }
  else {
    return 0;
  }
}

static int phy_convex_hull_orientation(phy_vector2 p, phy_vector2 q, phy_vector2 r) {
  float d = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
  if (d == 0.0) {
    return 0;
  }
  return (d > 0.0) ? 1 : 2;
}

static int phy_convex_hull_cmp(const void *el0, const void *el1) {
  phy_vector2 v0 = *(phy_vector2 *)el0;
  phy_vector2 v1 = *(phy_vector2 *)el1;
  int o = phy_convex_hull_orientation(phy_convex_hull_pivot, v0, v1);
  if (o == 0) {
    if (phy_vector2_dist2(phy_convex_hull_pivot, v1) >= phy_vector2_dist2(phy_convex_hull_pivot, v0)) {
      return -1;
    }
    else {
      return 1;
    }
  }
  else {
    if (o == 2) {
      return -1;
    }
    else {
      return 1;
    }
  }
}

static inline size_t phy_generate_convex_hull(phy_vector2 *points, size_t num_points, phy_vector2 *vertices) {
  size_t n = num_points;
  size_t current_min_i = 0;
  float min_y = points[current_min_i].x;
  phy_vector2 pivot;
  for (size_t i = 0; i < n; i++) {
    phy_vector2 v = points[i];
    if (v.y < min_y || (v.y == min_y && v.x < points[current_min_i].x)) {
      current_min_i = i;
      min_y = v.y;
    }
  }
  phy_vector2 temp = points[0];
  points[0] = points[current_min_i];
  points[current_min_i] = temp;
  pivot = points[0];
  phy_convex_hull_pivot = pivot;
  phy_vector2 tmp_points[n];
  for (size_t i = 0; i < n; i++) {
    phy_vector2 v = points[i];
    tmp_points[i] = v;
  }
  qsort(&tmp_points[1], n - 1, sizeof(phy_vector2), phy_convex_hull_cmp);
  for (size_t i = 0; i < n; i++) {
    phy_vector2 *v = &points[i];
    v->x = tmp_points[i].x;
    v->y = tmp_points[i].y;
  }
  phy_vector2 *hull = malloc(sizeof(phy_vector2) * n);
  size_t hull_size = 3;
  hull[0] = points[0];
  hull[1] = points[1];
  hull[2] = points[2];
  for (size_t i = 3; i < n; i++) {
    while (hull_size > 1 && phy_convex_hull_orientation(hull[hull_size - 2], hull[hull_size - 1], points[i]) != 2) {
      hull_size--;
    }
    hull[hull_size++] = points[i];
  }
  size_t final_size;
  if (hull_size > PHY_POLYGON_MAX_VERTICES) {
    final_size = PHY_POLYGON_MAX_VERTICES;
  }
  else {
    final_size = hull_size;
  }
  for (size_t i = 0; i < final_size; i++) {
    vertices[i] = hull[i];
  }
  free(hull);
  return final_size;
}

