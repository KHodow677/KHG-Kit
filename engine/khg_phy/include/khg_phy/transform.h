#pragma once

#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include "khg_phy/bb.h"

/// Identity transform matrix.
static const phy_transform cpTransformIdentity = {1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};

/// Construct a new transform matrix.
/// (a, b) is the x basis vector.
/// (c, d) is the y basis vector.
/// (tx, ty) is the translation.
static inline phy_transform
cpTransformNew(float a, float b, float c, float d, float tx, float ty)
{
	phy_transform t = {a, b, c, d, tx, ty};
	return t;
}

/// Construct a new transform matrix in transposed order.
static inline phy_transform
cpTransformNewTranspose(float a, float c, float tx, float b, float d, float ty)
{
	phy_transform t = {a, b, c, d, tx, ty};
	return t;
}

/// Get the inverse of a transform matrix.
static inline phy_transform
cpTransformInverse(phy_transform t)
{
  float inv_det = 1.0/(t.a*t.d - t.c*t.b);
  return cpTransformNewTranspose(
     t.d*inv_det, -t.c*inv_det, (t.c*t.ty - t.tx*t.d)*inv_det,
    -t.b*inv_det,  t.a*inv_det, (t.tx*t.b - t.a*t.ty)*inv_det
  );
}

/// Multiply two transformation matrices.
static inline phy_transform
cpTransformMult(phy_transform t1, phy_transform t2)
{
  return cpTransformNewTranspose(
    t1.a*t2.a + t1.c*t2.b, t1.a*t2.c + t1.c*t2.d, t1.a*t2.tx + t1.c*t2.ty + t1.tx,
    t1.b*t2.a + t1.d*t2.b, t1.b*t2.c + t1.d*t2.d, t1.b*t2.tx + t1.d*t2.ty + t1.ty
  );
}

/// Transform an absolute point. (i.e. a vertex)
static inline phy_vect
cpTransformPoint(phy_transform t, phy_vect p)
{
  return cpv(t.a*p.x + t.c*p.y + t.tx, t.b*p.x + t.d*p.y + t.ty);
}

/// Transform a vector (i.e. a normal)
static inline phy_vect
cpTransformVect(phy_transform t, phy_vect v)
{
  return cpv(t.a*v.x + t.c*v.y, t.b*v.x + t.d*v.y);
}

/// Transform a cpBB.
static inline phy_bb
cpTransformbBB(phy_transform t, phy_bb bb)
{
	phy_vect center = phy_bb_center(bb);
	float hw = (bb.r - bb.l)*0.5;
	float hh = (bb.t - bb.b)*0.5;
	
	float a = t.a*hw, b = t.c*hh, d = t.b*hw, e = t.d*hh;
	float hw_max = phy_max(phy_abs(a + b), phy_abs(a - b));
	float hh_max = phy_max(phy_abs(d + e), phy_abs(d - e));
	return phy_bb_new_for_extents(cpTransformPoint(t, center), hw_max, hh_max);
}

/// Create a transation matrix.
static inline phy_transform
cpTransformTranslate(phy_vect translate)
{
  return cpTransformNewTranspose(
    1.0, 0.0, translate.x,
    0.0, 1.0, translate.y
  );
}

/// Create a scale matrix.
static inline phy_transform
cpTransformScale(float scaleX, float scaleY)
{
	return cpTransformNewTranspose(
		scaleX,    0.0, 0.0,
		   0.0, scaleY, 0.0
	);
}

/// Create a rotation matrix.
static inline phy_transform
cpTransformRotate(float radians)
{
	phy_vect rot = cpvforangle(radians);
	return cpTransformNewTranspose(
		rot.x, -rot.y, 0.0,
		rot.y,  rot.x, 0.0
	);
}

/// Create a rigid transformation matrix. (transation + rotation)
static inline phy_transform
cpTransformRigid(phy_vect translate, float radians)
{
	phy_vect rot = cpvforangle(radians);
	return cpTransformNewTranspose(
		rot.x, -rot.y, translate.x,
		rot.y,  rot.x, translate.y
	);
}

/// Fast inverse of a rigid transformation matrix.
static inline phy_transform
cpTransformRigidInverse(phy_transform t)
{
  return cpTransformNewTranspose(
     t.d, -t.c, (t.c*t.ty - t.tx*t.d),
    -t.b,  t.a, (t.tx*t.b - t.a*t.ty)
  );
}

//MARK: Miscellaneous (but useful) transformation matrices.
// See source for documentation...

static inline phy_transform
cpTransformWrap(phy_transform outer, phy_transform inner)
{
  return cpTransformMult(cpTransformInverse(outer), cpTransformMult(inner, outer));
}

static inline phy_transform
cpTransformWrapInverse(phy_transform outer, phy_transform inner)
{
  return cpTransformMult(outer, cpTransformMult(inner, cpTransformInverse(outer)));
}

static inline phy_transform
cpTransformOrtho(phy_bb bb)
{
  return cpTransformNewTranspose(
    2.0/(bb.r - bb.l), 0.0, -(bb.r + bb.l)/(bb.r - bb.l),
    0.0, 2.0/(bb.t - bb.b), -(bb.t + bb.b)/(bb.t - bb.b)
  );
}

static inline phy_transform
cpTransformBoneScale(phy_vect v0, phy_vect v1)
{
  phy_vect d = cpvsub(v1, v0); 
  return cpTransformNewTranspose(
    d.x, -d.y, v0.x,
    d.y,  d.x, v0.y
  );
}

static inline phy_transform
cpTransformAxialScale(phy_vect axis, phy_vect pivot, float scale)
{
  float A = axis.x*axis.y*(scale - 1.0);
  float B = cpvdot(axis, pivot)*(1.0 - scale);
  
  return cpTransformNewTranspose(
    scale*axis.x*axis.x + axis.y*axis.y, A, axis.x*B,
    A, axis.x*axis.x + scale*axis.y*axis.y, axis.y*B
  );
}
