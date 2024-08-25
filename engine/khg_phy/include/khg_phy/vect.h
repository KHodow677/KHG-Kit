#pragma once

#include "khg_phy/phy_types.h"
#include <math.h>

static const phy_vect phy_v_zero = {0.0f,0.0f};

static inline phy_vect phy_v(const float x, const float y) {
	phy_vect v = {x, y};
	return v;
}

static inline bool phy_v_eql(const phy_vect v1, const phy_vect v2) {
	return (v1.x == v2.x && v1.y == v2.y);
}

static inline phy_vect phy_v_add(const phy_vect v1, const phy_vect v2) {
	return phy_v(v1.x + v2.x, v1.y + v2.y);
}

static inline phy_vect phy_v_sub(const phy_vect v1, const phy_vect v2) {
	return phy_v(v1.x - v2.x, v1.y - v2.y);
}

static inline phy_vect phy_v_neg(const phy_vect v) {
	return phy_v(-v.x, -v.y);
}

static inline phy_vect phy_v_mult(const phy_vect v, const float s) {
	return phy_v(v.x*s, v.y*s);
}

static inline float phy_v_dot(const phy_vect v1, const phy_vect v2) {
	return v1.x*v2.x + v1.y*v2.y;
}

static inline float phy_v_cross(const phy_vect v1, const phy_vect v2) {
	return v1.x*v2.y - v1.y*v2.x;
}

static inline phy_vect phy_v_perp(const phy_vect v) {
	return phy_v(-v.y, v.x);
}

static inline phy_vect phy_v_rperp(const phy_vect v) {
	return phy_v(v.y, -v.x);
}

static inline phy_vect phy_v_project(const phy_vect v1, const phy_vect v2) {
	return phy_v_mult(v2, phy_v_dot(v1, v2)/phy_v_dot(v2, v2));
}

static inline phy_vect phy_v_for_angle(const float a) {
	return phy_v(cosf(a), sinf(a));
}

static inline float phy_v_to_angle(const phy_vect v) {
	return atan2f(v.y, v.x);
}

static inline phy_vect phy_v_rotate(const phy_vect v1, const phy_vect v2) {
	return phy_v(v1.x*v2.x - v1.y*v2.y, v1.x*v2.y + v1.y*v2.x);
}

static inline phy_vect phy_v_unrotate(const phy_vect v1, const phy_vect v2) {
	return phy_v(v1.x*v2.x + v1.y*v2.y, v1.y*v2.x - v1.x*v2.y);
}

static inline float phy_v_length_sq(const phy_vect v) {
	return phy_v_dot(v, v);
}

static inline float phy_v_length(const phy_vect v) {
	return sqrtf(phy_v_dot(v, v));
}

static inline phy_vect phy_v_lerp(const phy_vect v1, const phy_vect v2, const float t) {
	return phy_v_add(phy_v_mult(v1, 1.0f - t), phy_v_mult(v2, t));
}

static inline phy_vect phy_v_normalize(const phy_vect v) {
	return phy_v_mult(v, 1.0f/(phy_v_length(v) + FLT_MIN));
}

static inline phy_vect phy_v_slerp(const phy_vect v1, const phy_vect v2, const float t) {
	float dot = phy_v_dot(phy_v_normalize(v1), phy_v_normalize(v2));
	float omega = acosf(phy_clamp(dot, -1.0f, 1.0f));
  if(omega < 1e-3){
		return phy_v_lerp(v1, v2, t);
	} 
  else {
		float denom = 1.0f/sinf(omega);
		return phy_v_add(phy_v_mult(v1, sinf((1.0f - t)*omega)*denom), phy_v_mult(v2, sinf(t*omega)*denom));
	}
}

static inline phy_vect phy_v_slerp_const(const phy_vect v1, const phy_vect v2, const float a) {
	float dot = phy_v_dot(phy_v_normalize(v1), phy_v_normalize(v2));
	float omega = acosf(phy_clamp(dot, -1.0f, 1.0f));
	return phy_v_slerp(v1, v2, phy_min(a, omega)/omega);
}

static inline phy_vect phy_v_clamp(const phy_vect v, const float len) {
	return (phy_v_dot(v,v) > len*len) ? phy_v_mult(phy_v_normalize(v), len) : v;
}

static inline phy_vect phy_v_lerp_const(phy_vect v1, phy_vect v2, float d) {
	return phy_v_add(v1, phy_v_clamp(phy_v_sub(v2, v1), d));
}

static inline float phy_v_dist(const phy_vect v1, const phy_vect v2) {
	return phy_v_length(phy_v_sub(v1, v2));
}

static inline float phy_v_dist_sq(const phy_vect v1, const phy_vect v2) {
	return phy_v_length_sq(phy_v_sub(v1, v2));
}

static inline bool phy_v_near(const phy_vect v1, const phy_vect v2, const float dist) {
	return phy_v_dist_sq(v1, v2) < dist*dist;
}

static inline phy_mat2x2 phy_mat2x2_new(float a, float b, float c, float d) {
	phy_mat2x2 m = {a, b, c, d};
	return m;
}

static inline phy_vect phy_mat2x2_transform(phy_mat2x2 m, phy_vect v) {
	return phy_v(v.x*m.a + v.y*m.b, v.x*m.c + v.y*m.d);
}

