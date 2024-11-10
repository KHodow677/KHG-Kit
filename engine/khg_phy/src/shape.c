/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#include "khg_phy/internal.h"
#include "khg_utl/error_func.h"
#include "khg_phy/shape.h"
#include <math.h>


/**
 * @file shape.c
 * 
 * @brief Collision shape implementations.
 */


/*
    Cheap solution, but it works..!
    No one would need over 4 billion shapes... right?
*/
static nv_uint32 id_counter;


nvShape *nvCircleShape_new(phy_vector2 center, nv_float radius) {
    nvShape *shape = NV_NEW(nvShape);
    if (!shape) {
      utl_error_func("Failed to allocate memory", utl_user_defined_data);
    }
    shape->id = id_counter++;

    shape->type = nvShapeType_CIRCLE;
    nvCircle *circle = &shape->circle;

    circle->center = center;
    circle->radius = radius;

    return shape;
}

nvShape *nvPolygonShape_new(
    phy_vector2 *vertices,
    size_t num_vertices,
    phy_vector2 offset
) {
    if (num_vertices > NV_POLYGON_MAX_VERTICES) {
      utl_error_func("Maximum number of vertices per convex polygon shape exceeded", utl_user_defined_data);
      return NULL;
    }

    if (num_vertices < 3) {
      utl_error_func("Polygon shape created with fewer than 3 vertices", utl_user_defined_data);
      return NULL;
    }

    nvShape *shape = NV_NEW(nvShape);
    if (!shape) {
      utl_error_func("Failed to allocate memory", utl_user_defined_data);
    }

    shape->id = id_counter++;

    shape->type = nvShapeType_POLYGON;
    nvPolygon *polygon = &shape->polygon;
    polygon->num_vertices = num_vertices;

    for (size_t i = 0; i < num_vertices; i++)
        polygon->vertices[i] = nvVector2_add(vertices[i], offset);

    for (size_t i = 0; i < num_vertices; i++)
        polygon->xvertices[i] = nvVector2_zero;

    for (size_t i = 0; i < num_vertices; i++) {
        phy_vector2 va = polygon->vertices[i];
        phy_vector2 vb = polygon->vertices[(i + 1) % num_vertices];
    
        phy_vector2 edge = nvVector2_sub(vb, va);
        phy_vector2 normal = nvVector2_normalize(nvVector2_perpr(edge));

        polygon->normals[i] = normal;
    }

    return shape;
}

nvShape *nvRectShape_new(nv_float width, nv_float height, phy_vector2 offset) {
    nv_float w = width / 2.0;
    nv_float h = height / 2.0;

    phy_vector2 vertices[4] = {
        NV_VECTOR2(-w, -h),
        NV_VECTOR2( w, -h),
        NV_VECTOR2( w, h),
        NV_VECTOR2(-w, h)
    };

    return nvPolygonShape_new(vertices, 4, offset);
}

nvShape *nvNGonShape_new(size_t n, nv_float radius, phy_vector2 offset) {
    if (n < 3) {
      utl_error_func("Polygon shape created with fewer than 3 vertices", utl_user_defined_data);
        return NULL;
    }
    if (n > NV_POLYGON_MAX_VERTICES) {
      utl_error_func("Max polygon vertices exceeded", utl_user_defined_data);
      return NULL;
    }

    phy_vector2 vertices[NV_POLYGON_MAX_VERTICES];
    phy_vector2 arm = NV_VECTOR2(radius, 0.0);

    for (size_t i = 0; i < n; i++) {
        vertices[i] = arm;
        arm = nvVector2_rotate(arm, 2.0 * NV_PI / (nv_float)n);
    }

    return nvPolygonShape_new(vertices, n, offset);
}

