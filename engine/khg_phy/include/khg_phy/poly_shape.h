#pragma once

#include "khg_phy/phy_types.h"
#include "khg_phy/bb.h"

/// Allocate a polygon shape.
phy_poly_shape* cpPolyShapeAlloc(void);
/// Initialize a polygon shape with rounded corners.
/// A convex hull will be created from the vertexes.
phy_poly_shape* cpPolyShapeInit(phy_poly_shape *poly, phy_body *body, int count, const phy_vect *verts, phy_transform transform, float radius);
/// Initialize a polygon shape with rounded corners.
/// The vertexes must be convex with a counter-clockwise winding.
phy_poly_shape* cpPolyShapeInitRaw(phy_poly_shape *poly, phy_body *body, int count, const phy_vect *verts, float radius);
/// Allocate and initialize a polygon shape with rounded corners.
/// A convex hull will be created from the vertexes.
phy_shape* cpPolyShapeNew(phy_body *body, int count, const phy_vect *verts, phy_transform transform, float radius);
/// Allocate and initialize a polygon shape with rounded corners.
/// The vertexes must be convex with a counter-clockwise winding.
phy_shape* cpPolyShapeNewRaw(phy_body *body, int count, const phy_vect *verts, float radius);

/// Initialize a box shaped polygon shape with rounded corners.
phy_poly_shape* cpBoxShapeInit(phy_poly_shape *poly, phy_body *body, float width, float height, float radius);
/// Initialize an offset box shaped polygon shape with rounded corners.
phy_poly_shape* cpBoxShapeInit2(phy_poly_shape *poly, phy_body *body, phy_bb box, float radius);
/// Allocate and initialize a box shaped polygon shape.
phy_shape* cpBoxShapeNew(phy_body *body, float width, float height, float radius);
/// Allocate and initialize an offset box shaped polygon shape.
phy_shape* cpBoxShapeNew2(phy_body *body, phy_bb box, float radius);

/// Get the number of verts in a polygon shape.
int cpPolyShapeGetCount(const phy_shape *shape);
/// Get the @c ith vertex of a polygon shape.
phy_vect cpPolyShapeGetVert(const phy_shape *shape, int index);
/// Get the radius of a polygon shape.
float cpPolyShapeGetRadius(const phy_shape *shape);

/// @}
