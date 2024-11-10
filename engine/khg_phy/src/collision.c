/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#include "khg_phy/collision.h"
#include "khg_phy/core/phy_array.h"
#include "khg_phy/core/phy_constants.h"
#include "khg_phy/math.h"
#include "khg_phy/aabb.h"
#include <float.h>
#include <math.h>


/**
 * @file collision.c
 * 
 * @brief Collision detection and contact point generation functions.
 */


phy_persistent_contact_pair phy_collide_circle_x_circle(
    phy_shape *circle_a,
    phy_transform xform_a,
    phy_shape *circle_b,
    phy_transform xform_b
) {
    phy_persistent_contact_pair pcp = {
        .contact_count = 0,
        .normal = phy_vector2_zero
    };

    // Transform circle centers
    phy_vector2 ca = phy_vector2_add(phy_vector2_rotate(circle_a->circle.center, xform_a.angle), xform_a.position);
    phy_vector2 cb = phy_vector2_add(phy_vector2_rotate(circle_b->circle.center, xform_b.angle), xform_b.position);

    phy_vector2 delta = phy_vector2_sub(cb, ca);
    float dist = phy_vector2_len(delta);
    float radii = circle_a->circle.radius + circle_b->circle.radius;

    // Distance is over radii combined, not colliding
    if (dist > radii) return pcp;

    if (dist == 0.0)
        pcp.normal = PHY_DEGENERATE_NORMAL;
    else
        pcp.normal = phy_vector2_div(delta, dist);

    // Midway contact
    phy_vector2 a_support = phy_vector2_add(ca, phy_vector2_mul(pcp.normal, circle_a->circle.radius));
    phy_vector2 b_support = phy_vector2_add(cb, phy_vector2_mul(pcp.normal, -circle_b->circle.radius));
    phy_vector2 contact = phy_vector2_mul(phy_vector2_add(a_support, b_support), 0.5);

    pcp.contact_count = 1;
    pcp.contacts[0].separation = -(radii - dist);
    pcp.contacts[0].id = 0;
    pcp.contacts[0].anchor_a = phy_vector2_sub(contact, xform_a.position);
    pcp.contacts[0].anchor_b = phy_vector2_sub(contact, xform_b.position);
    pcp.contacts[0].is_persisted = false;
    pcp.contacts[0].remove_invoked = false;
    pcp.contacts[0].solver_info = phy_contact_solver_info_zero;

    return pcp;
}

bool phy_collide_circle_x_point(
    phy_shape *circle,
    phy_transform xform,
    phy_vector2 point
) {
    phy_vector2 c = phy_vector2_add(xform.position, phy_vector2_rotate(circle->circle.center, xform.angle));
    phy_vector2 delta = phy_vector2_sub(c, point);
    return phy_vector2_len2(delta) <= circle->circle.radius * circle->circle.radius;
}

/**
 * @brief Project circle onto axis and return extreme points.
 */
static inline void nv_project_circle(
    phy_vector2 center,
    float radius,
    phy_vector2 axis,
    float *min_out,
    float *max_out
) {
    phy_vector2 a = phy_vector2_mul(phy_vector2_normalize(axis), radius);

    phy_vector2 p1 = phy_vector2_add(center, a);
    phy_vector2 p2 = phy_vector2_sub(center, a);

    float min = phy_vector2_dot(p1, axis);
    float max = phy_vector2_dot(p2, axis);

    if (min > max) {
        float temp = max;
        max = min;
        min = temp;
    }

    *min_out = min;
    *max_out = max;
}

/**
 * @brief Project polygon onto axis and return extreme points.
 */
static inline void nv_project_polyon(
    phy_vector2 *vertices,
    size_t num_vertices,
    phy_vector2 axis,
    float *min_out,
    float *max_out
) {
    float min = INFINITY;
    float max = -INFINITY;

    for (size_t i = 0; i < num_vertices; i++) {
        float projection = phy_vector2_dot(vertices[i], axis);
        
        if (projection < min) min = projection;

        if (projection > max) max = projection;
    }

    *min_out = min;
    *max_out = max;
}

/**
 * @brief Find closest vertex of the polygon to the circle.
 */
