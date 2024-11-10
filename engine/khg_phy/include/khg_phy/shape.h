#pragma once

#include "khg_phy/aabb.h"
#include "khg_phy/math.h"
#include "khg_phy/vector.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  PHY_SHAPE_TYPE_CIRCLE,
  PHY_SHAPE_TYPE_POLYGON
} phy_shape_type;

typedef struct phy_shape_mass_info {
  float mass;
  float inertia;
  phy_vector2 center;
} phy_shape_mass_info;

typedef struct phy_circle {
  phy_vector2 center;
  float radius;
} phy_circle;

typedef struct phy_polygon {
  phy_vector2 vertices[PHY_POLYGON_MAX_VERTICES];
  phy_vector2 xvertices[PHY_POLYGON_MAX_VERTICES];
  phy_vector2 normals[PHY_POLYGON_MAX_VERTICES];
  size_t num_vertices;
} phy_polygon;

typedef struct phy_shape {
  phy_shape_type type;
  uint32_t id;
  union {
    phy_circle circle;
    phy_polygon polygon;
  };
} phy_shape;

phy_shape *phy_circle_shape_new(phy_vector2 center, float radius);
phy_shape *phy_polygon_shape_new(phy_vector2 *vertices, size_t num_vertices, phy_vector2 offset);
phy_shape *phy_rect_shape_new(float width, float height, phy_vector2 offset);
phy_shape *phy_ngon_shape_new(size_t n, float radius, phy_vector2 offset);
phy_shape *phy_convex_hull_shape_new(phy_vector2 *points, size_t num_points, phy_vector2 offset, bool center);
void phy_shape_free(phy_shape *shape);

phy_aabb phy_shape_get_aabb(phy_shape *shape, phy_transform xform);
phy_shape_mass_info phy_shape_calculate_mass(phy_shape *shape, float density);
void phy_polygon_transform(phy_shape *shape, phy_transform xform);

