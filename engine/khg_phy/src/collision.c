#include "khg_phy/phy_private.h"
#include "khg_phy/arbiter.h"	
#include "khg_phy/robust.h"
#include "khg_utl/error_func.h"

#if DEBUG && 0
#include "ChipmunkDemo.h"
#define DRAW_ALL 0
#define DRAW_GJK (0 || DRAW_ALL)
#define DRAW_EPA (0 || DRAW_ALL)
#define DRAW_CLOSEST (0 || DRAW_ALL)
#define DRAW_CLIP (0 || DRAW_ALL)

#define PRINT_LOG 0
#endif

#define MAX_GJK_ITERATIONS 30
#define MAX_EPA_ITERATIONS 30
#define WARN_GJK_ITERATIONS 20
#define WARN_EPA_ITERATIONS 20

static inline void
cpCollisionInfoPushContact(struct cpCollisionInfo *info, phy_vect p1, phy_vect p2, phy_hash_value hash)
{
	if (info->count > PHY_MAX_CONTACTS_PER_ARBITER) {
    utl_error_func("Tried to push too many contacts", utl_user_defined_data);
  }
	
	struct cpContact *con = &info->arr[info->count];
	con->r1 = p1;
	con->r2 = p2;
	con->hash = hash;
	
	info->count++;
}

//MARK: Support Points and Edges:

// Support points are the maximal points on a shape's perimeter along a certain axis.
// The GJK and EPA algorithms use support points to iteratively sample the surface of the two shapes' minkowski difference.

static inline int
PolySupportPointIndex(const int count, const struct cpSplittingPlane *planes, const phy_vect n)
{
	float max = -INFINITY;
	int index = 0;
	
	for(int i=0; i<count; i++){
		phy_vect v = planes[i].v0;
		float d = cpvdot(v, n);
		if(d > max){
			max = d;
			index = i;
		}
	}
	
	return index;
}

struct SupportPoint {
	phy_vect p;
	// Save an index of the point so it can be cheaply looked up as a starting point for the next frame.
	phy_collision_id index;
};

static inline struct SupportPoint
SupportPointNew(phy_vect p, phy_collision_id index)
{
	struct SupportPoint point = {p, index};
	return point;
}

typedef struct SupportPoint (*SupportPointFunc)(const phy_shape *shape, const phy_vect n);

static inline struct SupportPoint
CircleSupportPoint(const phy_circle_shape *circle, const phy_vect n)
{
	return SupportPointNew(circle->tc, 0);
}

static inline struct SupportPoint
SegmentSupportPoint(const phy_segment_shape *seg, const phy_vect n)
{
	if(cpvdot(seg->ta, n) > cpvdot(seg->tb, n)){
		return SupportPointNew(seg->ta, 0);
	} else {
		return SupportPointNew(seg->tb, 1);
	}
}

static inline struct SupportPoint
PolySupportPoint(const phy_poly_shape *poly, const phy_vect n)
{
	const struct cpSplittingPlane *planes = poly->planes;
	int i = PolySupportPointIndex(poly->count, planes, n);
	return SupportPointNew(planes[i].v0, i);
}

// A point on the surface of two shape's minkowski difference.
struct MinkowskiPoint {
	// Cache the two original support points.
	phy_vect a, b;
	// b - a
	phy_vect ab;
	// Concatenate the two support point indexes.
	phy_collision_id id;
};

static inline struct MinkowskiPoint
MinkowskiPointNew(const struct SupportPoint a, const struct SupportPoint b)
{
	struct MinkowskiPoint point = {a.p, b.p, cpvsub(b.p, a.p), (a.index & 0xFF)<<8 | (b.index & 0xFF)};
	return point;
}

struct SupportContext {
	const phy_shape *shape1, *shape2;
	SupportPointFunc func1, func2;
};

// Calculate the maximal point on the minkowski difference of two shapes along a particular axis.
static inline struct MinkowskiPoint
Support(const struct SupportContext *ctx, const phy_vect n)
{
	struct SupportPoint a = ctx->func1(ctx->shape1, cpvneg(n));
	struct SupportPoint b = ctx->func2(ctx->shape2, n);
	return MinkowskiPointNew(a, b);
}