static inline phy_vector2 nv_polygon_closest_vertex_to_circle(
    phy_vector2 center,
    phy_vector2 *vertices,
    size_t num_vertices
) {
    size_t closest = 0;
    float min_dist = INFINITY;
    
    for (size_t i = 0; i < num_vertices; i++) {
        float dist = phy_vector2_dist2(vertices[i], center);

        if (dist < min_dist) {
            min_dist = dist;
            closest = i;
        }
    }

    return vertices[closest];
}

/**
 * @brief Perpendicular distance between point and line segment.
 */
static inline void nv_point_segment_dist(
    phy_vector2 center,
    phy_vector2 a,
    phy_vector2 b,
    float *dist_out,
    phy_vector2 *contact_out
) {
    phy_vector2 ab = phy_vector2_sub(b, a);
    phy_vector2 ap = phy_vector2_sub(center, a);

    float projection = phy_vector2_dot(ap, ab);
    float ab_len = phy_vector2_len2(ab);
    float dist = projection / ab_len;
    phy_vector2 contact;

    if (dist <= 0.0) contact = a;

    else if (dist >= 1.0) contact = b;

    else contact = phy_vector2_add(a, phy_vector2_mul(ab, dist));

    *dist_out = phy_vector2_dist2(center, contact);
    *contact_out = contact;
}

phy_persistent_contact_pair phy_collide_polygon_x_circle(
    phy_shape *polygon,
    phy_transform xform_poly,
    phy_shape *circle,
    phy_transform xform_circle,
    bool flip_anchors
) {
    phy_polygon poly = polygon->polygon;
    phy_circle circ = circle->circle;
    phy_polygon_transform(polygon, xform_poly);
    phy_vector2 p = phy_polygon_centroid(poly.xvertices, poly.num_vertices);
    phy_vector2 c = phy_vector2_add(xform_circle.position, phy_vector2_rotate(circ.center, xform_circle.angle));
    size_t n = poly.num_vertices;
    phy_vector2 *vertices = poly.xvertices;
    float separation = INFINITY;
    phy_vector2 normal = phy_vector2_zero;

    phy_persistent_contact_pair pcp = {
        .contact_count = 0,
        .normal = phy_vector2_zero
    };

    float min_a, min_b, max_a, max_b;

    // Check each axes of polygon edges x circle

    for (size_t i = 0; i < n; i++) {
        phy_vector2 va = vertices[i];
        phy_vector2 vb = vertices[(i + 1) % n];

        phy_vector2 edge = phy_vector2_sub(vb, va);
        phy_vector2 axis = phy_vector2_normalize(phy_vector2_perp(edge));

        nv_project_polyon(vertices, n, axis, &min_a, &max_a);
        nv_project_circle(c, circ.radius, axis, &min_b, &max_b);

        // Doesn't collide
        if (min_a >= max_b || min_b >= max_a) {
            return pcp;
        }

        float axis_depth = fminf(max_b - min_a, max_a - min_b);

        if (axis_depth < separation) {
            separation = axis_depth;
            normal = axis;
        }
    }

    phy_vector2 cp = nv_polygon_closest_vertex_to_circle(c, vertices, n);
    phy_vector2 axis = phy_vector2_normalize(phy_vector2_sub(cp, c));

    nv_project_polyon(vertices, n, axis, &min_a, &max_a);
    nv_project_circle(c, circ.radius, axis, &min_b, &max_b);

    // Doesn't collide
    if (min_a >= max_b || min_b >= max_a) {
        return pcp;
    }

    float axis_depth = fminf(max_b - min_a, max_a - min_b);

    if (axis_depth < separation) {
        separation = axis_depth;
        normal = axis;
    }
    separation = -separation;

    // Flip normal
    if (phy_vector2_dot(phy_vector2_sub(p, c), normal) > 0.0) {
        normal = phy_vector2_neg(normal);
    }
    if (flip_anchors) {
        normal = phy_vector2_neg(normal);
    }

    // Get the contact on the closest edge
    float dist;
    float min_dist = INFINITY;
    phy_vector2 contact = phy_vector2_zero;
    phy_vector2 new_contact = phy_vector2_zero;
    for (size_t i = 0; i < n; i++) {
        phy_vector2 va = vertices[i];
        phy_vector2 vb = vertices[(i + 1) % n];

        nv_point_segment_dist(c, va, vb, &dist, &new_contact);

        if (dist < min_dist) {
            min_dist = dist;
            contact = new_contact;
        }
    }

    // Midpoint contact
    phy_vector2 circle_contact = phy_vector2_add(contact, phy_vector2_mul(normal, separation));
    phy_vector2 half_contact = phy_vector2_mul(phy_vector2_add(contact, circle_contact), 0.5);

    phy_vector2 poly_anchor = phy_vector2_sub(half_contact, xform_poly.position);
    phy_vector2 circle_anchor = phy_vector2_sub(half_contact, xform_circle.position);

    pcp.normal = normal;
    pcp.contact_count = 1;
    pcp.contacts[0].id = 0;
    pcp.contacts[0].is_persisted = false;
    pcp.contacts[0].remove_invoked = false;
    pcp.contacts[0].solver_info = phy_contact_solver_info_zero;
    pcp.contacts[0].separation = separation;

    if (flip_anchors) {
        pcp.contacts[0].anchor_a = circle_anchor;
        pcp.contacts[0].anchor_b = poly_anchor;
    }
    else {
        pcp.contacts[0].anchor_a = poly_anchor;
        pcp.contacts[0].anchor_b = circle_anchor;
    }

    return pcp;
}


