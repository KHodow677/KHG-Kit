#pragma once

#include <stdlib.h>
#include <math.h>

#include "khg_phy/phy_types.h"

#include "khg_phy/vect.h"
#include "khg_phy/bb.h"
#include "khg_phy/transform.h"
#include "khg_phy/spatial_index.h"
#include "khg_phy/arbiter.h"	
#include "khg_phy/body.h"
#include "khg_phy/shape.h"
#include "khg_phy/poly_shape.h"
#include "khg_phy/constraint.h"
#include "khg_phy/space.h"

#define CP_VERSION_MAJOR 7
#define CP_VERSION_MINOR 0
#define CP_VERSION_RELEASE 3

/// Version string.
extern const char *cpVersionString;

/// Calculate the moment of inertia for a circle.
/// @c r1 and @c r2 are the inner and outer diameters. A solid circle has an inner diameter of 0.
float cpMomentForCircle(float m, float r1, float r2, phy_vect offset);

/// Calculate area of a hollow circle.
/// @c r1 and @c r2 are the inner and outer diameters. A solid circle has an inner diameter of 0.
float cpAreaForCircle(float r1, float r2);

/// Calculate the moment of inertia for a line segment.
/// Beveling radius is not supported.
float cpMomentForSegment(float m, phy_vect a, phy_vect b, float radius);

/// Calculate the area of a fattened (capsule shaped) line segment.
float cpAreaForSegment(phy_vect a, phy_vect b, float radius);

/// Calculate the moment of inertia for a solid polygon shape assuming it's center of gravity is at it's centroid. The offset is added to each vertex.
float cpMomentForPoly(float m, int count, const phy_vect *verts, phy_vect offset, float radius);

/// Calculate the signed area of a polygon. A Clockwise winding gives positive area.
/// This is probably backwards from what you expect, but matches Chipmunk's the winding for poly shapes.
float cpAreaForPoly(const int count, const phy_vect *verts, float radius);

/// Calculate the natural centroid of a polygon.
phy_vect cpCentroidForPoly(const int count, const phy_vect *verts);

/// Calculate the moment of inertia for a solid box.
float cpMomentForBox(float m, float width, float height);

/// Calculate the moment of inertia for a solid box.
float cpMomentForBox2(float m, phy_bb box);

/// Calculate the convex hull of a given set of points. Returns the count of points in the hull.
/// @c result must be a pointer to a @c phy_vect array with at least @c count elements. If @c verts == @c result, then @c verts will be reduced inplace.
/// @c first is an optional pointer to an integer to store where the first vertex in the hull came from (i.e. verts[first] == result[0])
/// @c tol is the allowed amount to shrink the hull when simplifying it. A tolerance of 0.0 creates an exact hull.
int cpConvexHull(int count, const phy_vect *verts, phy_vect *result, int *first, float tol);

/// Convenience macro to work with cpConvexHull.
/// @c count and @c verts is the input array passed to cpConvexHull().
/// @c count_var and @c verts_var are the names of the variables the macro creates to store the result.
/// The output vertex array is allocated on the stack using alloca() so it will be freed automatically, but cannot be returned from the current scope.
#define CP_CONVEX_HULL(__count__, __verts__, __count_var__, __verts_var__) \
phy_vect *__verts_var__ = (phy_vect *)alloca(__count__*sizeof(phy_vect)); \
int __count_var__ = cpConvexHull(__count__, __verts__, __verts_var__, NULL, 0.0); \

/// Returns the closest point on the line segment ab, to the point p.
static inline phy_vect
cpClosetPointOnSegment(const phy_vect p, const phy_vect a, const phy_vect b)
{
	phy_vect delta = cpvsub(a, b);
	float t = phy_clamp01(cpvdot(delta, cpvsub(p, b))/cpvlengthsq(delta));
	return cpvadd(b, cpvmult(delta, t));
}