struct EdgePoint {
	phy_vect p;
	// Keep a hash value for Chipmunk's collision hashing mechanism.
	phy_hash_value hash;
};

// Support edges are the edges of a polygon or segment shape that are in contact.
struct Edge {
	struct EdgePoint a, b;
	float r;
	phy_vect n;
};

static struct Edge
SupportEdgeForPoly(const phy_poly_shape *poly, const phy_vect n)
{
	int count = poly->count;
	int i1 = PolySupportPointIndex(poly->count, poly->planes, n);
	
	// TODO: get rid of mod eventually, very expensive on ARM
	int i0 = (i1 - 1 + count)%count;
	int i2 = (i1 + 1)%count;
	
	const struct cpSplittingPlane *planes = poly->planes;
	phy_hash_value hashid = poly->shape.hashid;
	if(cpvdot(n, planes[i1].n) > cpvdot(n, planes[i2].n)){
		struct Edge edge = {{planes[i0].v0, CP_HASH_PAIR(hashid, i0)}, {planes[i1].v0, CP_HASH_PAIR(hashid, i1)}, poly->r, planes[i1].n};
		return edge;
	} else {
		struct Edge edge = {{planes[i1].v0, CP_HASH_PAIR(hashid, i1)}, {planes[i2].v0, CP_HASH_PAIR(hashid, i2)}, poly->r, planes[i2].n};
		return edge;
	}
}

static struct Edge
SupportEdgeForSegment(const phy_segment_shape *seg, const phy_vect n)
{
	phy_hash_value hashid = seg->shape.hashid;
	if(cpvdot(seg->tn, n) > 0.0){
		struct Edge edge = {{seg->ta, CP_HASH_PAIR(hashid, 0)}, {seg->tb, CP_HASH_PAIR(hashid, 1)}, seg->r, seg->tn};
		return edge;
	} else {
		struct Edge edge = {{seg->tb, CP_HASH_PAIR(hashid, 1)}, {seg->ta, CP_HASH_PAIR(hashid, 0)}, seg->r, cpvneg(seg->tn)};
		return edge;
	}
}

// Find the closest p(t) to (0, 0) where p(t) = a*(1-t)/2 + b*(1+t)/2
// The range for t is [-1, 1] to avoid floating point issues if the parameters are swapped.
static inline float
ClosestT(const phy_vect a, const phy_vect b)
{
	phy_vect delta = cpvsub(b, a);
	return -phy_clamp(cpvdot(delta, cpvadd(a, b))/(cpvlengthsq(delta) + FLT_MIN), -1.0f, 1.0f);
}

// Basically the same as cpvlerp(), except t = [-1, 1]
static inline phy_vect
LerpT(const phy_vect a, const phy_vect b, const float t)
{
	float ht = 0.5f*t;
	return cpvadd(cpvmult(a, 0.5f - ht), cpvmult(b, 0.5f + ht));
}

// Closest points on the surface of two shapes.
struct ClosestPoints {
	// Surface points in absolute coordinates.
	phy_vect a, b;
	// Minimum separating axis of the two shapes.
	phy_vect n;
	// Signed distance between the points.
	float d;
	// Concatenation of the id's of the minkoski points.
	phy_collision_id id;
};

// Calculate the closest points on two shapes given the closest edge on their minkowski difference to (0, 0)
static inline struct ClosestPoints
ClosestPointsNew(const struct MinkowskiPoint v0, const struct MinkowskiPoint v1)
{
	// Find the closest p(t) on the minkowski difference to (0, 0)
	float t = ClosestT(v0.ab, v1.ab);
	phy_vect p = LerpT(v0.ab, v1.ab, t);
	
	// Interpolate the original support points using the same 't' value as above.
	// This gives you the closest surface points in absolute coordinates. NEAT!
	phy_vect pa = LerpT(v0.a, v1.a, t);
	phy_vect pb = LerpT(v0.b, v1.b, t);
	phy_collision_id id = (v0.id & 0xFFFF)<<16 | (v1.id & 0xFFFF);
	
	// First try calculating the MSA from the minkowski difference edge.
	// This gives us a nice, accurate MSA when the surfaces are close together.
	phy_vect delta = cpvsub(v1.ab, v0.ab);
	phy_vect n = cpvnormalize(cpvrperp(delta));
	float d = cpvdot(n, p);
	
	if(d <= 0.0f || (-1.0f < t && t < 1.0f)){
		// If the shapes are overlapping, or we have a regular vertex/edge collision, we are done.
		struct ClosestPoints points = {pa, pb, n, d, id};
		return points;
	} else {
		// Vertex/vertex collisions need special treatment since the MSA won't be shared with an axis of the minkowski difference.
		float d2 = cpvlength(p);
		phy_vect n2 = cpvmult(p, 1.0f/(d2 + FLT_MIN));
		
		struct ClosestPoints points = {pa, pb, n2, d2, id};
		return points;
	}
}

