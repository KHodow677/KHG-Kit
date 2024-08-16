/* Copyright (c) 2013 Scott Lembcke and Howling Moon Software
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef CHIPMUNK_H
#define CHIPMUNK_H

#include <stdlib.h>
#include <math.h>

#include "khg_phy/phy_types.h"

#ifdef __cplusplus
extern "C" {
#endif

CP_EXPORT void cpMessage(const char *condition, const char *file, int line, int isError, int isHardError, const char *message, ...);
#ifdef NDEBUG
	#define	cpAssertWarn(__condition__, ...)
	#define	cpAssertSoft(__condition__, ...)
#else
	#define cpAssertSoft(__condition__, ...) if(!(__condition__)){cpMessage(#__condition__, __FILE__, __LINE__, 1, 0, __VA_ARGS__); abort();}
	#define cpAssertWarn(__condition__, ...) if(!(__condition__)) cpMessage(#__condition__, __FILE__, __LINE__, 0, 0, __VA_ARGS__)
#endif

// Hard assertions are used in situations where the program definitely will crash anyway, and the reason is inexpensive to detect.
#define cpAssertHard(__condition__, ...) if(!(__condition__)){cpMessage(#__condition__, __FILE__, __LINE__, 1, 1, __VA_ARGS__); abort();}


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

// Chipmunk 7.0.3
#define CP_VERSION_MAJOR 7
#define CP_VERSION_MINOR 0
#define CP_VERSION_RELEASE 3

/// Version string.
CP_EXPORT extern const char *cpVersionString;

/// Calculate the moment of inertia for a circle.
/// @c r1 and @c r2 are the inner and outer diameters. A solid circle has an inner diameter of 0.
CP_EXPORT float cpMomentForCircle(float m, float r1, float r2, phy_vect offset);

/// Calculate area of a hollow circle.
/// @c r1 and @c r2 are the inner and outer diameters. A solid circle has an inner diameter of 0.
CP_EXPORT float cpAreaForCircle(float r1, float r2);

/// Calculate the moment of inertia for a line segment.
/// Beveling radius is not supported.
CP_EXPORT float cpMomentForSegment(float m, phy_vect a, phy_vect b, float radius);

/// Calculate the area of a fattened (capsule shaped) line segment.
CP_EXPORT float cpAreaForSegment(phy_vect a, phy_vect b, float radius);

/// Calculate the moment of inertia for a solid polygon shape assuming it's center of gravity is at it's centroid. The offset is added to each vertex.
CP_EXPORT float cpMomentForPoly(float m, int count, const phy_vect *verts, phy_vect offset, float radius);

/// Calculate the signed area of a polygon. A Clockwise winding gives positive area.
/// This is probably backwards from what you expect, but matches Chipmunk's the winding for poly shapes.
CP_EXPORT float cpAreaForPoly(const int count, const phy_vect *verts, float radius);

/// Calculate the natural centroid of a polygon.
CP_EXPORT phy_vect cpCentroidForPoly(const int count, const phy_vect *verts);

/// Calculate the moment of inertia for a solid box.
CP_EXPORT float cpMomentForBox(float m, float width, float height);

/// Calculate the moment of inertia for a solid box.
CP_EXPORT float cpMomentForBox2(float m, cpBB box);

/// Calculate the convex hull of a given set of points. Returns the count of points in the hull.
/// @c result must be a pointer to a @c phy_vect array with at least @c count elements. If @c verts == @c result, then @c verts will be reduced inplace.
/// @c first is an optional pointer to an integer to store where the first vertex in the hull came from (i.e. verts[first] == result[0])
/// @c tol is the allowed amount to shrink the hull when simplifying it. A tolerance of 0.0 creates an exact hull.
CP_EXPORT int cpConvexHull(int count, const phy_vect *verts, phy_vect *result, int *first, float tol);

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

#if defined(__has_extension)
#if __has_extension(blocks)
// Define alternate block based alternatives for a few of the callback heavy functions.
// Collision handlers are post-step callbacks are not included to avoid memory management issues.
// If you want to use blocks for those and are aware of how to correctly manage the memory, the implementation is trivial. 

void cpSpaceEachBody_b(cpSpace *space, void (^block)(cpBody *body));
void cpSpaceEachShape_b(cpSpace *space, void (^block)(cpShape *shape));
void cpSpaceEachConstraint_b(cpSpace *space, void (^block)(cpConstraint *constraint));

void cpBodyEachShape_b(cpBody *body, void (^block)(cpShape *shape));
void cpBodyEachConstraint_b(cpBody *body, void (^block)(cpConstraint *constraint));
void cpBodyEachArbiter_b(cpBody *body, void (^block)(cpArbiter *arbiter));

typedef void (^cpSpacePointQueryBlock)(cpShape *shape, phy_vect point, float distance, phy_vect gradient);
void cpSpacePointQuery_b(cpSpace *space, phy_vect point, float maxDistance, cpShapeFilter filter, cpSpacePointQueryBlock block);

typedef void (^cpSpaceSegmentQueryBlock)(cpShape *shape, phy_vect point, phy_vect normal, float alpha);
void cpSpaceSegmentQuery_b(cpSpace *space, phy_vect start, phy_vect end, float radius, cpShapeFilter filter, cpSpaceSegmentQueryBlock block);

typedef void (^cpSpaceBBQueryBlock)(cpShape *shape);
void cpSpaceBBQuery_b(cpSpace *space, cpBB bb, cpShapeFilter filter, cpSpaceBBQueryBlock block);

typedef void (^cpSpaceShapeQueryBlock)(cpShape *shape, cpContactPointSet *points);
bool cpSpaceShapeQuery_b(cpSpace *space, cpShape *shape, cpSpaceShapeQueryBlock block);

#endif
#endif


//@}

#ifdef __cplusplus
}

static inline phy_vect operator *(const phy_vect v, const float s){return cpvmult(v, s);}
static inline phy_vect operator +(const phy_vect v1, const phy_vect v2){return cpvadd(v1, v2);}
static inline phy_vect operator -(const phy_vect v1, const phy_vect v2){return cpvsub(v1, v2);}
static inline bool operator ==(const phy_vect v1, const phy_vect v2){return cpveql(v1, v2);}
static inline phy_vect operator -(const phy_vect v){return cpvneg(v);}

#endif
#endif