static phy_persistent_contact_pair clip_polygons(
    phy_polygon a,
    phy_polygon b,
    int edge_a,
    int edge_b,
    bool flip
) {
    /*
        https://box2d.org/files/ErinCatto_ContactManifolds_GDC2007.pdf
        -
        Also see nv_collide_polygon_x_polygon for the reference.
    */

    // Reference polygon
    phy_polygon ref_polygon;
    int i11, i12;

    // Incident polygon
    phy_polygon inc_polygon;
    int i21, i22;

    if (flip) {
        ref_polygon = b;
        inc_polygon = a;
        i11 = edge_b;
        i12 = edge_b + 1 < b.num_vertices ? edge_b + 1 : 0;
		i21 = edge_a;
	    i22 = edge_a + 1 < a.num_vertices ? edge_a + 1 : 0;
    }
    else {
        ref_polygon = a;
		inc_polygon = b;
		i11 = edge_a;
		i12 = edge_a + 1 < a.num_vertices ? edge_a + 1 : 0;
		i21 = edge_b;
		i22 = edge_b + 1 < b.num_vertices ? edge_b + 1 : 0;
    }

    phy_vector2 normal = ref_polygon.normals[i11];
    phy_vector2 tangent = phy_vector2_perp(normal);

    // Reference edge vertices
    phy_vector2 v11 = ref_polygon.vertices[i11];
    phy_vector2 v12 = ref_polygon.vertices[i12];

    // Incident edge vertices
    phy_vector2 v21 = inc_polygon.vertices[i21];
    phy_vector2 v22 = inc_polygon.vertices[i22];

    float lower1 = 0.0;
    float upper1 = phy_vector2_dot(phy_vector2_sub(v12, v11), tangent);
    float upper2 = phy_vector2_dot(phy_vector2_sub(v21, v11), tangent);
    float lower2 = phy_vector2_dot(phy_vector2_sub(v22, v11), tangent);
    float d = upper2 - lower2;

    phy_vector2 v_lower;
    if (lower2 < lower1 && upper2 - lower2 > FLT_EPSILON)
        v_lower = phy_vector2_lerp(v22, v21, (lower1 - lower2) / d);
    else
        v_lower = v22;

    phy_vector2 v_upper;
    if (upper2 > upper1 && upper2 - lower2 > FLT_EPSILON)
        v_upper = phy_vector2_lerp(v22, v21, (upper1 - lower2) / d);
    else
        v_upper = v21;

    float separation_lower = phy_vector2_dot(phy_vector2_sub(v_lower, v11), normal);
    float separation_upper = phy_vector2_dot(phy_vector2_sub(v_upper, v11), normal);

    // Put contact points at midpoint
    float lower_mid_scale = -separation_lower * 0.5;
    float upper_mid_scale = -separation_upper * 0.5;
    v_lower = phy_vector2_new(
        v_lower.x + lower_mid_scale * normal.x,
        v_lower.y + lower_mid_scale * normal.y
    );
    v_upper = phy_vector2_new(
        v_upper.x + upper_mid_scale * normal.x,
        v_upper.y + upper_mid_scale * normal.y
    );

    phy_persistent_contact_pair pcp;

    if (!flip) {
        pcp.normal = normal;

        pcp.contacts[0].anchor_a = v_lower;
        pcp.contacts[0].separation = separation_lower;
        pcp.contacts[0].id = phy_u32_pair(i11, i22);

        pcp.contacts[1].anchor_a = v_upper;
        pcp.contacts[1].separation = separation_upper;
        pcp.contacts[1].id = phy_u32_pair(i12, i21);

        pcp.contact_count = 2;
    }
    else {
        pcp.normal = phy_vector2_neg(normal);

        pcp.contacts[0].anchor_a = v_upper;
        pcp.contacts[0].separation = separation_upper;
        pcp.contacts[0].id = phy_u32_pair(i21, i12);

        pcp.contacts[1].anchor_a = v_lower;
        pcp.contacts[1].separation = separation_lower;
        pcp.contacts[1].id = phy_u32_pair(i22, i11);

        pcp.contact_count = 2;
    }

    return pcp;
}