//MARK: EPA Functions

static inline float
ClosestDist(const phy_vect v0,const phy_vect v1)
{
	return cpvlengthsq(LerpT(v0, v1, ClosestT(v0, v1)));
}

// Recursive implementation of the EPA loop.
// Each recursion adds a point to the convex hull until it's known that we have the closest point on the surface.
static struct ClosestPoints
EPARecurse(const struct SupportContext *ctx, const int count, const struct MinkowskiPoint *hull, const int iteration)
{
	int mini = 0;
	float minDist = INFINITY;
	
	// TODO: precalculate this when building the hull and save a step.
	// Find the closest segment hull[i] and hull[i + 1] to (0, 0)
	for(int j=0, i=count-1; j<count; i=j, j++){
		float d = ClosestDist(hull[i].ab, hull[j].ab);
		if(d < minDist){
			minDist = d;
			mini = i;
		}
	}
	
	struct MinkowskiPoint v0 = hull[mini];
	struct MinkowskiPoint v1 = hull[(mini + 1)%count];
	if (cpveql(v0.ab, v1.ab)) {
    utl_error_func("Internal Error: EPA vertices are the same", utl_user_defined_data);
  }
	
	// Check if there is a point on the minkowski difference beyond this edge.
	struct MinkowskiPoint p = Support(ctx, cpvperp(cpvsub(v1.ab, v0.ab)));
	
#if DRAW_EPA
	phy_vect verts[count];
	for(int i=0; i<count; i++) verts[i] = hull[i].ab;
	
	ChipmunkDebugDrawPolygon(count, verts, 0.0, RGBAColor(1, 1, 0, 1), RGBAColor(1, 1, 0, 0.25));
	ChipmunkDebugDrawSegment(v0.ab, v1.ab, RGBAColor(1, 0, 0, 1));
	
	ChipmunkDebugDrawDot(5, p.ab, LAColor(1, 1));
#endif
	
	// The usual exit condition is a duplicated vertex.
	// Much faster to check the ids than to check the signed area.
	bool duplicate = (p.id == v0.id || p.id == v1.id);
	
	if(!duplicate && cpCheckPointGreater(v0.ab, v1.ab, p.ab) && iteration < MAX_EPA_ITERATIONS){
		// Rebuild the convex hull by inserting p.
		struct MinkowskiPoint *hull2 = (struct MinkowskiPoint *)alloca((count + 1)*sizeof(struct MinkowskiPoint));
		int count2 = 1;
		hull2[0] = p;
		
		for(int i=0; i<count; i++){
			int index = (mini + 1 + i)%count;
			
			phy_vect h0 = hull2[count2 - 1].ab;
			phy_vect h1 = hull[index].ab;
			phy_vect h2 = (i + 1 < count ? hull[(index + 1)%count] : p).ab;
			
			if(cpCheckPointGreater(h0, h2, h1)){
				hull2[count2] = hull[index];
				count2++;
			}
		}
		
		return EPARecurse(ctx, count2, hull2, iteration + 1);
	} else {
		// Could not find a new point to insert, so we have found the closest edge of the minkowski difference.
		if (iteration >= WARN_EPA_ITERATIONS) {
      utl_error_func("High EPA iterations", utl_user_defined_data);
    }
		return ClosestPointsNew(v0, v1);
	}
}

