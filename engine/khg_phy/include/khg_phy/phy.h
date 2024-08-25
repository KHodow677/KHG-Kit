#pragma once

#include "khg_phy/bb.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include <stdlib.h>

float phy_moment_for_circle(float m, float r1, float r2, phy_vect offset);
float phy_area_for_circle(float r1, float r2);

float phy_moment_for_segment(float m, phy_vect a, phy_vect b, float radius);
float phy_area_for_segment(phy_vect a, phy_vect b, float radius);

float phy_moment_for_poly(float m, int count, const phy_vect *verts, phy_vect offset, float radius);
float phy_area_for_poly(const int count, const phy_vect *verts, float radius);

phy_vect phy_centroid_for_poly(const int count, const phy_vect *verts);
float phy_moment_for_box(float m, float width, float height);
float phy_moment_for_box_2(float m, phy_bb box);

int phy_convex_hull(int count, const phy_vect *verts, phy_vect *result, int *first, float tol);
#define PHY_CONVEX_HULL(count, verts, count_var, verts_var)\
  phy_vect *verts_var = (phy_vect *)alloca(count * sizeof(phy_vect));\
  int count_var = phy_convex_hull(count, verts, verts_var, NULL, 0.0);\

static inline phy_vect phy_closest_point_on_segment(const phy_vect p, const phy_vect a, const phy_vect b) {
	phy_vect delta = phy_v_sub(a, b);
	float t = phy_clamp_01(phy_v_dot(delta, phy_v_sub(p, b))/phy_v_length_sq(delta));
	return phy_v_add(b, phy_v_mult(delta, t));
}