nvShape *nvConvexHullShape_new(
    phy_vector2 *points,
    size_t num_points,
    phy_vector2 offset,
    nv_bool center
){
    if (num_points < 3) {
      utl_error_func("Polygon shape created with fewer than 3 vertices", utl_user_defined_data);
      return NULL;
    }

    phy_vector2 vertices[NV_POLYGON_MAX_VERTICES];
    size_t num_vertices = nv_generate_convex_hull(points, num_points, vertices);

    if (center) {
        phy_vector2 hull_centroid = nv_polygon_centroid(vertices, num_vertices);
        for (size_t i = 0; i < num_vertices; i++) {
            vertices[i] = nvVector2_sub(vertices[i], hull_centroid);
        }
    }

    return nvPolygonShape_new(vertices, num_vertices, offset);
}

void nvShape_free(nvShape *shape) {
    if (!shape) return;

    NV_FREE(shape);
}

phy_aabb nvShape_get_aabb(nvShape *shape, nvTransform xform) {
    NV_TRACY_ZONE_START;

    nv_float min_x;
    nv_float min_y;
    nv_float max_x;
    nv_float max_y;
 
    phy_aabb aabb;

    // TODO: Do not inflate AABBs here.
    nv_float inflate = 0.00;

    switch (shape->type) {
        case nvShapeType_CIRCLE: {
            phy_vector2 c = nvVector2_add(nvVector2_rotate(shape->circle.center, xform.angle), xform.position);
            aabb = (phy_aabb){
                c.x - shape->circle.radius,
                c.y - shape->circle.radius,
                c.x + shape->circle.radius,
                c.y + shape->circle.radius
            };

            NV_TRACY_ZONE_END;
            return nvAABB_inflate(aabb, inflate);
        }
        case nvShapeType_POLYGON: {
            min_x = INFINITY;
            min_y = INFINITY;
            max_x = -INFINITY;
            max_y = -INFINITY;

            nvPolygon_transform(shape, xform);

            for (size_t i = 0; i < shape->polygon.num_vertices; i++) {
                phy_vector2 v = shape->polygon.xvertices[i];
                if (v.x < min_x) min_x = v.x;
                if (v.x > max_x) max_x = v.x;
                if (v.y < min_y) min_y = v.y;
                if (v.y > max_y) max_y = v.y;
            }

            aabb = (phy_aabb){min_x, min_y, max_x, max_y};

            NV_TRACY_ZONE_END;
            return nvAABB_inflate(aabb, inflate);
        }
        default:
            NV_TRACY_ZONE_END;
            return (phy_aabb){0.0, 0.0, 0.0, 0.0};
    }
}

nvShapeMassInfo nvShape_calculate_mass(nvShape *shape, nv_float density) {
    nv_float mass, inertia;

    switch (shape->type) {
        case nvShapeType_CIRCLE: {
            nvCircle circle = shape->circle;

            mass = nv_circle_area(circle.radius) * density;
            inertia = nv_circle_inertia(mass, circle.radius, circle.center);

            return (nvShapeMassInfo){mass, inertia, circle.center};
        }
        case nvShapeType_POLYGON: {
            nvPolygon polygon = shape->polygon;

            mass = nv_polygon_area(polygon.vertices, polygon.num_vertices) * density;
            inertia = nv_polygon_inertia(mass, polygon.vertices, polygon.num_vertices);
            phy_vector2 centroid = nv_polygon_centroid(polygon.vertices, polygon.num_vertices);

            return (nvShapeMassInfo){mass, inertia, centroid};
        }
        default:
            utl_error_func("Invalid shape", utl_user_defined_data);
            return (nvShapeMassInfo){-1.0, -1.0, NV_VECTOR2(-1.0, -1.0)};
    }
}

void nvPolygon_transform(nvShape *shape, nvTransform xform) {
    NV_TRACY_ZONE_START;

    for (size_t i = 0; i < shape->polygon.num_vertices; i++) {
        phy_vector2 new = nvVector2_add(xform.position,
            nvVector2_rotate(
                shape->polygon.vertices[i],
                xform.angle
                )
            );

        shape->polygon.xvertices[i] = new;
    }

    NV_TRACY_ZONE_END;
}