// Find the closest points on the surface of two overlapping shapes using the EPA algorithm.
// EPA is called from GJK when two shapes overlap.
// This is a moderately expensive step! Avoid it by adding radii to your shapes so their inner polygons won't overlap.
static struct ClosestPoints
EPA(const struct SupportContext *ctx, const struct MinkowskiPoint v0, const struct MinkowskiPoint v1, const struct MinkowskiPoint v2)
{
	// TODO: allocate a NxM array here and do an in place convex hull reduction in EPARecurse?
	struct MinkowskiPoint hull[3] = {v0, v1, v2};
	return EPARecurse(ctx, 3, hull, 1);
}

//MARK: GJK Functions.

// Recursive implementation of the GJK loop.
static inline struct ClosestPoints
GJKRecurse(const struct SupportContext *ctx, const struct MinkowskiPoint v0, const struct MinkowskiPoint v1, const int iteration)
{
	if(iteration > MAX_GJK_ITERATIONS){
		utl_error_func("High GJK iterations", utl_user_defined_data);
		return ClosestPointsNew(v0, v1);
	}
	
	if(cpCheckPointGreater(v1.ab, v0.ab, cpvzero)){
		// Origin is behind axis. Flip and try again.
		return GJKRecurse(ctx, v1, v0, iteration);
	} else {
		float t = ClosestT(v0.ab, v1.ab);
		phy_vect n = (-1.0f < t && t < 1.0f ? cpvperp(cpvsub(v1.ab, v0.ab)) : cpvneg(LerpT(v0.ab, v1.ab, t)));
		struct MinkowskiPoint p = Support(ctx, n);
		
#if DRAW_GJK
		ChipmunkDebugDrawSegment(v0.ab, v1.ab, RGBAColor(1, 1, 1, 1));
		phy_vect c = cpvlerp(v0.ab, v1.ab, 0.5);
		ChipmunkDebugDrawSegment(c, cpvadd(c, cpvmult(cpvnormalize(n), 5.0)), RGBAColor(1, 0, 0, 1));
		
		ChipmunkDebugDrawDot(5.0, p.ab, LAColor(1, 1));
#endif
		
		if(cpCheckPointGreater(p.ab, v0.ab, cpvzero) && cpCheckPointGreater(v1.ab, p.ab, cpvzero)){
			// The triangle v0, p, v1 contains the origin. Use EPA to find the MSA.
			if (iteration >= WARN_GJK_ITERATIONS) {
        utl_error_func("High GJK->EPA iterations", utl_user_defined_data);
      }
			return EPA(ctx, v0, p, v1);
		} else {
			if(cpCheckAxis(v0.ab, v1.ab, p.ab, n)){
				// The edge v0, v1 that we already have is the closest to (0, 0) since p was not closer.
			  if (iteration >= WARN_GJK_ITERATIONS) {
				  utl_error_func("High GJK iterations", utl_user_defined_data);
        }
				return ClosestPointsNew(v0, v1);
			} else {
				// p was closer to the origin than our existing edge.
				// Need to figure out which existing point to drop.
				if(ClosestDist(v0.ab, p.ab) < ClosestDist(p.ab, v1.ab)){
					return GJKRecurse(ctx, v0, p, iteration + 1);
				} else {
					return GJKRecurse(ctx, p, v1, iteration + 1);
				}
			}
		}
	}
}

// Get a SupportPoint from a cached shape and index.
static struct SupportPoint
ShapePoint(const phy_shape *shape, const int i)
{
	switch(shape->klass->type){
		case CP_CIRCLE_SHAPE: {
			return SupportPointNew(((phy_circle_shape *)shape)->tc, 0);
		} case CP_SEGMENT_SHAPE: {
			phy_segment_shape *seg = (phy_segment_shape *)shape;
			return SupportPointNew(i == 0 ? seg->ta : seg->tb, i);
		} case CP_POLY_SHAPE: {
			phy_poly_shape *poly = (phy_poly_shape *)shape;
			// Poly shapes may change vertex count.
			int index = (i < poly->count ? i : 0);
			return SupportPointNew(poly->planes[index].v0, index);
		} default: {
			return SupportPointNew(cpvzero, 0);
		}
	}
}

