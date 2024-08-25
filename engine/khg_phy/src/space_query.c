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
	phy_shape_filter filter;
	phy_space_point_query_func func;
};

static phy_collision_id
NearestPointQuery(struct PointQueryContext *context, phy_shape *shape, phy_collision_id id, void *data)
{
	if(
		!phy_shape_filter_reject(shape->filter, context->filter)
	){
		phy_point_query_info info;
		phy_shape_point_query(shape, context->point, &info);
		
		if(info.shape && info.distance < context->maxDistance) context->func(shape, info.point, info.distance, info.gradient, data);
	}
	
	return id;
}

void
phy_space_point_query(phy_space *space, phy_vect point, float maxDistance, phy_shape_filter filter, phy_space_point_query_func func, void *data)
{
	struct PointQueryContext context = {point, maxDistance, filter, func};
	phy_bb bb = phy_bb_new_for_circle(point, phy_max(maxDistance, 0.0f));
	
	phy_space_lock(space); {
		phy_spatial_index_query(space->dynamic_shapes, &context, bb, (phy_spatial_index_query_func)NearestPointQuery, data);
		phy_spatial_index_query(space->static_shapes, &context, bb, (phy_spatial_index_query_func)NearestPointQuery, data);
	} phy_space_unlock(space, true);
}

static phy_collision_id
NearestPointQueryNearest(struct PointQueryContext *context, phy_shape *shape, phy_collision_id id, phy_point_query_info *out)
{
	if(
		!phy_shape_filter_reject(shape->filter, context->filter) && !shape->sensor
	){
		phy_point_query_info info;
		phy_shape_point_query(shape, context->point, &info);
		
		if(info.distance < out->distance) (*out) = info;
	}
	
	return id;
}

phy_shape *
phy_space_point_query_nearest(phy_space *space, phy_vect point, float maxDistance, phy_shape_filter filter, phy_point_query_info *out)
{
	phy_point_query_info info = {NULL, phy_v_zero, maxDistance, phy_v_zero};
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
	phy_spatial_index_query(space->dynamic_shapes, &context, bb, (phy_spatial_index_query_func)NearestPointQueryNearest, out);
	phy_spatial_index_query(space->static_shapes, &context, bb, (phy_spatial_index_query_func)NearestPointQueryNearest, out);
	
	return (phy_shape *)out->shape;
}


//MARK: Segment Query Functions

struct SegmentQueryContext {
	phy_vect start, end;
	float radius;
	phy_shape_filter filter;
	phy_space_segment_query_func func;
};

static float
SegmentQuery(struct SegmentQueryContext *context, phy_shape *shape, void *data)
{
	phy_segment_query_info info;
	
	if(
		!phy_shape_filter_reject(shape->filter, context->filter) &&
		phy_shape_segment_query(shape, context->start, context->end, context->radius, &info)
	){
		context->func(shape, info.point, info.normal, info.alpha, data);
	}
	
	return 1.0f;
}

void
phy_space_segment_query(phy_space *space, phy_vect start, phy_vect end, float radius, phy_shape_filter filter, phy_space_segment_query_func func, void *data)
{
	struct SegmentQueryContext context = {
		start, end,
		radius,
		filter,
		func,
	};
	
	phy_space_lock(space); {
    phy_spatial_index_segment_query(space->static_shapes, &context, start, end, 1.0f, (phy_spatial_index_segment_query_func)SegmentQuery, data);
    phy_spatial_index_segment_query(space->dynamic_shapes, &context, start, end, 1.0f, (phy_spatial_index_segment_query_func)SegmentQuery, data);
	} phy_space_unlock(space, true);
}

static float
SegmentQueryFirst(struct SegmentQueryContext *context, phy_shape *shape, phy_segment_query_info *out)
{
	phy_segment_query_info info;
	
	if(
		!phy_shape_filter_reject(shape->filter, context->filter) && !shape->sensor &&
		phy_shape_segment_query(shape, context->start, context->end, context->radius, &info) &&
		info.alpha < out->alpha
	){
		(*out) = info;
	}
	
	return out->alpha;
}

phy_shape *
phy_space_segment_query_first(phy_space *space, phy_vect start, phy_vect end, float radius, phy_shape_filter filter, phy_segment_query_info *out)
{
	phy_segment_query_info info = {NULL, end, phy_v_zero, 1.0f};
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
	
	phy_spatial_index_segment_query(space->static_shapes, &context, start, end, 1.0f, (phy_spatial_index_segment_query_func)SegmentQueryFirst, out);
	phy_spatial_index_segment_query(space->dynamic_shapes, &context, start, end, out->alpha, (phy_spatial_index_segment_query_func)SegmentQueryFirst, out);
	
	return (phy_shape *)out->shape;
}

//MARK: BB Query Functions

struct BBQueryContext {
	phy_bb bb;
	phy_shape_filter filter;
	phy_space_BB_query_func func;
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
phy_space_BB_query(phy_space *space, phy_bb bb, phy_shape_filter filter, phy_space_BB_query_func func, void *data)
{
	struct BBQueryContext context = {bb, filter, func};
	
	phy_space_lock(space); {
    phy_spatial_index_query(space->dynamic_shapes, &context, bb, (phy_spatial_index_query_func)BBQuery, data);
    phy_spatial_index_query(space->static_shapes, &context, bb, (phy_spatial_index_query_func)BBQuery, data);
	} phy_space_unlock(space, true);
}

//MARK: Shape Query Functions

struct ShapeQueryContext {
	phy_space_shape_query_func func;
	void *data;
	bool anyCollision;
};

// Callback from the spatial hash.
static phy_collision_id
ShapeQuery(phy_shape *a, phy_shape *b, phy_collision_id id, struct ShapeQueryContext *context)
{
	if(phy_shape_filter_reject(a->filter, b->filter) || a == b) return id;

	phy_contact_point_set set = phy_shapes_collide(a, b);
	if(set.count){
		if(context->func) context->func(b, &set, context->data);
		context->anyCollision = !(a->sensor || b->sensor);
	}
	
	return id;
}

bool
phy_space_shape_query(phy_space *space, phy_shape *shape, phy_space_shape_query_func func, void *data)
{
	phy_body *body = shape->body;
	phy_bb bb = (body ? phy_shape_update(shape, body->transform) : shape->bb);
	struct ShapeQueryContext context = {func, data, false};
	
	phy_space_lock(space); {
    phy_spatial_index_query(space->dynamic_shapes, shape, bb, (phy_spatial_index_query_func)ShapeQuery, &context);
    phy_spatial_index_query(space->static_shapes, shape, bb, (phy_spatial_index_query_func)ShapeQuery, &context);
	} phy_space_unlock(space, true);
	
	return context.anyCollision;
}