static void find_max_separation(
    int *edge,
    float *separation,
    phy_polygon a,
    phy_polygon b
) {
    /*
        Find the max separation between two polygons using edge normals of first polygon.
        See nv_collide_polygon_x_polygon for the reference.
    */

    int best_index = 0;
    float max_separation = -INFINITY;

    for (int i = 0; i < a.num_vertices; i++) {
        phy_vector2 n = a.normals[i];
        phy_vector2 v1 = a.vertices[i];

        float si = INFINITY;
        for (int j = 0; j < b.num_vertices; j++) {
            float sij = phy_vector2_dot(n, phy_vector2_sub(b.vertices[j], v1));
            if (sij < si)
                si = sij;
        }

        if (si > max_separation) {
            max_separation = si;
            best_index = i;
        }
    }

    *edge = best_index;
    *separation = max_separation;
}

static phy_persistent_contact_pair SAT(phy_polygon a, phy_polygon b) {
    /*
        See nv_collide_polygon_x_polygon for the reference.
    */

    phy_persistent_contact_pair pcp;
    pcp.contact_count = 0;
    pcp.normal = phy_vector2_zero;

    int edge_a = 0;
    float separation_a;
    find_max_separation(&edge_a, &separation_a, a, b);

    int edge_b = 0;
    float separation_b;
    find_max_separation(&edge_b, &separation_b, b, a);

    // Shapes are only overlapping if both separations are negative
    if (separation_a > 0.0 || separation_b > 0.0) return pcp;

    bool flip;

    if (separation_b > separation_a) {
        flip = true;
        phy_vector2 search_dir = b.normals[edge_b];
        float min_dot = INFINITY;
        edge_a = 0;

        // Find the incident edge on polygon A
        for (int i = 0; i < a.num_vertices; i++) {
            float dot = phy_vector2_dot(search_dir, a.normals[i]);
            if (dot < min_dot) {
                min_dot = dot;
                edge_a = i;
            }
        }
    }
    else {
        flip = false;
        phy_vector2 search_dir = a.normals[edge_a];
        float min_dot = INFINITY;
        edge_b = 0;

        // Find the incident edge on polygon B
        for (int i = 0; i < b.num_vertices; i++) {
            float dot = phy_vector2_dot(search_dir, b.normals[i]);
            if (dot < min_dot) {
                min_dot = dot;
                edge_b = i;
            }
        }
    }

    return clip_polygons(a, b, edge_a, edge_b, flip);
}

