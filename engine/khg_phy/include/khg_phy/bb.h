#pragma once

#include "khg_phy/phy_types.h"
#include "khg_phy/vect.h"

typedef struct cpBB{
	float l, b, r ,t;
} cpBB;

static inline cpBB cpBBNew(const float l, const float b, const float r, const float t) {
	cpBB bb = { l, b, r, t };
	return bb;
}

static inline cpBB cpBBNewForExtents(const phy_vect c, const float hw, const float hh) {
	return cpBBNew(c.x - hw, c.y - hh, c.x + hw, c.y + hh);
}

static inline cpBB cpBBNewForCircle(const phy_vect p, const float r) {
	return cpBBNewForExtents(p, r, r);
}

static inline bool cpBBIntersects(const cpBB a, const cpBB b) {
	return (a.l <= b.r && b.l <= a.r && a.b <= b.t && b.b <= a.t);
}

static inline bool cpBBContainsBB(const cpBB bb, const cpBB other) {
	return (bb.l <= other.l && bb.r >= other.r && bb.b <= other.b && bb.t >= other.t);
}

static inline bool cpBBContainsVect(const cpBB bb, const phy_vect v) {
	return (bb.l <= v.x && bb.r >= v.x && bb.b <= v.y && bb.t >= v.y);
}

static inline cpBB cpBBMerge(const cpBB a, const cpBB b) {
	return cpBBNew(phy_min(a.l, b.l), phy_min(a.b, b.b), phy_max(a.r, b.r), phy_max(a.t, b.t));
}

static inline cpBB cpBBExpand(const cpBB bb, const phy_vect v){
	return cpBBNew(phy_min(bb.l, v.x), phy_min(bb.b, v.y), phy_max(bb.r, v.x), phy_max(bb.t, v.y));
}

static inline phy_vect cpBBCenter(cpBB bb) {
	return cpvlerp(cpv(bb.l, bb.b), cpv(bb.r, bb.t), 0.5f);
}

static inline float cpBBArea(cpBB bb) {
	return (bb.r - bb.l)*(bb.t - bb.b);
}

static inline float cpBBMergedArea(cpBB a, cpBB b) {
	return (phy_max(a.r, b.r) - phy_min(a.l, b.l))*(phy_max(a.t, b.t) - phy_min(a.b, b.b));
}

static inline float cpBBSegmentQuery(cpBB bb, phy_vect a, phy_vect b) {
	phy_vect delta = cpvsub(b, a);
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

static inline bool cpBBIntersectsSegment(cpBB bb, phy_vect a, phy_vect b) {
	return (cpBBSegmentQuery(bb, a, b) != INFINITY);
}

static inline phy_vect cpBBClampVect(const cpBB bb, const phy_vect v) {
	return cpv(phy_clamp(v.x, bb.l, bb.r), phy_clamp(v.y, bb.b, bb.t));
}

static inline phy_vect cpBBWrapVect(const cpBB bb, const phy_vect v) {
	float dx = phy_abs(bb.r - bb.l);
	float modx = fmodf(v.x - bb.l, dx);
	float x = (modx > 0.0f) ? modx : modx + dx;
	float dy = phy_abs(bb.t - bb.b);
	float mody = fmodf(v.y - bb.b, dy);
	float y = (mody > 0.0f) ? mody : mody + dy;
	return cpv(x + bb.l, y + bb.b);
}

static inline cpBB cpBBOffset(const cpBB bb, const phy_vect v) {
	return cpBBNew(bb.l + v.x, bb.b + v.y, bb.r + v.x, bb.t + v.y);
}
