#pragma once

#include "khg_phy/phy_types.h"

typedef struct phy_polyline {
  int count, capacity;
  phy_vect verts[];
} phy_polyline;

void phy_polyline_free(phy_polyline *line);
bool phy_polyline_is_closed(phy_polyline *line);

phy_polyline *phy_polyline_simplify_curves(phy_polyline *line, float tol);
phy_polyline *phy_polyline_simplify_vertices(phy_polyline *line, float tol);
phy_polyline *phy_polyline_to_convex_hull(phy_polyline *line, float tol);

typedef struct phy_polyline_set {
  int count, capacity;
  phy_polyline **lines;
} phy_polyline_set;

phy_polyline_set *phy_polyline_set_alloc(void);
phy_polyline_set *phy_polyline_set_new(void);
phy_polyline_set *phy_polyline_set_init(phy_polyline_set *set);

void phy_polyline_set_destroy(phy_polyline_set *set, bool free_polylines);
void phy_polyline_set_free(phy_polyline_set *set, bool free_polylines);

void phy_polyline_set_collect_segment(phy_vect v0, phy_vect v1, phy_polyline_set *lines);
phy_polyline_set *phy_polyline_convex_decomposition(phy_polyline *line, float tol);