phy_persistent_contact_pair phy_collide_polygon_x_polygon(
    phy_shape *polygon_a,
    phy_transform xform_a,
    phy_shape *polygon_b,
    phy_transform xform_b
) {
    /*
        Box2D V3's one-shot contact point generation algorithm for convex polygons.
        https://github.com/erincatto/box2c/blob/main/src/manifold.c

        Corner rounding and GJK is not included, Nova only uses SAT.
    */

    // TODO: Number of trig calls could definitely be lowered

    phy_polygon a = polygon_a->polygon;
    phy_polygon b = polygon_b->polygon;

    phy_vector2 origin = a.vertices[0];

    // Shift polygon A to origin
    phy_transform xform_a_translated = {
        phy_vector2_add(xform_a.position, phy_vector2_rotate(origin, xform_a.angle)),
        xform_a.angle
    };
    // Inverse multiply transforms
    phy_transform xform;
    {
        float sa = sinf(xform_a_translated.angle);
        float ca = cosf(xform_a_translated.angle);
        float sb = sinf(xform_b.angle);
        float cb = cosf(xform_b.angle);

        // Inverse rotate
        phy_vector2 d = phy_vector2_sub(xform_b.position, xform_a_translated.position);
        phy_vector2 p = phy_vector2_new(ca * d.x + sa * d.y, -sa * d.x + ca * d.y);

        // Inverse multiply rotations
        float is = ca * sb - sa * cb;
        float ic = ca * cb + sa * sb;
        float ia = atan2f(is, ic);

        xform = (phy_transform){p, ia};
    }

    phy_polygon a_local;
    a_local.num_vertices = a.num_vertices;
    a_local.vertices[0] = phy_vector2_zero;
    a_local.normals[0] = a.normals[0];
    for (size_t i = 1; i < a_local.num_vertices; i++) {
        a_local.vertices[i] = phy_vector2_sub(a.vertices[i], origin);
        a_local.normals[i] = a.normals[i];
    }

    phy_polygon b_local;
    b_local.num_vertices = b.num_vertices;
    for (size_t i = 0; i < b_local.num_vertices; i++) {
        phy_vector2 xv = phy_vector2_add(phy_vector2_rotate(b.vertices[i], xform.angle), xform.position);

        b_local.vertices[i] = xv;
        b_local.normals[i] = phy_vector2_rotate(b.normals[i], xform.angle);
    }

    phy_persistent_contact_pair pcp = SAT(a_local, b_local);

    if (pcp.contact_count > 0) {
        pcp.normal = phy_vector2_rotate(pcp.normal, xform_a.angle);

        for (size_t i = 0; i < pcp.contact_count; i++) {
            phy_contact *contact = &pcp.contacts[i];

            contact->anchor_a = phy_vector2_rotate(
                phy_vector2_add(contact->anchor_a, origin), xform_a.angle);
            contact->anchor_b = phy_vector2_add(
                contact->anchor_a, phy_vector2_sub(xform_a.position, xform_b.position));
            contact->is_persisted = false;
            contact->remove_invoked = false;

            contact->solver_info = phy_contact_solver_info_zero;
        }
    }

    return pcp;
}

bool phy_collide_polygon_x_point(
    phy_shape *polygon,
    phy_transform xform,
    phy_vector2 point
) {
    /*
        Algorithm from "Real-Time Collision Detection", Christer Ericson
        Chapter 5, Page 202
    */

    phy_polygon_transform(polygon, xform);
    phy_vector2 *vertices = polygon->polygon.xvertices;
    size_t n = polygon->polygon.num_vertices;
    
    // Do binary search over polygon vertices to find the fan triangle
    // (v[0], v[low], v[high]) the point p lies within the near sides of
    int low = 0;
    int high = (int)n;
    do {
        int mid = (low + high) / 2;
        if (phy_triangle_winding((phy_vector2[3]){vertices[0], vertices[mid], point}) == 1)
            low = mid;
        else
            high = mid;
    } while (low + 1 < high);

    // If point outside last (or first) edge, then it is not inside the n-gon
    if (low == 0 || high == n) return false;

    // p is inside the polygon if it is left of
    // the directed edge from v[low] to v[high]
    return phy_triangle_winding((phy_vector2[3]){vertices[low], vertices[high], point}) == 1;
}