// Find the closest points between two shapes using the GJK algorithm.
static struct ClosestPoints
GJK(const struct SupportContext *ctx, phy_collision_id *id)
{
#if DRAW_GJK || DRAW_EPA
	int count1 = 1;
	int count2 = 1;
	
	switch(ctx->shape1->klass->type){
		case CP_SEGMENT_SHAPE: count1 = 2; break;
		case CP_POLY_SHAPE: count1 = ((cpPolyShape *)ctx->shape1)->count; break;
		default: break;
	}
	
	switch(ctx->shape2->klass->type){
		case CP_SEGMENT_SHAPE: count1 = 2; break;
		case CP_POLY_SHAPE: count2 = ((cpPolyShape *)ctx->shape2)->count; break;
		default: break;
	}
	
	
	// draw the minkowski difference origin
	phy_vect origin = cpvzero;
	ChipmunkDebugDrawDot(5.0, origin, RGBAColor(1,0,0,1));
	
	int mdiffCount = count1*count2;
	phy_vect *mdiffVerts = alloca(mdiffCount*sizeof(phy_vect));
	
	for(int i=0; i<count1; i++){
		for(int j=0; j<count2; j++){
			phy_vect v = cpvsub(ShapePoint(ctx->shape2, j).p, ShapePoint(ctx->shape1, i).p);
			mdiffVerts[i*count2 + j] = v;
			ChipmunkDebugDrawDot(2.0, v, RGBAColor(1, 0, 0, 1));
		}
	}
	 
	phy_vect *hullVerts = alloca(mdiffCount*sizeof(phy_vect));
	int hullCount = cpConvexHull(mdiffCount, mdiffVerts, hullVerts, NULL, 0.0);
	
	ChipmunkDebugDrawPolygon(hullCount, hullVerts, 0.0, RGBAColor(1, 0, 0, 1), RGBAColor(1, 0, 0, 0.25));
#endif
	
	struct MinkowskiPoint v0, v1;
	if(*id){
		// Use the minkowski points from the last frame as a starting point using the cached indexes.
		v0 = MinkowskiPointNew(ShapePoint(ctx->shape1, (*id>>24)&0xFF), ShapePoint(ctx->shape2, (*id>>16)&0xFF));
		v1 = MinkowskiPointNew(ShapePoint(ctx->shape1, (*id>> 8)&0xFF), ShapePoint(ctx->shape2, (*id    )&0xFF));
	} else {
		// No cached indexes, use the shapes' bounding box centers as a guess for a starting axis.
		phy_vect axis = cpvperp(cpvsub(phy_bb_center(ctx->shape1->bb), phy_bb_center(ctx->shape2->bb)));
		v0 = Support(ctx, axis);
		v1 = Support(ctx, cpvneg(axis));
	}
	
	struct ClosestPoints points = GJKRecurse(ctx, v0, v1, 1);
	*id = points.id;
	return points;
}

//MARK: Contact Clipping

