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

//MARK: Nearest Point Query Functions

struct PointQueryContext {
	phy_vect point;
	float maxDistance;
	cpShapeFilter filter;
	cpSpacePointQueryFunc func;
};

static phy_collision_id
NearestPointQuery(struct PointQueryContext *context, phy_shape *shape, phy_collision_id id, void *data)
{
	if(
		!cpShapeFilterReject(shape->filter, context->filter)
	){
		cpPointQueryInfo info;
		cpShapePointQuery(shape, context->point, &info);
		
		if(info.shape && info.distance < context->maxDistance) context->func(shape, info.point, info.distance, info.gradient, data);
	}
	
	return id;
}

void
cpSpacePointQuery(phy_space *space, phy_vect point, float maxDistance, cpShapeFilter filter, cpSpacePointQueryFunc func, void *data)
{
	struct PointQueryContext context = {point, maxDistance, filter, func};
	cpBB bb = cpBBNewForCircle(point, phy_max(maxDistance, 0.0f));
	
	cpSpaceLock(space); {
		cpSpatialIndexQuery(space->dynamicShapes, &context, bb, (cpSpatialIndexQueryFunc)NearestPointQuery, data);
		cpSpatialIndexQuery(space->staticShapes, &context, bb, (cpSpatialIndexQueryFunc)NearestPointQuery, data);
	} cpSpaceUnlock(space, true);
}

static phy_collision_id
NearestPointQueryNearest(struct PointQueryContext *context, phy_shape *shape, phy_collision_id id, cpPointQueryInfo *out)
{
	if(
		!cpShapeFilterReject(shape->filter, context->filter) && !shape->sensor
	){
		cpPointQueryInfo info;
		cpShapePointQuery(shape, context->point, &info);
		
		if(info.distance < out->distance) (*out) = info;
	}
	
	return id;
}

phy_shape *
cpSpacePointQueryNearest(phy_space *space, phy_vect point, float maxDistance, cpShapeFilter filter, cpPointQueryInfo *out)
{
	cpPointQueryInfo info = {NULL, cpvzero, maxDistance, cpvzero};
	if(out){
		(*out) = info;
  } else {
		out = &info;
	}
	
	struct PointQueryContext context = {
		point, maxDistance,
		filter,
		NULL
	};
	
	cpBB bb = cpBBNewForCircle(point, phy_max(maxDistance, 0.0f));
	cpSpatialIndexQuery(space->dynamicShapes, &context, bb, (cpSpatialIndexQueryFunc)NearestPointQueryNearest, out);
	cpSpatialIndexQuery(space->staticShapes, &context, bb, (cpSpatialIndexQueryFunc)NearestPointQueryNearest, out);
	
	return (phy_shape *)out->shape;
}


//MARK: Segment Query Functions

struct SegmentQueryContext {
	phy_vect start, end;
	float radius;
	cpShapeFilter filter;
	cpSpaceSegmentQueryFunc func;
};

static float
SegmentQuery(struct SegmentQueryContext *context, phy_shape *shape, void *data)
{
	cpSegmentQueryInfo info;
	
	if(
		!cpShapeFilterReject(shape->filter, context->filter) &&
		cpShapeSegmentQuery(shape, context->start, context->end, context->radius, &info)
	){
		context->func(shape, info.point, info.normal, info.alpha, data);
	}
	
	return 1.0f;
}

void
cpSpaceSegmentQuery(phy_space *space, phy_vect start, phy_vect end, float radius, cpShapeFilter filter, cpSpaceSegmentQueryFunc func, void *data)
{
	struct SegmentQueryContext context = {
		start, end,
		radius,
		filter,
		func,
	};
	
	cpSpaceLock(space); {
    cpSpatialIndexSegmentQuery(space->staticShapes, &context, start, end, 1.0f, (cpSpatialIndexSegmentQueryFunc)SegmentQuery, data);
    cpSpatialIndexSegmentQuery(space->dynamicShapes, &context, start, end, 1.0f, (cpSpatialIndexSegmentQueryFunc)SegmentQuery, data);
	} cpSpaceUnlock(space, true);
}

