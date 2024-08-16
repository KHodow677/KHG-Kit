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

#include "khg_phy/phy_private.h"
#include "khg_phy/phy_unsafe.h"

phy_poly_shape *
cpPolyShapeAlloc(void)
{
	return (phy_poly_shape *)calloc(1, sizeof(phy_poly_shape));
}

static void
cpPolyShapeDestroy(phy_poly_shape *poly)
{
	if(poly->count > CP_POLY_SHAPE_INLINE_ALLOC){
		free(poly->planes);
	}
}

static cpBB
cpPolyShapeCacheData(phy_poly_shape *poly, phy_transform transform)
{
	int count = poly->count;
	struct cpSplittingPlane *dst = poly->planes;
	struct cpSplittingPlane *src = dst + count;
	
	float l = (float)INFINITY, r = -(float)INFINITY;
	float b = (float)INFINITY, t = -(float)INFINITY;
	
	for(int i=0; i<count; i++){
		phy_vect v = cpTransformPoint(transform, src[i].v0);
		phy_vect n = cpTransformVect(transform, src[i].n);
		
		dst[i].v0 = v;
		dst[i].n = n;
		
		l = phy_min(l, v.x);
		r = phy_max(r, v.x);
		b = phy_min(b, v.y);
		t = phy_max(t, v.y);
	}
	
	float radius = poly->r;
	return (poly->shape.bb = cpBBNew(l - radius, b - radius, r + radius, t + radius));
}

static void
cpPolyShapePointQuery(phy_poly_shape *poly, phy_vect p, cpPointQueryInfo *info){
	int count = poly->count;
	struct cpSplittingPlane *planes = poly->planes;
	float r = poly->r;
	
	phy_vect v0 = planes[count - 1].v0;
	float minDist = INFINITY;
	phy_vect closestPoint = cpvzero;
	phy_vect closestNormal = cpvzero;
	bool outside = false;
	
	for(int i=0; i<count; i++){
		phy_vect v1 = planes[i].v0;
		outside = outside || (cpvdot(planes[i].n, cpvsub(p,v1)) > 0.0f);
		
		phy_vect closest = cpClosetPointOnSegment(p, v0, v1);
		
		float dist = cpvdist(p, closest);
		if(dist < minDist){
			minDist = dist;
			closestPoint = closest;
			closestNormal = planes[i].n;
		}
		
		v0 = v1;
	}
	
	float dist = (outside ? minDist : -minDist);
	phy_vect g = cpvmult(cpvsub(p, closestPoint), 1.0f/dist);
	
	info->shape = (phy_shape *)poly;
	info->point = cpvadd(closestPoint, cpvmult(g, r));
	info->distance = dist - r;
	
	// Use the normal of the closest segment if the distance is small.
	info->gradient = (minDist > MAGIC_EPSILON ? g : closestNormal);
}

static void
cpPolyShapeSegmentQuery(phy_poly_shape *poly, phy_vect a, phy_vect b, float r2, cpSegmentQueryInfo *info)
{
	struct cpSplittingPlane *planes = poly->planes;
	int count = poly->count;
	float r = poly->r;
	float rsum = r + r2;
	
	for(int i=0; i<count; i++){
		phy_vect n = planes[i].n;
		float an = cpvdot(a, n);
		float d =  an - cpvdot(planes[i].v0, n) - rsum;
		if(d < 0.0f) continue;
		
		float bn = cpvdot(b, n);
		// Avoid divide by zero. (d is always positive)
		float t = d/phy_max(an - bn, FLT_MIN);
		if(t < 0.0f || 1.0f < t) continue;
		
		phy_vect point = cpvlerp(a, b, t);
		float dt = cpvcross(n, point);
		float dtMin = cpvcross(n, planes[(i - 1 + count)%count].v0);
		float dtMax = cpvcross(n, planes[i].v0);
		
		if(dtMin <= dt && dt <= dtMax){
			info->shape = (phy_shape *)poly;
			info->point = cpvsub(cpvlerp(a, b, t), cpvmult(n, r2));
			info->normal = n;
			info->alpha = t;
		}
	}
	
	// Also check against the beveled vertexes.
	if(rsum > 0.0f){
		for(int i=0; i<count; i++){
			cpSegmentQueryInfo circle_info = {NULL, b, cpvzero, 1.0f};
			CircleSegmentQuery(&poly->shape, planes[i].v0, r, a, b, r2, &circle_info);
			if(circle_info.alpha < info->alpha) (*info) = circle_info;
		}
	}
}

static void
SetVerts(phy_poly_shape *poly, int count, const phy_vect *verts)
{
	poly->count = count;
	if(count <= CP_POLY_SHAPE_INLINE_ALLOC){
		poly->planes = poly->_planes;
	} else {
		poly->planes = (struct cpSplittingPlane *)calloc(2*count, sizeof(struct cpSplittingPlane));
	}
	
	for(int i=0; i<count; i++){
		phy_vect a = verts[(i - 1 + count)%count];
		phy_vect b = verts[i];
		phy_vect n = cpvnormalize(cpvrperp(cpvsub(b, a)));
		
		poly->planes[i + count].v0 = b;
		poly->planes[i + count].n = n;
	}
}

static struct cpShapeMassInfo
cpPolyShapeMassInfo(float mass, int count, const phy_vect *verts, float radius)
{
	// TODO moment is approximate due to radius.
	
	phy_vect centroid = cpCentroidForPoly(count, verts);
	struct cpShapeMassInfo info = {
		mass, cpMomentForPoly(1.0f, count, verts, cpvneg(centroid), radius),
		centroid,
		cpAreaForPoly(count, verts, radius),
	};
	
	return info;
}

