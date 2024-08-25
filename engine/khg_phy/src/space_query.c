#include "khg_phy/arbiter.h"
#include "khg_phy/phy_private.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/shape.h"
#include <stdbool.h>
#include <stdlib.h>

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
		!phy_shape_filter_reject(shape->filter, context->filter)
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
	phy_bb bb = phy_bb_new_for_circle(point, phy_max(maxDistance, 0.0f));
	
	phy_space_lock(space); {
		cpSpatialIndexQuery(space->dynamic_shapes, &context, bb, (cpSpatialIndexQueryFunc)NearestPointQuery, data);
		cpSpatialIndexQuery(space->static_shapes, &context, bb, (cpSpatialIndexQueryFunc)NearestPointQuery, data);
	} phy_space_unlock(space, true);
}

static phy_collision_id
NearestPointQueryNearest(struct PointQueryContext *context, phy_shape *shape, phy_collision_id id, cpPointQueryInfo *out)
{
	if(
		!phy_shape_filter_reject(shape->filter, context->filter) && !shape->sensor
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
	
	phy_bb bb = phy_bb_new_for_circle(point, phy_max(maxDistance, 0.0f));
	cpSpatialIndexQuery(space->dynamic_shapes, &context, bb, (cpSpatialIndexQueryFunc)NearestPointQueryNearest, out);
	cpSpatialIndexQuery(space->static_shapes, &context, bb, (cpSpatialIndexQueryFunc)NearestPointQueryNearest, out);
	
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
		!phy_shape_filter_reject(shape->filter, context->filter) &&
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
	
	phy_space_lock(space); {
    cpSpatialIndexSegmentQuery(space->static_shapes, &context, start, end, 1.0f, (cpSpatialIndexSegmentQueryFunc)SegmentQuery, data);
    cpSpatialIndexSegmentQuery(space->dynamic_shapes, &context, start, end, 1.0f, (cpSpatialIndexSegmentQueryFunc)SegmentQuery, data);
	} phy_space_unlock(space, true);
}

static float
SegmentQueryFirst(struct SegmentQueryContext *context, phy_shape *shape, cpSegmentQueryInfo *out)
{
	cpSegmentQueryInfo info;
	
	if(
		!phy_shape_filter_reject(shape->filter, context->filter) && !shape->sensor &&
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
	
	cpSpatialIndexSegmentQuery(space->static_shapes, &context, start, end, 1.0f, (cpSpatialIndexSegmentQueryFunc)SegmentQueryFirst, out);
	cpSpatialIndexSegmentQuery(space->dynamic_shapes, &context, start, end, out->alpha, (cpSpatialIndexSegmentQueryFunc)SegmentQueryFirst, out);
	
	return (phy_shape *)out->shape;
}

//MARK: BB Query Functions

struct BBQueryContext {
	phy_bb bb;
	cpShapeFilter filter;
	cpSpaceBBQueryFunc func;
};

static phy_collision_id
BBQuery(struct BBQueryContext *context, phy_shape *shape, phy_collision_id id, void *data)
{
	if(
		!phy_shape_filter_reject(shape->filter, context->filter) &&
		phy_bb_intersects(context->bb, shape->bb)
	){
		context->func(shape, data);
	}
	
	return id;
}

void
cpSpaceBBQuery(phy_space *space, phy_bb bb, cpShapeFilter filter, cpSpaceBBQueryFunc func, void *data)
{
	struct BBQueryContext context = {bb, filter, func};
	
	phy_space_lock(space); {
    cpSpatialIndexQuery(space->dynamic_shapes, &context, bb, (cpSpatialIndexQueryFunc)BBQuery, data);
    cpSpatialIndexQuery(space->static_shapes, &context, bb, (cpSpatialIndexQueryFunc)BBQuery, data);
	} phy_space_unlock(space, true);
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
	if(phy_shape_filter_reject(a->filter, b->filter) || a == b) return id;

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
	phy_bb bb = (body ? cpShapeUpdate(shape, body->transform) : shape->bb);
	struct ShapeQueryContext context = {func, data, false};
	
	phy_space_lock(space); {
    cpSpatialIndexQuery(space->dynamic_shapes, shape, bb, (cpSpatialIndexQueryFunc)ShapeQuery, &context);
    cpSpatialIndexQuery(space->static_shapes, shape, bb, (cpSpatialIndexQueryFunc)ShapeQuery, &context);
	} phy_space_unlock(space, true);
	
	return context.anyCollision;
}