static float
SegmentQueryFirst(struct SegmentQueryContext *context, phy_shape *shape, cpSegmentQueryInfo *out)
{
	cpSegmentQueryInfo info;
	
	if(
		!cpShapeFilterReject(shape->filter, context->filter) && !shape->sensor &&
		cpShapeSegmentQuery(shape, context->start, context->end, context->radius, &info) &&
		info.alpha < out->alpha
	){
		(*out) = info;
	}
	
	return out->alpha;
}

phy_shape *
cpSpaceSegmentQueryFirst(phy_space *space, phy_vect start, phy_vect end, float radius, cpShapeFilter filter, cpSegmentQueryInfo *out)
{
	cpSegmentQueryInfo info = {NULL, end, cpvzero, 1.0f};
	if(out){
		(*out) = info;
  } else {
		out = &info;
	}
	
	struct SegmentQueryContext context = {
		start, end,
		radius,
		filter,
		NULL
	};
	
	cpSpatialIndexSegmentQuery(space->staticShapes, &context, start, end, 1.0f, (cpSpatialIndexSegmentQueryFunc)SegmentQueryFirst, out);
	cpSpatialIndexSegmentQuery(space->dynamicShapes, &context, start, end, out->alpha, (cpSpatialIndexSegmentQueryFunc)SegmentQueryFirst, out);
	
	return (phy_shape *)out->shape;
}

//MARK: BB Query Functions

struct BBQueryContext {
	cpBB bb;
	cpShapeFilter filter;
	cpSpaceBBQueryFunc func;
};

static phy_collision_id
BBQuery(struct BBQueryContext *context, phy_shape *shape, phy_collision_id id, void *data)
{
	if(
		!cpShapeFilterReject(shape->filter, context->filter) &&
		cpBBIntersects(context->bb, shape->bb)
	){
		context->func(shape, data);
	}
	
	return id;
}

void
cpSpaceBBQuery(phy_space *space, cpBB bb, cpShapeFilter filter, cpSpaceBBQueryFunc func, void *data)
{
	struct BBQueryContext context = {bb, filter, func};
	
	cpSpaceLock(space); {
    cpSpatialIndexQuery(space->dynamicShapes, &context, bb, (cpSpatialIndexQueryFunc)BBQuery, data);
    cpSpatialIndexQuery(space->staticShapes, &context, bb, (cpSpatialIndexQueryFunc)BBQuery, data);
	} cpSpaceUnlock(space, true);
}

//MARK: Shape Query Functions

struct ShapeQueryContext {
	cpSpaceShapeQueryFunc func;
	void *data;
	bool anyCollision;
};

// Callback from the spatial hash.
static phy_collision_id
ShapeQuery(phy_shape *a, phy_shape *b, phy_collision_id id, struct ShapeQueryContext *context)
{
	if(cpShapeFilterReject(a->filter, b->filter) || a == b) return id;
	
	phy_contact_point_set set = cpShapesCollide(a, b);
	if(set.count){
		if(context->func) context->func(b, &set, context->data);
		context->anyCollision = !(a->sensor || b->sensor);
	}
	
	return id;
}

bool
cpSpaceShapeQuery(phy_space *space, phy_shape *shape, cpSpaceShapeQueryFunc func, void *data)
{
	phy_body *body = shape->body;
	cpBB bb = (body ? cpShapeUpdate(shape, body->transform) : shape->bb);
	struct ShapeQueryContext context = {func, data, false};
	
	cpSpaceLock(space); {
    cpSpatialIndexQuery(space->dynamicShapes, shape, bb, (cpSpatialIndexQueryFunc)ShapeQuery, &context);
    cpSpatialIndexQuery(space->staticShapes, shape, bb, (cpSpatialIndexQueryFunc)ShapeQuery, &context);
	} cpSpaceUnlock(space, true);
	
	return context.anyCollision;
}