bool phy_collide_aabb_x_aabb(phy_aabb a, phy_aabb b) {
    return (!(a.max_x <= b.min_x || b.max_x <= a.min_x ||
              a.max_y <= b.min_y || b.max_y <= a.min_y));
}

bool phy_collide_aabb_x_point(phy_aabb aabb, phy_vector2 point) {
    return (aabb.min_x <= point.x && point.x <= aabb.max_x &&
            aabb.min_y <= point.y && point.y <= aabb.max_y);
}


bool phy_collide_ray_x_circle(
    phy_raycast_result *result,
    phy_vector2 origin,
    phy_vector2 dir,
    float maxsq,
    phy_shape *shape,
    phy_transform xform
) {
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html

    phy_circle circle = shape->circle;
    phy_vector2 center = phy_vector2_add(phy_vector2_rotate(circle.center, xform.angle), xform.position);
    float rsq = circle.radius * circle.radius;
    phy_vector2 delta = phy_vector2_sub(center, origin);

    float tca = phy_vector2_dot(delta, dir);
    float d2 = phy_vector2_dot(delta, delta) - tca * tca;
    if (d2 > rsq) return false;
    float thc = sqrtf(rsq - d2);
    float t0 = tca - thc;
    float t1 = tca + thc;
    
    if (t0 > t1) {
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }

    if (t0 < 0.0) {
        t0 = t1;
        if (t0 < 0.0) return false; // Intersection behind ray origin
    }

    float t = t0;

    phy_vector2 hitpoint = phy_vector2_add(origin, phy_vector2_mul(dir, t));

    // Out of ray's range
    if (phy_vector2_len2(phy_vector2_sub(hitpoint, origin)) > maxsq) return false;

    *result = (phy_raycast_result){
        .position = hitpoint,
        .normal = phy_vector2_normalize(phy_vector2_sub(hitpoint, center)),
        .shape = shape
    };
    return true;
}

bool phy_collide_ray_x_polygon(
    phy_raycast_result *result,
    phy_vector2 origin,
    phy_vector2 dir,
    float maxsq,
    phy_shape *shape,
    phy_transform xform
) {
    // https://rootllama.wordpress.com/2014/06/20/ray-line-segment-intersection-test-in-2d/
    // https://stackoverflow.com/a/29020182

    phy_polygon poly = shape->polygon;

    phy_vector2 hits[PHY_POLYGON_MAX_VERTICES];
    size_t normal_idxs[PHY_POLYGON_MAX_VERTICES];
    size_t hit_count = 0;

    phy_polygon_transform(shape, xform);
    for (size_t i = 0; i < poly.num_vertices; i++) {
        phy_vector2 va = poly.xvertices[i];
        phy_vector2 vb = poly.xvertices[(i + 1) % poly.num_vertices];

        phy_vector2 v1 = phy_vector2_sub(origin, va);
        phy_vector2 v2 = phy_vector2_sub(vb, va);
        phy_vector2 v3 = phy_vector2_perp(dir);

        float dot = phy_vector2_dot(v2, v3);
        if (fabsf(dot) < FLT_EPSILON) continue;;

        float t1 = phy_vector2_cross(v2, v1) / dot;
        float t2 = phy_vector2_dot(v1, v3) / dot;

        if (t1 >= 0.0 && (t2 >= 0.0 && t2 <= 1.0)) {
            hits[hit_count++] = phy_vector2_add(origin, phy_vector2_mul(dir, t1));
            normal_idxs[hit_count - 1] = i;
        }
    }

    if (hit_count == 0) return false;

    phy_vector2 closest_hit;
    phy_vector2 normal = phy_vector2_zero;
    float min_dist = INFINITY;
    for (size_t i = 0; i < hit_count; i++) {
        float dist = phy_vector2_len2(phy_vector2_sub(hits[i], origin));
        if (dist < min_dist) {
            min_dist = dist;
            closest_hit = hits[i];
            normal = poly.normals[normal_idxs[i]];
        }
    }

    // Out of ray's range
    if (min_dist > maxsq) return false;

    *result = (phy_raycast_result){
        .position = closest_hit,
        .normal = phy_vector2_rotate(normal, xform.angle),
        .shape = shape
    };
    return true;
}
