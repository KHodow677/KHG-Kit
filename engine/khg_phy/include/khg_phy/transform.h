#pragma once

#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"
#include "khg_phy/bb.h"

static const phy_transform phy_transform_identity = {1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};

static inline phy_transform phy_transform_new(float a, float b, float c, float d, float tx, float ty) {
	phy_transform t = {a, b, c, d, tx, ty};
	return t;
}

static inline phy_transform phy_transform_new_transpose(float a, float c, float tx, float b, float d, float ty) {
	phy_transform t = {a, b, c, d, tx, ty};
	return t;
}

static inline phy_transform phy_transform_inverse(phy_transform t) {
  float inv_det = 1.0/(t.a*t.d - t.c*t.b);
  return phy_transform_new_transpose(t.d*inv_det, -t.c*inv_det, (t.c*t.ty - t.tx*t.d)*inv_det, -t.b*inv_det,  t.a*inv_det, (t.tx*t.b - t.a*t.ty)*inv_det);
}

static inline phy_transform phy_transform_mult(phy_transform t1, phy_transform t2) {
  return phy_transform_new_transpose(t1.a*t2.a + t1.c*t2.b, t1.a*t2.c + t1.c*t2.d, t1.a*t2.tx + t1.c*t2.ty + t1.tx, t1.b*t2.a + t1.d*t2.b, t1.b*t2.c + t1.d*t2.d, t1.b*t2.tx + t1.d*t2.ty + t1.ty);
}

static inline phy_vect phy_transform_point(phy_transform t, phy_vect p) {
  return phy_v(t.a*p.x + t.c*p.y + t.tx, t.b*p.x + t.d*p.y + t.ty);
}

static inline phy_vect phy_transform_vect(phy_transform t, phy_vect v) {
  return phy_v(t.a*v.x + t.c*v.y, t.b*v.x + t.d*v.y);
}

static inline phy_bb phy_transform_BB(phy_transform t, phy_bb bb) {
	phy_vect center = phy_bb_center(bb);
	float hw = (bb.r - bb.l)*0.5;
	float hh = (bb.t - bb.b)*0.5;
	float a = t.a*hw, b = t.c*hh, d = t.b*hw, e = t.d*hh;
	float hw_max = phy_max(phy_abs(a + b), phy_abs(a - b));
	float hh_max = phy_max(phy_abs(d + e), phy_abs(d - e));
	return phy_bb_new_for_extents(phy_transform_point(t, center), hw_max, hh_max);
}

static inline phy_transform phy_transform_translate(phy_vect translate) {
  return phy_transform_new_transpose(1.0, 0.0, translate.x, 0.0, 1.0, translate.y);
}

static inline phy_transform phy_transform_scale(float scaleX, float scaleY) {
	return phy_transform_new_transpose(scaleX, 0.0, 0.0, 0.0, scaleY, 0.0);
}

static inline phy_transform phy_transform_rotate(float radians) {
	phy_vect rot = phy_v_for_angle(radians);
	return phy_transform_new_transpose(rot.x, -rot.y, 0.0, rot.y,  rot.x, 0.0);
}

static inline phy_transform phy_transform_rigid(phy_vect translate, float radians) {
	phy_vect rot = phy_v_for_angle(radians);
	return phy_transform_new_transpose(rot.x, -rot.y, translate.x, rot.y,  rot.x, translate.y);
}

static inline phy_transform phy_transform_rigid_inverse(phy_transform t) {
  return phy_transform_new_transpose(t.d, -t.c, (t.c*t.ty - t.tx*t.d), -t.b,  t.a, (t.tx*t.b - t.a*t.ty));
}

static inline phy_transform phy_transform_wrap(phy_transform outer, phy_transform inner) {
  return phy_transform_mult(phy_transform_inverse(outer), phy_transform_mult(inner, outer));
}

static inline phy_transform phy_transform_wrap_inverse(phy_transform outer, phy_transform inner) {
  return phy_transform_mult(outer, phy_transform_mult(inner, phy_transform_inverse(outer)));
}

static inline phy_transform phy_transform_ortho(phy_bb bb) {
  return phy_transform_new_transpose(2.0/(bb.r - bb.l), 0.0, -(bb.r + bb.l)/(bb.r - bb.l), 0.0, 2.0/(bb.t - bb.b), -(bb.t + bb.b)/(bb.t - bb.b));
}

static inline phy_transform phy_transform_bone_scale(phy_vect v0, phy_vect v1) {
  phy_vect d = phy_v_sub(v1, v0); 
  return phy_transform_new_transpose(d.x, -d.y, v0.x, d.y,  d.x, v0.y);
}

static inline phy_transform phy_transform_axial_scale(phy_vect axis, phy_vect pivot, float scale) {
  float A = axis.x*axis.y*(scale - 1.0);
  float B = phy_v_dot(axis, pivot)*(1.0 - scale);
  return phy_transform_new_transpose(scale*axis.x*axis.x + axis.y*axis.y, A, axis.x*B, A, axis.x*axis.x + scale*axis.y*axis.y, axis.y*B);
}