static const cpShapeClass polyClass = {
	CP_POLY_SHAPE,
	(cpShapeCacheDataImpl)cpPolyShapeCacheData,
	(cpShapeDestroyImpl)cpPolyShapeDestroy,
	(cpShapePointQueryImpl)cpPolyShapePointQuery,
	(cpShapeSegmentQueryImpl)cpPolyShapeSegmentQuery,
};

phy_poly_shape *
cpPolyShapeInit(phy_poly_shape *poly, phy_body *body, int count, const phy_vect *verts, phy_transform transform, float radius)
{
	phy_vect *hullVerts = (phy_vect *)alloca(count*sizeof(phy_vect));
	
	// Transform the verts before building the hull in case of a negative scale.
	for(int i=0; i<count; i++) hullVerts[i] = cpTransformPoint(transform, verts[i]);
	
	unsigned int hullCount = cpConvexHull(count, hullVerts, hullVerts, NULL, 0.0);
	return cpPolyShapeInitRaw(poly, body, hullCount, hullVerts, radius);
}

phy_poly_shape *
cpPolyShapeInitRaw(phy_poly_shape *poly, phy_body *body, int count, const phy_vect *verts, float radius)
{
	cpShapeInit((phy_shape *)poly, &polyClass, body, cpPolyShapeMassInfo(0.0f, count, verts, radius));
	
	SetVerts(poly, count, verts);
	poly->r = radius;

	return poly;
}

phy_shape *
cpPolyShapeNew(phy_body *body, int count, const phy_vect *verts, phy_transform transform, float radius)
{
	return (phy_shape *)cpPolyShapeInit(cpPolyShapeAlloc(), body, count, verts, transform, radius);
}

phy_shape *
cpPolyShapeNewRaw(phy_body *body, int count, const phy_vect *verts, float radius)
{
	return (phy_shape *)cpPolyShapeInitRaw(cpPolyShapeAlloc(), body, count, verts, radius);
}

phy_poly_shape *
cpBoxShapeInit(phy_poly_shape *poly, phy_body *body, float width, float height, float radius)
{
	float hw = width/2.0f;
	float hh = height/2.0f;
	
	return cpBoxShapeInit2(poly, body, cpBBNew(-hw, -hh, hw, hh), radius);
}

phy_poly_shape *
cpBoxShapeInit2(phy_poly_shape *poly, phy_body *body, cpBB box, float radius)
{
	phy_vect verts[4] = {
		cpv(box.r, box.b),
		cpv(box.r, box.t),
		cpv(box.l, box.t),
		cpv(box.l, box.b),
	};
	
	return cpPolyShapeInitRaw(poly, body, 4, verts, radius);
}

phy_shape *
cpBoxShapeNew(phy_body *body, float width, float height, float radius)
{
	return (phy_shape *)cpBoxShapeInit(cpPolyShapeAlloc(), body, width, height, radius);
}

phy_shape *
cpBoxShapeNew2(phy_body *body, cpBB box, float radius)
{
	return (phy_shape *)cpBoxShapeInit2(cpPolyShapeAlloc(), body, box, radius);
}

int
cpPolyShapeGetCount(const phy_shape *shape)
{
	cpAssertHard(shape->klass == &polyClass, "Shape is not a poly shape.");
	return ((phy_poly_shape *)shape)->count;
}

phy_vect
cpPolyShapeGetVert(const phy_shape *shape, int i)
{
	cpAssertHard(shape->klass == &polyClass, "Shape is not a poly shape.");
	
	int count = cpPolyShapeGetCount(shape);
	cpAssertHard(0 <= i && i < count, "Index out of range.");
	
	return ((phy_poly_shape *)shape)->planes[i + count].v0;
}

float
cpPolyShapeGetRadius(const phy_shape *shape)
{
	cpAssertHard(shape->klass == &polyClass, "Shape is not a poly shape.");
	return ((phy_poly_shape *)shape)->r;
}

// Unsafe API (chipmunk_unsafe.h)

void
cpPolyShapeSetVerts(phy_shape *shape, int count, phy_vect *verts, phy_transform transform)
{
	phy_vect *hullVerts = (phy_vect *)alloca(count*sizeof(phy_vect));
	
	// Transform the verts before building the hull in case of a negative scale.
	for(int i=0; i<count; i++) hullVerts[i] = cpTransformPoint(transform, verts[i]);
	
	unsigned int hullCount = cpConvexHull(count, hullVerts, hullVerts, NULL, 0.0);
	cpPolyShapeSetVertsRaw(shape, hullCount, hullVerts);
}

void
cpPolyShapeSetVertsRaw(phy_shape *shape, int count, phy_vect *verts)
{
	cpAssertHard(shape->klass == &polyClass, "Shape is not a poly shape.");
	phy_poly_shape *poly = (phy_poly_shape *)shape;
	cpPolyShapeDestroy(poly);
	
	SetVerts(poly, count, verts);
	
	float mass = shape->massInfo.m;
	shape->massInfo = cpPolyShapeMassInfo(shape->massInfo.m, count, verts, poly->r);
	if(mass > 0.0f) cpBodyAccumulateMassFromShapes(shape->body);
}

void
cpPolyShapeSetRadius(phy_shape *shape, float radius)
{
	cpAssertHard(shape->klass == &polyClass, "Shape is not a poly shape.");
	phy_poly_shape *poly = (phy_poly_shape *)shape;
	poly->r = radius;
	
	
	// TODO radius is not handled by moment/area
//	float mass = shape->massInfo.m;
//	shape->massInfo = cpPolyShapeMassInfo(shape->massInfo.m, poly->count, poly->verts, poly->r);
//	if(mass > 0.0f) cpBodyAccumulateMassFromShapes(shape->body);
}
