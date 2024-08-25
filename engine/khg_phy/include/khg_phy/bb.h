#pragma once

#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"

typedef struct phy_bb {
	float l, b, r ,t;
} phy_bb;

static inline phy_bb phy_bb_new(const float l, const float b, const float r, const float t) {
	phy_bb bb = { l, b, r, t };
	return bb;
}

static inline phy_bb phy_bb_new_for_extents(const phy_vect c, const float hw, const float hh) {
	return phy_bb_new(c.x - hw, c.y - hh, c.x + hw, c.y + hh);
}

static inline phy_bb phy_bb_new_for_circle(const phy_vect p, const float r) {
	return phy_bb_new_for_extents(p, r, r);
}

static inline bool phy_bb_intersects(const phy_bb a, const phy_bb b) {
	return (a.l <= b.r && b.l <= a.r && a.b <= b.t && b.b <= a.t);
}

static inline bool phy_bb_contains_bb(const phy_bb bb, const phy_bb other) {
	return (bb.l <= other.l && bb.r >= other.r && bb.b <= other.b && bb.t >= other.t);
}

static inline bool phy_bb_contains_vect(const phy_bb bb, const phy_vect v) {
	return (bb.l <= v.x && bb.r >= v.x && bb.b <= v.y && bb.t >= v.y);
}

static inline phy_bb phy_bb_merge(const phy_bb a, const phy_bb b) {
	return phy_bb_new(phy_min(a.l, b.l), phy_min(a.b, b.b), phy_max(a.r, b.r), phy_max(a.t, b.t));
}

static inline phy_bb phy_bb_expand(const phy_bb bb, const phy_vect v){
	return phy_bb_new(phy_min(bb.l, v.x), phy_min(bb.b, v.y), phy_max(bb.r, v.x), phy_max(bb.t, v.y));
}

static inline phy_vect phy_bb_center(phy_bb bb) {
	return phy_v_lerp(phy_v(bb.l, bb.b), phy_v(bb.r, bb.t), 0.5f);
}

static inline float phy_bb_area(phy_bb bb) {
	return (bb.r - bb.l)*(bb.t - bb.b);
}

static inline float phy_bb_merged_area(phy_bb a, phy_bb b) {
	return (phy_max(a.r, b.r) - phy_min(a.l, b.l))*(phy_max(a.t, b.t) - phy_min(a.b, b.b));
}

static inline float phy_bb_segment_query(phy_bb bb, phy_vect a, phy_vect b) {
	phy_vect delta = phy_v_sub(b, a);
	float tmin = -INFINITY, tmax = INFINITY;
  if (delta.x == 0.0f) {
		if(a.x < bb.l || bb.r < a.x) {
      return INFINITY;
    }
	} 
  else {
		float t1 = (bb.l - a.x)/delta.x;
		float t2 = (bb.r - a.x)/delta.x;
		tmin = phy_max(tmin, phy_min(t1, t2));
		tmax = phy_min(tmax, phy_max(t1, t2));
	}
  if (delta.y == 0.0f) {
		if(a.y < bb.b || bb.t < a.y) {
      return INFINITY;
    }
	} 
  else {
		float t1 = (bb.b - a.y)/delta.y;
		float t2 = (bb.t - a.y)/delta.y;
		tmin = phy_max(tmin, phy_min(t1, t2));
		tmax = phy_min(tmax, phy_max(t1, t2));
	}
  if (tmin <= tmax && 0.0f <= tmax && tmin <= 1.0f) {
		return phy_max(tmin, 0.0f);
	} 
  else {
		return INFINITY;
	}
}

static inline bool phy_bb_intersects_segment(phy_bb bb, phy_vect a, phy_vect b) {
	return (phy_bb_segment_query(bb, a, b) != INFINITY);
}

static inline phy_vect phy_bb_clamp_vect(const phy_bb bb, const phy_vect v) {
	return phy_v(phy_clamp(v.x, bb.l, bb.r), phy_clamp(v.y, bb.b, bb.t));
}

static inline phy_vect phy_bb_wrap_vect(const phy_bb bb, const phy_vect v) {
	float dx = phy_abs(bb.r - bb.l);
	float modx = fmodf(v.x - bb.l, dx);
	float x = (modx > 0.0f) ? modx : modx + dx;
	float dy = phy_abs(bb.t - bb.b);
	float mody = fmodf(v.y - bb.b, dy);
	float y = (mody > 0.0f) ? mody : mody + dy;
	return phy_v(x + bb.l, y + bb.b);
}

static inline phy_bb phy_bb_offset(const phy_bb bb, const phy_vect v) {
	return phy_bb_new(bb.l + v.x, bb.b + v.y, bb.r + v.x, bb.t + v.y);
}