// Given two support edges, find contact point pairs on their surfaces.
static inline void
ContactPoints(const struct Edge e1, const struct Edge e2, const struct ClosestPoints points, struct cpCollisionInfo *info)
{
	float mindist = e1.r + e2.r;
	if(points.d <= mindist){
#ifdef DRAW_CLIP
	ChipmunkDebugDrawFatSegment(e1.a.p, e1.b.p, e1.r, RGBAColor(0, 1, 0, 1), LAColor(0, 0));
	ChipmunkDebugDrawFatSegment(e2.a.p, e2.b.p, e2.r, RGBAColor(1, 0, 0, 1), LAColor(0, 0));
#endif
		phy_vect n = info->n = points.n;
		
		// Distances along the axis parallel to n
		float d_e1_a = cpvcross(e1.a.p, n);
		float d_e1_b = cpvcross(e1.b.p, n);
		float d_e2_a = cpvcross(e2.a.p, n);
		float d_e2_b = cpvcross(e2.b.p, n);
		
		// TODO + min isn't a complete fix.
		float e1_denom = 1.0f/(d_e1_b - d_e1_a + FLT_MIN);
		float e2_denom = 1.0f/(d_e2_b - d_e2_a + FLT_MIN);
		
		// Project the endpoints of the two edges onto the opposing edge, clamping them as necessary.
		// Compare the projected points to the collision normal to see if the shapes overlap there.
		{
			phy_vect p1 = cpvadd(cpvmult(n,  e1.r), cpvlerp(e1.a.p, e1.b.p, phy_clamp01((d_e2_b - d_e1_a)*e1_denom)));
			phy_vect p2 = cpvadd(cpvmult(n, -e2.r), cpvlerp(e2.a.p, e2.b.p, phy_clamp01((d_e1_a - d_e2_a)*e2_denom)));
			float dist = cpvdot(cpvsub(p2, p1), n);
			if(dist <= 0.0f){
				phy_hash_value hash_1a2b = CP_HASH_PAIR(e1.a.hash, e2.b.hash);
				cpCollisionInfoPushContact(info, p1, p2, hash_1a2b);
			}
		}{
			phy_vect p1 = cpvadd(cpvmult(n,  e1.r), cpvlerp(e1.a.p, e1.b.p, phy_clamp01((d_e2_a - d_e1_a)*e1_denom)));
			phy_vect p2 = cpvadd(cpvmult(n, -e2.r), cpvlerp(e2.a.p, e2.b.p, phy_clamp01((d_e1_b - d_e2_a)*e2_denom)));
			float dist = cpvdot(cpvsub(p2, p1), n);
			if(dist <= 0.0f){
				phy_hash_value hash_1b2a = CP_HASH_PAIR(e1.b.hash, e2.a.hash);
				cpCollisionInfoPushContact(info, p1, p2, hash_1b2a);
			}
		}
	}
}

//MARK: Collision Functions

typedef void (*CollisionFunc)(const phy_shape *a, const phy_shape *b, struct cpCollisionInfo *info);

// Collide circle shapes.
static void
CircleToCircle(const phy_circle_shape *c1, const phy_circle_shape *c2, struct cpCollisionInfo *info)
{
	float mindist = c1->r + c2->r;
	phy_vect delta = cpvsub(c2->tc, c1->tc);
	float distsq = cpvlengthsq(delta);
	
	if(distsq < mindist*mindist){
		float dist = sqrtf(distsq);
		phy_vect n = info->n = (dist ? cpvmult(delta, 1.0f/dist) : cpv(1.0f, 0.0f));
		cpCollisionInfoPushContact(info, cpvadd(c1->tc, cpvmult(n, c1->r)), cpvadd(c2->tc, cpvmult(n, -c2->r)), 0);
	}
}

static void
CircleToSegment(const phy_circle_shape *circle, const phy_segment_shape *segment, struct cpCollisionInfo *info)
{
	phy_vect seg_a = segment->ta;
	phy_vect seg_b = segment->tb;
	phy_vect center = circle->tc;
	
	// Find the closest point on the segment to the circle.
	phy_vect seg_delta = cpvsub(seg_b, seg_a);
	float closest_t = phy_clamp01(cpvdot(seg_delta, cpvsub(center, seg_a))/cpvlengthsq(seg_delta));
	phy_vect closest = cpvadd(seg_a, cpvmult(seg_delta, closest_t));
	
	// Compare the radii of the two shapes to see if they are colliding.
	float mindist = circle->r + segment->r;
	phy_vect delta = cpvsub(closest, center);
	float distsq = cpvlengthsq(delta);
	if(distsq < mindist*mindist){
		float dist = sqrtf(distsq);
		// Handle coincident shapes as gracefully as possible.
		phy_vect n = info->n = (dist ? cpvmult(delta, 1.0f/dist) : segment->tn);
		
		// Reject endcap collisions if tangents are provided.
		phy_vect rot = phy_body_get_rotation(segment->shape.body);
		if(
			(closest_t != 0.0f || cpvdot(n, cpvrotate(segment->a_tangent, rot)) >= 0.0) &&
			(closest_t != 1.0f || cpvdot(n, cpvrotate(segment->b_tangent, rot)) >= 0.0)
		){
			cpCollisionInfoPushContact(info, cpvadd(center, cpvmult(n, circle->r)), cpvadd(closest, cpvmult(n, -segment->r)), 0);
		}
	}
}

