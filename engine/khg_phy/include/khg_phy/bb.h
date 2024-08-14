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

static inline cpBB cpBBNewForExtents(const cpVect c, const float hw, const float hh) {
	return cpBBNew(c.x - hw, c.y - hh, c.x + hw, c.y + hh);
}

static inline cpBB cpBBNewForCircle(const cpVect p, const float r) {
	return cpBBNewForExtents(p, r, r);
}

static inline cpBool cpBBIntersects(const cpBB a, const cpBB b) {
	return (a.l <= b.r && b.l <= a.r && a.b <= b.t && b.b <= a.t);
}

static inline cpBool cpBBContainsBB(const cpBB bb, const cpBB other) {
	return (bb.l <= other.l && bb.r >= other.r && bb.b <= other.b && bb.t >= other.t);
}

static inline cpBool cpBBContainsVect(const cpBB bb, const cpVect v) {
	return (bb.l <= v.x && bb.r >= v.x && bb.b <= v.y && bb.t >= v.y);
}

static inline cpBB cpBBMerge(const cpBB a, const cpBB b) {
	return cpBBNew(cpfmin(a.l, b.l), cpfmin(a.b, b.b), cpfmax(a.r, b.r), cpfmax(a.t, b.t));
}

static inline cpBB cpBBExpand(const cpBB bb, const cpVect v){
	return cpBBNew(cpfmin(bb.l, v.x), cpfmin(bb.b, v.y), cpfmax(bb.r, v.x), cpfmax(bb.t, v.y));
}

static inline cpVect cpBBCenter(cpBB bb) {
	return cpvlerp(cpv(bb.l, bb.b), cpv(bb.r, bb.t), 0.5f);
}

static inline float cpBBArea(cpBB bb) {
	return (bb.r - bb.l)*(bb.t - bb.b);
}

static inline float cpBBMergedArea(cpBB a, cpBB b) {
	return (cpfmax(a.r, b.r) - cpfmin(a.l, b.l))*(cpfmax(a.t, b.t) - cpfmin(a.b, b.b));
}

static inline float cpBBSegmentQuery(cpBB bb, cpVect a, cpVect b) {
	cpVect delta = cpvsub(b, a);
	float tmin = -INFINITY, tmax = INFINITY;
  if (delta.x == 0.0f) {
		if(a.x < bb.l || bb.r < a.x) {
      return INFINITY;
    }
	} 
  else {
		float t1 = (bb.l - a.x)/delta.x;
		float t2 = (bb.r - a.x)/delta.x;
		tmin = cpfmax(tmin, cpfmin(t1, t2));
		tmax = cpfmin(tmax, cpfmax(t1, t2));
	}
  if (delta.y == 0.0f) {
		if(a.y < bb.b || bb.t < a.y) {
      return INFINITY;
    }
	} 
  else {
		float t1 = (bb.b - a.y)/delta.y;
		float t2 = (bb.t - a.y)/delta.y;
		tmin = cpfmax(tmin, cpfmin(t1, t2));
		tmax = cpfmin(tmax, cpfmax(t1, t2));
	}
  if (tmin <= tmax && 0.0f <= tmax && tmin <= 1.0f) {
		return cpfmax(tmin, 0.0f);
	} 
  else {
		return INFINITY;
	}
}

static inline cpBool cpBBIntersectsSegment(cpBB bb, cpVect a, cpVect b) {
	return (cpBBSegmentQuery(bb, a, b) != INFINITY);
}

static inline cpVect cpBBClampVect(const cpBB bb, const cpVect v) {
	return cpv(cpfclamp(v.x, bb.l, bb.r), cpfclamp(v.y, bb.b, bb.t));
}

static inline cpVect cpBBWrapVect(const cpBB bb, const cpVect v) {
	float dx = cpfabs(bb.r - bb.l);
	float modx = cpfmod(v.x - bb.l, dx);
	float x = (modx > 0.0f) ? modx : modx + dx;
	float dy = cpfabs(bb.t - bb.b);
	float mody = cpfmod(v.y - bb.b, dy);
	float y = (mody > 0.0f) ? mody : mody + dy;
	return cpv(x + bb.l, y + bb.b);
}

static inline cpBB cpBBOffset(const cpBB bb, const cpVect v) {
	return cpBBNew(bb.l + v.x, bb.b + v.y, bb.r + v.x, bb.t + v.y);
}
