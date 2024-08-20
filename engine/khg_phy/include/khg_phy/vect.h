#pragma once

#include "khg_phy/phy_types.h"
#include <math.h>

/// @defgroup phy_vect phy_vect
/// Chipmunk's 2D vector type along with a handy 2D vector math lib.
/// @{

/// Constant for the zero vector.
static const phy_vect cpvzero = {0.0f,0.0f};

/// Convenience constructor for phy_vect structs.
static inline phy_vect cpv(const float x, const float y)
{
	phy_vect v = {x, y};
	return v;
}

/// Check if two vectors are equal. (Be careful when comparing floating point numbers!)
static inline bool cpveql(const phy_vect v1, const phy_vect v2)
{
	return (v1.x == v2.x && v1.y == v2.y);
}

/// Add two vectors
static inline phy_vect cpvadd(const phy_vect v1, const phy_vect v2)
{
	return cpv(v1.x + v2.x, v1.y + v2.y);
}

/// Subtract two vectors.
static inline phy_vect cpvsub(const phy_vect v1, const phy_vect v2)
{
	return cpv(v1.x - v2.x, v1.y - v2.y);
}

/// Negate a vector.
static inline phy_vect cpvneg(const phy_vect v)
{
	return cpv(-v.x, -v.y);
}

/// Scalar multiplication.
static inline phy_vect cpvmult(const phy_vect v, const float s)
{
	return cpv(v.x*s, v.y*s);
}

/// Vector dot product.
static inline float cpvdot(const phy_vect v1, const phy_vect v2)
{
	return v1.x*v2.x + v1.y*v2.y;
}

/// 2D vector cross product analog.
/// The cross product of 2D vectors results in a 3D vector with only a z component.
/// This function returns the magnitude of the z value.
static inline float cpvcross(const phy_vect v1, const phy_vect v2)
{
	return v1.x*v2.y - v1.y*v2.x;
}

/// Returns a perpendicular vector. (90 degree rotation)
static inline phy_vect cpvperp(const phy_vect v)
{
	return cpv(-v.y, v.x);
}

/// Returns a perpendicular vector. (-90 degree rotation)
static inline phy_vect cpvrperp(const phy_vect v)
{
	return cpv(v.y, -v.x);
}

/// Returns the vector projection of v1 onto v2.
static inline phy_vect cpvproject(const phy_vect v1, const phy_vect v2)
{
	return cpvmult(v2, cpvdot(v1, v2)/cpvdot(v2, v2));
}

/// Returns the unit length vector for the given angle (in radians).
static inline phy_vect cpvforangle(const float a)
{
	return cpv(cosf(a), sinf(a));
}

/// Returns the angular direction v is pointing in (in radians).
static inline float cpvtoangle(const phy_vect v)
{
	return atan2f(v.y, v.x);
}

/// Uses complex number multiplication to rotate v1 by v2. Scaling will occur if v1 is not a unit vector.
static inline phy_vect cpvrotate(const phy_vect v1, const phy_vect v2)
{
	return cpv(v1.x*v2.x - v1.y*v2.y, v1.x*v2.y + v1.y*v2.x);
}

/// Inverse of cpvrotate().
static inline phy_vect cpvunrotate(const phy_vect v1, const phy_vect v2)
{
	return cpv(v1.x*v2.x + v1.y*v2.y, v1.y*v2.x - v1.x*v2.y);
}

/// Returns the squared length of v. Faster than cpvlength() when you only need to compare lengths.
static inline float cpvlengthsq(const phy_vect v)
{
	return cpvdot(v, v);
}

/// Returns the length of v.
static inline float cpvlength(const phy_vect v)
{
	return sqrtf(cpvdot(v, v));
}

/// Linearly interpolate between v1 and v2.
static inline phy_vect cpvlerp(const phy_vect v1, const phy_vect v2, const float t)
{
	return cpvadd(cpvmult(v1, 1.0f - t), cpvmult(v2, t));
}

/// Returns a normalized copy of v.
static inline phy_vect cpvnormalize(const phy_vect v)
{
	// Neat trick I saw somewhere to avoid div/0.
	return cpvmult(v, 1.0f/(cpvlength(v) + FLT_MIN));
}

/// Spherical linearly interpolate between v1 and v2.
static inline phy_vect
cpvslerp(const phy_vect v1, const phy_vect v2, const float t)
{
	float dot = cpvdot(cpvnormalize(v1), cpvnormalize(v2));
	float omega = acosf(phy_clamp(dot, -1.0f, 1.0f));
	
	if(omega < 1e-3){
		// If the angle between two vectors is very small, lerp instead to avoid precision issues.
		return cpvlerp(v1, v2, t);
	} else {
		float denom = 1.0f/sinf(omega);
		return cpvadd(cpvmult(v1, sinf((1.0f - t)*omega)*denom), cpvmult(v2, sinf(t*omega)*denom));
	}
}

/// Spherical linearly interpolate between v1 towards v2 by no more than angle a radians
static inline phy_vect
cpvslerpconst(const phy_vect v1, const phy_vect v2, const float a)
{
	float dot = cpvdot(cpvnormalize(v1), cpvnormalize(v2));
	float omega = acosf(phy_clamp(dot, -1.0f, 1.0f));
	
	return cpvslerp(v1, v2, phy_min(a, omega)/omega);
}

/// Clamp v to length len.
static inline phy_vect cpvclamp(const phy_vect v, const float len)
{
	return (cpvdot(v,v) > len*len) ? cpvmult(cpvnormalize(v), len) : v;
}

/// Linearly interpolate between v1 towards v2 by distance d.
static inline phy_vect cpvlerpconst(phy_vect v1, phy_vect v2, float d)
{
	return cpvadd(v1, cpvclamp(cpvsub(v2, v1), d));
}

/// Returns the distance between v1 and v2.
static inline float cpvdist(const phy_vect v1, const phy_vect v2)
{
	return cpvlength(cpvsub(v1, v2));
}

/// Returns the squared distance between v1 and v2. Faster than cpvdist() when you only need to compare distances.
static inline float cpvdistsq(const phy_vect v1, const phy_vect v2)
{
	return cpvlengthsq(cpvsub(v1, v2));
}

/// Returns true if the distance between v1 and v2 is less than dist.
static inline bool cpvnear(const phy_vect v1, const phy_vect v2, const float dist)
{
	return cpvdistsq(v1, v2) < dist*dist;
}

/// @}

/// @defgroup cpMat2x2 cpMat2x2
/// 2x2 matrix type used for tensors and such.
/// @{

// NUKE
static inline phy_mat2x2
cpMat2x2New(float a, float b, float c, float d)
{
	phy_mat2x2 m = {a, b, c, d};
	return m;
}

static inline phy_vect
cpMat2x2Transform(phy_mat2x2 m, phy_vect v)
{
	return cpv(v.x*m.a + v.y*m.b, v.x*m.c + v.y*m.d);
}