static void
SegmentToSegment(const phy_segment_shape *seg1, const phy_segment_shape *seg2, struct cpCollisionInfo *info)
{
	struct SupportContext context = {(phy_shape *)seg1, (phy_shape *)seg2, (SupportPointFunc)SegmentSupportPoint, (SupportPointFunc)SegmentSupportPoint};
	struct ClosestPoints points = GJK(&context, &info->id);
	
#if DRAW_CLOSEST
#if PRINT_LOG
//	ChipmunkDemoPrintString("Distance: %.2f\n", points.d);
#endif
	
	ChipmunkDebugDrawDot(6.0, points.a, RGBAColor(1, 1, 1, 1));
	ChipmunkDebugDrawDot(6.0, points.b, RGBAColor(1, 1, 1, 1));
	ChipmunkDebugDrawSegment(points.a, points.b, RGBAColor(1, 1, 1, 1));
	ChipmunkDebugDrawSegment(points.a, cpvadd(points.a, cpvmult(points.n, 10.0)), RGBAColor(1, 0, 0, 1));
#endif
	
	phy_vect n = points.n;
	phy_vect rot1 = phy_body_get_rotation(seg1->shape.body);
	phy_vect rot2 = phy_body_get_rotation(seg2->shape.body);
	
	// If the closest points are nearer than the sum of the radii...
	if(
		points.d <= (seg1->r + seg2->r) && (
			// Reject endcap collisions if tangents are provided.
			(!cpveql(points.a, seg1->ta) || cpvdot(n, cpvrotate(seg1->a_tangent, rot1)) <= 0.0) &&
			(!cpveql(points.a, seg1->tb) || cpvdot(n, cpvrotate(seg1->b_tangent, rot1)) <= 0.0) &&
			(!cpveql(points.b, seg2->ta) || cpvdot(n, cpvrotate(seg2->a_tangent, rot2)) >= 0.0) &&
			(!cpveql(points.b, seg2->tb) || cpvdot(n, cpvrotate(seg2->b_tangent, rot2)) >= 0.0)
		)
	){
		ContactPoints(SupportEdgeForSegment(seg1, n), SupportEdgeForSegment(seg2, cpvneg(n)), points, info);
	}
}

static void
PolyToPoly(const phy_poly_shape *poly1, const phy_poly_shape *poly2, struct cpCollisionInfo *info)
{
	struct SupportContext context = {(phy_shape *)poly1, (phy_shape *)poly2, (SupportPointFunc)PolySupportPoint, (SupportPointFunc)PolySupportPoint};
	struct ClosestPoints points = GJK(&context, &info->id);
	
#if DRAW_CLOSEST
#if PRINT_LOG
//	ChipmunkDemoPrintString("Distance: %.2f\n", points.d);
#endif
	
	ChipmunkDebugDrawDot(3.0, points.a, RGBAColor(1, 1, 1, 1));
	ChipmunkDebugDrawDot(3.0, points.b, RGBAColor(1, 1, 1, 1));
	ChipmunkDebugDrawSegment(points.a, points.b, RGBAColor(1, 1, 1, 1));
	ChipmunkDebugDrawSegment(points.a, cpvadd(points.a, cpvmult(points.n, 10.0)), RGBAColor(1, 0, 0, 1));
#endif
	
	// If the closest points are nearer than the sum of the radii...
	if(points.d - poly1->r - poly2->r <= 0.0){
		ContactPoints(SupportEdgeForPoly(poly1, points.n), SupportEdgeForPoly(poly2, cpvneg(points.n)), points, info);
	}
}

