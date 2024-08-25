#pragma once

#include "khg_phy/phy_types.h"
#include "khg_phy/bb.h"

phy_poly_shape *phy_poly_shape_alloc(void);
phy_shape *phy_poly_shape_new(phy_body *body, int count, const phy_vect *verts, phy_transform transform, float radius);
phy_shape *phy_poly_shape_new_raw(phy_body *body, int count, const phy_vect *verts, float radius);
phy_poly_shape *phy_poly_shape_init(phy_poly_shape *poly, phy_body *body, int count, const phy_vect *verts, phy_transform transform, float radius);
phy_poly_shape *phy_poly_shape_init_raw(phy_poly_shape *poly, phy_body *body, int count, const phy_vect *verts, float radius);

phy_shape *phy_box_shape_new(phy_body *body, float width, float height, float radius);
phy_shape *phy_box_shape_new_2(phy_body *body, phy_bb box, float radius);
phy_poly_shape *phy_box_shape_init(phy_poly_shape *poly, phy_body *body, float width, float height, float radius);
phy_poly_shape *phy_box_shape_init_2(phy_poly_shape *poly, phy_body *body, phy_bb box, float radius);

int phy_poly_shape_get_count(const phy_shape *shape);
phy_vect phy_poly_shape_get_vert(const phy_shape *shape, int index);
float phy_poly_shape_get_radius(const phy_shape *shape);

