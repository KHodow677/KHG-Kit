#include "khg_phy/shape.h"
#include "khg_utl/error_func.h"
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

static unsigned int id_counter;

phy_shape *phy_circle_shape_new(phy_vector2 center, float radius) {
  phy_shape *shape = malloc(sizeof(phy_shape));
  if (!shape) {
    utl_error_func("Failed to allocate memory", utl_user_defined_data);
  }
  shape->id = id_counter++;
  shape->type = PHY_SHAPE_TYPE_CIRCLE;
  phy_circle *circle = &shape->circle;
  circle->center = center;
  circle->radius = radius;
  return shape;
}

phy_shape *phy_polygon_shape_new(phy_vector2 *vertices, unsigned int num_vertices, phy_vector2 offset) {
  if (num_vertices > PHY_POLYGON_MAX_VERTICES) {
    utl_error_func("Maximum number of vertices per convex polygon shape exceeded", utl_user_defined_data);
    return NULL;
  }
  if (num_vertices < 3) {
    utl_error_func("Polygon shape created with fewer than 3 vertices", utl_user_defined_data);
    return NULL;
  }
  phy_shape *shape = malloc(sizeof(phy_shape));
  if (!shape) {
    utl_error_func("Failed to allocate memory", utl_user_defined_data);
  }
  shape->id = id_counter++;
  shape->type = PHY_SHAPE_TYPE_POLYGON;
  phy_polygon *polygon = &shape->polygon;
  polygon->num_vertices = num_vertices;
  for (unsigned int i = 0; i < num_vertices; i++) {
    polygon->vertices[i] = phy_vector2_add(vertices[i], offset);
  }
  for (unsigned int i = 0; i < num_vertices; i++) {
    polygon->xvertices[i] = phy_vector2_zero;
  }
  for (unsigned int i = 0; i < num_vertices; i++) {
    phy_vector2 va = polygon->vertices[i];
    phy_vector2 vb = polygon->vertices[(i + 1) % num_vertices];
    phy_vector2 edge = phy_vector2_sub(vb, va);
    phy_vector2 normal = phy_vector2_normalize(phy_vector2_perpr(edge));
    polygon->normals[i] = normal;
  }
  return shape;
}

phy_shape *phy_rect_shape_new(float width, float height, phy_vector2 offset) {
  float w = width / 2.0;
  float h = height / 2.0;
  phy_vector2 vertices[4] = { phy_vector2_new(-w, -h), phy_vector2_new( w, -h), phy_vector2_new( w, h), phy_vector2_new(-w, h) };
  return phy_polygon_shape_new(vertices, 4, offset);
}

phy_shape *phy_ngon_shape_new(unsigned int n, float radius, phy_vector2 offset) {
  if (n < 3) {
    utl_error_func("Polygon shape created with fewer than 3 vertices", utl_user_defined_data);
    return NULL;
  }
  if (n > PHY_POLYGON_MAX_VERTICES) {
    utl_error_func("Max polygon vertices exceeded", utl_user_defined_data);
    return NULL;
  }
  phy_vector2 vertices[PHY_POLYGON_MAX_VERTICES];
  phy_vector2 arm = phy_vector2_new(radius, 0.0);
  for (unsigned int i = 0; i < n; i++) {
    vertices[i] = arm;
    arm = phy_vector2_rotate(arm, 2.0 * PHY_PI / (float)n);
  }
  return phy_polygon_shape_new(vertices, n, offset);
}

phy_shape *phy_convex_hull_shape_new(phy_vector2 *points, unsigned int num_points, phy_vector2 offset, bool center){
  if (num_points < 3) {
    utl_error_func("Polygon shape created with fewer than 3 vertices", utl_user_defined_data);
    return NULL;
  }
  phy_vector2 vertices[PHY_POLYGON_MAX_VERTICES];
  unsigned int num_vertices = phy_generate_convex_hull(points, num_points, vertices);
  if (center) {
    phy_vector2 hull_centroid = phy_polygon_centroid(vertices, num_vertices);
    for (unsigned int i = 0; i < num_vertices; i++) {
      vertices[i] = phy_vector2_sub(vertices[i], hull_centroid);
    }
  }
  return phy_polygon_shape_new(vertices, num_vertices, offset);
}

void phy_shape_free(phy_shape *shape) {
  if (!shape) {
    return;
  }
  free(shape);
}

phy_aabb phy_shape_get_aabb(phy_shape *shape, phy_transform xform) {
  float min_x;
  float min_y;
  float max_x;
  float max_y;
  phy_aabb aabb;
  float inflate = 0.00;
  switch (shape->type) {
    case PHY_SHAPE_TYPE_CIRCLE: {
      phy_vector2 c = phy_vector2_add(phy_vector2_rotate(shape->circle.center, xform.angle), xform.position);
      aabb = (phy_aabb){ c.x - shape->circle.radius, c.y - shape->circle.radius, c.x + shape->circle.radius, c.y + shape->circle.radius };
      return nvAABB_inflate(aabb, inflate);
    }
    case PHY_SHAPE_TYPE_POLYGON: {
      min_x = INFINITY;
      min_y = INFINITY;
      max_x = -INFINITY;
      max_y = -INFINITY;
      phy_polygon_transform(shape, xform);
      for (unsigned int i = 0; i < shape->polygon.num_vertices; i++) {
        phy_vector2 v = shape->polygon.xvertices[i];
        if (v.x < min_x) {
          min_x = v.x;
        }
        if (v.x > max_x) {
          max_x = v.x;
        }
        if (v.y < min_y) {
          min_y = v.y;
        }
        if (v.y > max_y) {
          max_y = v.y;
        }
      }
      aabb = (phy_aabb){ min_x, min_y, max_x, max_y };
      return nvAABB_inflate(aabb, inflate);
    }
    default:
      return (phy_aabb){ 0.0, 0.0, 0.0, 0.0 }; 
  }
}

phy_shape_mass_info phy_shape_calculate_mass(phy_shape *shape, float density) {
  float mass, inertia;
  switch (shape->type) {
    case PHY_SHAPE_TYPE_CIRCLE: {
      phy_circle circle = shape->circle;
      mass = phy_circle_area(circle.radius) * density;
      inertia = phy_circle_inertia(mass, circle.radius, circle.center);
      return (phy_shape_mass_info){ mass, inertia, circle.center };
    }
    case PHY_SHAPE_TYPE_POLYGON: {
      phy_polygon polygon = shape->polygon;
      mass = phy_polygon_area(polygon.vertices, polygon.num_vertices) * density;
      inertia = phy_polygon_inertia(mass, polygon.vertices, polygon.num_vertices);
      phy_vector2 centroid = phy_polygon_centroid(polygon.vertices, polygon.num_vertices);
      return (phy_shape_mass_info){mass, inertia, centroid};
    }
    default:
      utl_error_func("Invalid shape", utl_user_defined_data);
      return (phy_shape_mass_info){ -1.0, -1.0, phy_vector2_new(-1.0, -1.0) };
  }
}

void phy_polygon_transform(phy_shape *shape, phy_transform xform) {
  for (unsigned int i = 0; i < shape->polygon.num_vertices; i++) {
    phy_vector2 new = phy_vector2_add(xform.position, phy_vector2_rotate(shape->polygon.vertices[i], xform.angle));
    shape->polygon.xvertices[i] = new;
  }
}