static void
SegmentToPoly(const phy_segment_shape *seg, const phy_poly_shape *poly, struct cpCollisionInfo *info)
{
	struct SupportContext context = {(phy_shape *)seg, (phy_shape *)poly, (SupportPointFunc)SegmentSupportPoint, (SupportPointFunc)PolySupportPoint};
	struct ClosestPoints points = GJK(&context, &info->id);
	
#if DRAW_CLOSEST
#if PRINT_LOG
//	ChipmunkDemoPrintString("Distance: %.2f\n", points.d);
#endif
	
	ChipmunkDebugDrawDot(3.0, points.a, RGBAColor(1, 1, 1, 1));
	ChipmunkDebugDrawDot(3.0, points.b, RGBAColor(1, 1, 1, 1));
	ChipmunkDebugDrawSegment(points.a, points.b, RGBAColor(1, 1, 1, 1));
	ChipmunkDebugDrawSegment(points.a, cpvadd(points.a, cpvmult(points.n, 10.0)), RGBAColor(1, 0, 0, 1));
#endif
	
	phy_vect n = points.n;
	phy_vect rot = phy_body_get_rotation(seg->shape.body);
	
	if(
		// If the closest points are nearer than the sum of the radii...
		points.d - seg->r - poly->r <= 0.0 && (
			// Reject endcap collisions if tangents are provided.
			(!cpveql(points.a, seg->ta) || cpvdot(n, cpvrotate(seg->a_tangent, rot)) <= 0.0) &&
			(!cpveql(points.a, seg->tb) || cpvdot(n, cpvrotate(seg->b_tangent, rot)) <= 0.0)
		)
	){
		ContactPoints(SupportEdgeForSegment(seg, n), SupportEdgeForPoly(poly, cpvneg(n)), points, info);
	}
}

static void
CircleToPoly(const phy_circle_shape *circle, const phy_poly_shape *poly, struct cpCollisionInfo *info)
{
	struct SupportContext context = {(phy_shape *)circle, (phy_shape *)poly, (SupportPointFunc)CircleSupportPoint, (SupportPointFunc)PolySupportPoint};
	struct ClosestPoints points = GJK(&context, &info->id);
	
#if DRAW_CLOSEST
	ChipmunkDebugDrawDot(3.0, points.a, RGBAColor(1, 1, 1, 1));
	ChipmunkDebugDrawDot(3.0, points.b, RGBAColor(1, 1, 1, 1));
	ChipmunkDebugDrawSegment(points.a, points.b, RGBAColor(1, 1, 1, 1));
	ChipmunkDebugDrawSegment(points.a, cpvadd(points.a, cpvmult(points.n, 10.0)), RGBAColor(1, 0, 0, 1));
#endif
	
	// If the closest points are nearer than the sum of the radii...
	if(points.d <= circle->r + poly->r){
		phy_vect n = info->n = points.n;
		cpCollisionInfoPushContact(info, cpvadd(points.a, cpvmult(n, circle->r)), cpvadd(points.b, cpvmult(n, -poly->r)), 0);
	}
}

static void
CollisionError(const phy_shape *circle, const phy_shape *poly, struct cpCollisionInfo *info)
{
	utl_error_func("Shape types are not sorted", utl_user_defined_data);
}

static const CollisionFunc BuiltinCollisionFuncs[9] = {
	(CollisionFunc)CircleToCircle,
	CollisionError,
	CollisionError,
	(CollisionFunc)CircleToSegment,
	(CollisionFunc)SegmentToSegment,
	CollisionError,
	(CollisionFunc)CircleToPoly,
	(CollisionFunc)SegmentToPoly,
	(CollisionFunc)PolyToPoly,
};
static const CollisionFunc *CollisionFuncs = BuiltinCollisionFuncs;

struct cpCollisionInfo
cpCollide(const phy_shape *a, const phy_shape *b, phy_collision_id id, struct cpContact *contacts)
{
	struct cpCollisionInfo info = {a, b, id, cpvzero, 0, contacts};
	
	// Make sure the shape types are in order.
	if(a->klass->type > b->klass->type){
		info.a = b;
		info.b = a;
	}
	
	CollisionFuncs[info.a->klass->type + info.b->klass->type*CP_NUM_SHAPES](info.a, info.b, &info);
	
//	if(0){
//		for(int i=0; i<info.count; i++){
//			phy_vect r1 = info.arr[i].r1;
//			phy_vect r2 = info.arr[i].r2;
//			phy_vect mid = cpvlerp(r1, r2, 0.5f);
//			
//			ChipmunkDebugDrawSegment(r1, mid, RGBAColor(1, 0, 0, 1));
//			ChipmunkDebugDrawSegment(r2, mid, RGBAColor(0, 0, 1, 1));
//		}
//	}
	
	return info;
}
