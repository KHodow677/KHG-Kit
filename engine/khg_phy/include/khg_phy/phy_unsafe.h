#pragma once

#include "khg_phy/phy_types.h"

void phy_circle_shape_set_radius(phy_shape *shape, float radius);
void phy_circle_shape_set_offset(phy_shape *shape, phy_vect offset);

void phy_segment_shape_set_endpoints(phy_shape *shape, phy_vect a, phy_vect b);
void phy_segment_shape_set_radius(phy_shape *shape, float radius);

void phy_poly_shape_set_verts(phy_shape *shape, int count, phy_vect *verts, phy_transform transform);
void phy_poly_shape_set_verts_raw(phy_shape *shape, int count, phy_vect *verts);

void phy_poly_shape_set_radius(phy_shape *shape, float radius);

