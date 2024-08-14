#pragma once

#include "khg_phy/phy_types.h"
#include "khg_phy/bb.h"

/// Allocate a polygon shape.
CP_EXPORT cpPolyShape* cpPolyShapeAlloc(void);
/// Initialize a polygon shape with rounded corners.
/// A convex hull will be created from the vertexes.
CP_EXPORT cpPolyShape* cpPolyShapeInit(cpPolyShape *poly, cpBody *body, int count, const cpVect *verts, cpTransform transform, float radius);
/// Initialize a polygon shape with rounded corners.
/// The vertexes must be convex with a counter-clockwise winding.
CP_EXPORT cpPolyShape* cpPolyShapeInitRaw(cpPolyShape *poly, cpBody *body, int count, const cpVect *verts, float radius);
/// Allocate and initialize a polygon shape with rounded corners.
/// A convex hull will be created from the vertexes.
CP_EXPORT cpShape* cpPolyShapeNew(cpBody *body, int count, const cpVect *verts, cpTransform transform, float radius);
/// Allocate and initialize a polygon shape with rounded corners.
/// The vertexes must be convex with a counter-clockwise winding.
CP_EXPORT cpShape* cpPolyShapeNewRaw(cpBody *body, int count, const cpVect *verts, float radius);

/// Initialize a box shaped polygon shape with rounded corners.
CP_EXPORT cpPolyShape* cpBoxShapeInit(cpPolyShape *poly, cpBody *body, float width, float height, float radius);
/// Initialize an offset box shaped polygon shape with rounded corners.
CP_EXPORT cpPolyShape* cpBoxShapeInit2(cpPolyShape *poly, cpBody *body, cpBB box, float radius);
/// Allocate and initialize a box shaped polygon shape.
CP_EXPORT cpShape* cpBoxShapeNew(cpBody *body, float width, float height, float radius);
/// Allocate and initialize an offset box shaped polygon shape.
CP_EXPORT cpShape* cpBoxShapeNew2(cpBody *body, cpBB box, float radius);

/// Get the number of verts in a polygon shape.
CP_EXPORT int cpPolyShapeGetCount(const cpShape *shape);
/// Get the @c ith vertex of a polygon shape.
CP_EXPORT cpVect cpPolyShapeGetVert(const cpShape *shape, int index);
/// Get the radius of a polygon shape.
CP_EXPORT float cpPolyShapeGetRadius(const cpShape *shape);

/// @}
