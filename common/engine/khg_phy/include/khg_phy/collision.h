#pragma once

#include "khg_phy/contact.h"
#include <stdbool.h>

#define PHY_DEGENERATE_NORMAL phy_vector2_new(0.0, 1.0)

typedef struct phy_raycast_result {
  phy_vector2 position;
  phy_vector2 normal;
  phy_rigid_body *body;
  phy_shape *shape;
} phy_raycast_result;

phy_persistent_contact_pair phy_collide_circle_x_circle(phy_shape *circle_a, phy_transform xform_a, phy_shape *circle_b, phy_transform xform_b);
bool phy_collide_circle_x_point(phy_shape *circle, phy_transform xform, phy_vector2 point);

phy_persistent_contact_pair phy_collide_polygon_x_circle(phy_shape *polygon, phy_transform xform_poly, phy_shape *circle, phy_transform xform_circle, bool flip_anchors);
phy_persistent_contact_pair phy_collide_polygon_x_polygon(phy_shape *polygon_a, phy_transform xform_a, phy_shape *polygon_b, phy_transform xform_b);
bool phy_collide_polygon_x_point(phy_shape *polygon, phy_transform xform, phy_vector2 point);

bool phy_collide_aabb_x_aabb(phy_aabb a, phy_aabb b);
bool phy_collide_aabb_x_point(phy_aabb aabb, phy_vector2 point);

bool phy_collide_ray_x_circle(phy_raycast_result *result, phy_vector2 origin, phy_vector2 dir, float maxsq, phy_shape *shape, phy_transform xform);
bool phy_collide_ray_x_polygon(phy_raycast_result *result, phy_vector2 origin, phy_vector2 dir, float maxsq, phy_shape *shape, phy_transform xform);

