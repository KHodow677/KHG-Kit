#include "khg_phy/arbiter.h"	
#include "khg_phy/phy.h"
#include "khg_phy/phy_private.h"
#include "khg_phy/phy_unsafe.h"
#include "khg_phy/transform.h"
#include "khg_utl/error_func.h"

#define CP_DefineShapeGetter(struct, type, member, name) \
CP_DeclareShapeGetter(struct, type, name){ \
	if (shape->klass != &struct##Class) {\
    utl_error_func("shape is not a "#struct, utl_user_defined_data);\
  }\
	return ((struct *)shape)->member;\
}

phy_shape *
cpShapeInit(phy_shape *shape, const cpShapeClass *klass, phy_body *body, struct cpShapeMassInfo massInfo)
{
	shape->klass = klass;
	
	shape->body = body;
	shape->massInfo = massInfo;
	
	shape->sensor = 0;
	
	shape->e = 0.0f;
	shape->u = 0.0f;
	shape->surfaceV = cpvzero;
	
	shape->type = 0;
	shape->filter.group = PHY_NO_GROUP;
	shape->filter.categories = PHY_ALL_CATEGORIES;
	shape->filter.mask = PHY_ALL_CATEGORIES;
	
	shape->userData = NULL;
	
	shape->space = NULL;
	
	shape->next = NULL;
	shape->prev = NULL;
	
	return shape;
}

void
cpShapeDestroy(phy_shape *shape)
{
	if(shape->klass && shape->klass->destroy) shape->klass->destroy(shape);
}

void
cpShapeFree(phy_shape *shape)
{
	if(shape){
		cpShapeDestroy(shape);
		free(shape);
	}
}

phy_space *
cpShapeGetSpace(const phy_shape *shape)
{
	return shape->space;
}

phy_body *
cpShapeGetBody(const phy_shape *shape)
{
	return shape->body;
}

void
cpShapeSetBody(phy_shape *shape, phy_body *body)
{
	if (cpShapeActive(shape)) {
    utl_error_func("You cannot change the body on an active shape, you must remove the shape from the space before changing the body", utl_user_defined_data);
  }
	shape->body = body;
}

float cpShapeGetMass(phy_shape *shape){ return shape->massInfo.m; }

void
cpShapeSetMass(phy_shape *shape, float mass){
	phy_body *body = shape->body;
	phy_body_activate(body);
	
	shape->massInfo.m = mass;
	cpBodyAccumulateMassFromShapes(body);
}

float cpShapeGetDensity(phy_shape *shape){ return shape->massInfo.m/shape->massInfo.area; }
void cpShapeSetDensity(phy_shape *shape, float density){ cpShapeSetMass(shape, density*shape->massInfo.area); }

float cpShapeGetMoment(phy_shape *shape){ return shape->massInfo.m*shape->massInfo.i; }
float cpShapeGetArea(phy_shape *shape){ return shape->massInfo.area; }
phy_vect cpShapeGetCenterOfGravity(phy_shape *shape) { return shape->massInfo.cog; }

phy_bb
cpShapeGetBB(const phy_shape *shape)
{
	return shape->bb;
}

bool
cpShapeGetSensor(const phy_shape *shape)
{
	return shape->sensor;
}

void
cpShapeSetSensor(phy_shape *shape, bool sensor)
{
	phy_body_activate(shape->body);
	shape->sensor = sensor;
}

float
cpShapeGetElasticity(const phy_shape *shape)
{
	return shape->e;
}

void
cpShapeSetElasticity(phy_shape *shape, float elasticity)
{
	if (elasticity < 0.0f) {
    utl_error_func("Elasticity must be positive", utl_user_defined_data);
  }
	phy_body_activate(shape->body);
	shape->e = elasticity;
}

float
cpShapeGetFriction(const phy_shape *shape)
{
	return shape->u;
}

void
cpShapeSetFriction(phy_shape *shape, float friction)
{
	if (friction < 0.0f) {
    utl_error_func("Friction must be postive", utl_user_defined_data);
  }
	phy_body_activate(shape->body);
	shape->u = friction;
}

phy_vect
cpShapeGetSurfaceVelocity(const phy_shape *shape)
{
	return shape->surfaceV;
}

void
cpShapeSetSurfaceVelocity(phy_shape *shape, phy_vect surfaceVelocity)
{
	phy_body_activate(shape->body);
	shape->surfaceV = surfaceVelocity;
}

phy_data_pointer
cpShapeGetUserData(const phy_shape *shape)
{
	return shape->userData;
}

void
cpShapeSetUserData(phy_shape *shape, phy_data_pointer userData)
{
	shape->userData = userData;
}

phy_collision_type
cpShapeGetCollisionType(const phy_shape *shape)
{
	return shape->type;
}

void
cpShapeSetCollisionType(phy_shape *shape, phy_collision_type collisionType)
{
	phy_body_activate(shape->body);
	shape->type = collisionType;
}

cpShapeFilter
cpShapeGetFilter(const phy_shape *shape)
{
	return shape->filter;
}

void
cpShapeSetFilter(phy_shape *shape, cpShapeFilter filter)
{
	phy_body_activate(shape->body);
	shape->filter = filter;
}

phy_bb
cpShapeCacheBB(phy_shape *shape)
{
	return cpShapeUpdate(shape, shape->body->transform);
}

phy_bb
cpShapeUpdate(phy_shape *shape, phy_transform transform)
{
	return (shape->bb = shape->klass->cacheData(shape, transform));
}

float
cpShapePointQuery(const phy_shape *shape, phy_vect p, cpPointQueryInfo *info)
{
	cpPointQueryInfo blank = {NULL, cpvzero, INFINITY, cpvzero};
	if(info){
		(*info) = blank;
	} else {
		info = &blank;
	}
	
	shape->klass->pointQuery(shape, p, info);
	return info->distance;
}


bool
cpShapeSegmentQuery(const phy_shape *shape, phy_vect a, phy_vect b, float radius, cpSegmentQueryInfo *info){
	cpSegmentQueryInfo blank = {NULL, b, cpvzero, 1.0f};
	if(info){
		(*info) = blank;
	} else {
		info = &blank;
	}
	
	cpPointQueryInfo nearest;
	shape->klass->pointQuery(shape, a, &nearest);
	if(nearest.distance <= radius){
		info->shape = shape;
		info->alpha = 0.0;
		info->normal = cpvnormalize(cpvsub(a, nearest.point));
	} else {
		shape->klass->segmentQuery(shape, a, b, radius, info);
	}
	
	return (info->shape != NULL);
}

phy_contact_point_set
cpShapesCollide(const phy_shape *a, const phy_shape *b)
{
	struct cpContact contacts[PHY_MAX_CONTACTS_PER_ARBITER];
	struct cpCollisionInfo info = cpCollide(a, b, 0, contacts);
	
	phy_contact_point_set set;
	set.count = info.count;
	
	// cpCollideShapes() may have swapped the contact order. Flip the normal.
	bool swapped = (a != info.a);
	set.normal = (swapped ? cpvneg(info.n) : info.n);
	
	for(int i=0; i<info.count; i++){
		// cpCollideShapesInfo() returns contacts with absolute positions.
		phy_vect p1 = contacts[i].r1;
		phy_vect p2 = contacts[i].r2;
		
		set.points[i].pointA = (swapped ? p2 : p1);
		set.points[i].pointB = (swapped ? p1 : p2);
		set.points[i].distance = cpvdot(cpvsub(p2, p1), set.normal);
	}
	
	return set;
}

phy_circle_shape *
cpCircleShapeAlloc(void)
{
	return (phy_circle_shape *)calloc(1, sizeof(phy_circle_shape));
}

static phy_bb
cpCircleShapeCacheData(phy_circle_shape *circle, phy_transform transform)
{
	phy_vect c = circle->tc = cpTransformPoint(transform, circle->c);
	return phy_bb_new_for_circle(c, circle->r);
}

static void
cpCircleShapePointQuery(phy_circle_shape *circle, phy_vect p, cpPointQueryInfo *info)
{
	phy_vect delta = cpvsub(p, circle->tc);
	float d = cpvlength(delta);
	float r = circle->r;
	
	info->shape = (phy_shape *)circle;
	float r_over_d = d > 0.0f ? r/d : r;
	info->point = cpvadd(circle->tc, cpvmult(delta, r_over_d)); // TODO: div/0
	info->distance = d - r;
	
	// Use up for the gradient if the distance is very small.
	info->gradient = (d > MAGIC_EPSILON ? cpvmult(delta, 1.0f/d) : cpv(0.0f, 1.0f));
}

static void
cpCircleShapeSegmentQuery(phy_circle_shape *circle, phy_vect a, phy_vect b, float radius, cpSegmentQueryInfo *info)
{
	CircleSegmentQuery((phy_shape *)circle, circle->tc, circle->r, a, b, radius, info);
}

static struct cpShapeMassInfo
cpCircleShapeMassInfo(float mass, float radius, phy_vect center)
{
	struct cpShapeMassInfo info = {
		mass, phy_moment_for_circle(1.0f, 0.0f, radius, cpvzero),
		center,
		phy_area_for_circle(0.0f, radius),
	};
	
	return info;
}

static const cpShapeClass cpCircleShapeClass = {
	CP_CIRCLE_SHAPE,
	(cpShapeCacheDataImpl)cpCircleShapeCacheData,
	NULL,
	(cpShapePointQueryImpl)cpCircleShapePointQuery,
	(cpShapeSegmentQueryImpl)cpCircleShapeSegmentQuery,
};

phy_circle_shape *
cpCircleShapeInit(phy_circle_shape *circle, phy_body *body, float radius, phy_vect offset)
{
	circle->c = offset;
	circle->r = radius;
	
	cpShapeInit((phy_shape *)circle, &cpCircleShapeClass, body, cpCircleShapeMassInfo(0.0f, radius, offset));
	
	return circle;
}

phy_shape *
cpCircleShapeNew(phy_body *body, float radius, phy_vect offset)
{
	return (phy_shape *)cpCircleShapeInit(cpCircleShapeAlloc(), body, radius, offset);
}

phy_vect
cpCircleShapeGetOffset(const phy_shape *shape)
{
	if (shape->klass != &cpCircleShapeClass) {
    utl_error_func("Shape is not a circle shape", utl_user_defined_data);
  }
	return ((phy_circle_shape *)shape)->c;
}

float
cpCircleShapeGetRadius(const phy_shape *shape)
{
	if (shape->klass != &cpCircleShapeClass) {
    utl_error_func("Shape is not a circle shape", utl_user_defined_data);
  }
	return ((phy_circle_shape *)shape)->r;
}


phy_segment_shape *
cpSegmentShapeAlloc(void)
{
	return (phy_segment_shape *)calloc(1, sizeof(phy_segment_shape));
}

static phy_bb
cpSegmentShapeCacheData(phy_segment_shape *seg, phy_transform transform)
{
	seg->ta = cpTransformPoint(transform, seg->a);
	seg->tb = cpTransformPoint(transform, seg->b);
	seg->tn = cpTransformVect(transform, seg->n);
	
	float l,r,b,t;
	
	if(seg->ta.x < seg->tb.x){
		l = seg->ta.x;
		r = seg->tb.x;
	} else {
		l = seg->tb.x;
		r = seg->ta.x;
	}
	
	if(seg->ta.y < seg->tb.y){
		b = seg->ta.y;
		t = seg->tb.y;
	} else {
		b = seg->tb.y;
		t = seg->ta.y;
	}
	
	float rad = seg->r;
	return phy_bb_new(l - rad, b - rad, r + rad, t + rad);
}

static void
cpSegmentShapePointQuery(phy_segment_shape *seg, phy_vect p, cpPointQueryInfo *info)
{
	phy_vect closest = phy_closest_point_on_segment(p, seg->ta, seg->tb);
	
	phy_vect delta = cpvsub(p, closest);
	float d = cpvlength(delta);
	float r = seg->r;
	phy_vect g = cpvmult(delta, 1.0f/d);
	
	info->shape = (phy_shape *)seg;
	info->point = (d ? cpvadd(closest, cpvmult(g, r)) : closest);
	info->distance = d - r;
	
	// Use the segment's normal if the distance is very small.
	info->gradient = (d > MAGIC_EPSILON ? g : seg->n);
}

static void
cpSegmentShapeSegmentQuery(phy_segment_shape *seg, phy_vect a, phy_vect b, float r2, cpSegmentQueryInfo *info)
{
	phy_vect n = seg->tn;
	float d = cpvdot(cpvsub(seg->ta, a), n);
	float r = seg->r + r2;
	
	phy_vect flipped_n = (d > 0.0f ? cpvneg(n) : n);
	phy_vect seg_offset = cpvsub(cpvmult(flipped_n, r), a);
	
	// Make the endpoints relative to 'a' and move them by the thickness of the segment.
	phy_vect seg_a = cpvadd(seg->ta, seg_offset);
	phy_vect seg_b = cpvadd(seg->tb, seg_offset);
	phy_vect delta = cpvsub(b, a);
	
	if(cpvcross(delta, seg_a)*cpvcross(delta, seg_b) <= 0.0f){
		float d_offset = d + (d > 0.0f ? -r : r);
		float ad = -d_offset;
		float bd = cpvdot(delta, n) - d_offset;
		
		if(ad*bd < 0.0f){
			float t = ad/(ad - bd);
			
			info->shape = (phy_shape *)seg;
			info->point = cpvsub(cpvlerp(a, b, t), cpvmult(flipped_n, r2));
			info->normal = flipped_n;
			info->alpha = t;
		}
	} else if(r != 0.0f){
		cpSegmentQueryInfo info1 = {NULL, b, cpvzero, 1.0f};
		cpSegmentQueryInfo info2 = {NULL, b, cpvzero, 1.0f};
		CircleSegmentQuery((phy_shape *)seg, seg->ta, seg->r, a, b, r2, &info1);
		CircleSegmentQuery((phy_shape *)seg, seg->tb, seg->r, a, b, r2, &info2);
		
		if(info1.alpha < info2.alpha){
			(*info) = info1;
		} else {
			(*info) = info2;
		}
	}
}

static struct cpShapeMassInfo
cpSegmentShapeMassInfo(float mass, phy_vect a, phy_vect b, float r)
{
	struct cpShapeMassInfo info = {
		mass, phy_moment_for_box(1.0f, cpvdist(a, b) + 2.0f*r, 2.0f*r), // TODO is an approximation.
		cpvlerp(a, b, 0.5f),
		phy_area_for_segment(a, b, r),
	};
	
	return info;
}

static const cpShapeClass cpSegmentShapeClass = {
	CP_SEGMENT_SHAPE,
	(cpShapeCacheDataImpl)cpSegmentShapeCacheData,
	NULL,
	(cpShapePointQueryImpl)cpSegmentShapePointQuery,
	(cpShapeSegmentQueryImpl)cpSegmentShapeSegmentQuery,
};

phy_segment_shape *
cpSegmentShapeInit(phy_segment_shape *seg, phy_body *body, phy_vect a, phy_vect b, float r)
{
	seg->a = a;
	seg->b = b;
	seg->n = cpvrperp(cpvnormalize(cpvsub(b, a)));
	
	seg->r = r;
	
	seg->a_tangent = cpvzero;
	seg->b_tangent = cpvzero;
	
	cpShapeInit((phy_shape *)seg, &cpSegmentShapeClass, body, cpSegmentShapeMassInfo(0.0f, a, b, r));
	
	return seg;
}

phy_shape*
cpSegmentShapeNew(phy_body *body, phy_vect a, phy_vect b, float r)
{
	return (phy_shape *)cpSegmentShapeInit(cpSegmentShapeAlloc(), body, a, b, r);
}

phy_vect
cpSegmentShapeGetA(const phy_shape *shape)
{
	if (shape->klass != &cpSegmentShapeClass) {
    utl_error_func("Shape is not a segment shape", utl_user_defined_data);
  }
	return ((phy_segment_shape *)shape)->a;
}

phy_vect
cpSegmentShapeGetB(const phy_shape *shape)
{
	if (shape->klass != &cpSegmentShapeClass) {
    utl_error_func("Shape is not a segment shape", utl_user_defined_data);
  }
	return ((phy_segment_shape *)shape)->b;
}

phy_vect
cpSegmentShapeGetNormal(const phy_shape *shape)
{
	if (shape->klass != &cpSegmentShapeClass) {
    utl_error_func("Shape is not a segment shape", utl_user_defined_data);
  }
	return ((phy_segment_shape *)shape)->n;
}

float
cpSegmentShapeGetRadius(const phy_shape *shape)
{
	if (shape->klass != &cpSegmentShapeClass) {
    utl_error_func("Shape is not a segment shape", utl_user_defined_data);
  }
	return ((phy_segment_shape *)shape)->r;
}

void
cpSegmentShapeSetNeighbors(phy_shape *shape, phy_vect prev, phy_vect next)
{
	if (shape->klass != &cpSegmentShapeClass) {
    utl_error_func("Shape is not a segment shape", utl_user_defined_data);
  }
	phy_segment_shape *seg = (phy_segment_shape *)shape;
	
	seg->a_tangent = cpvsub(prev, seg->a);
	seg->b_tangent = cpvsub(next, seg->b);
}

// Unsafe API (chipmunk_unsafe.h)

// TODO setters should wake the shape up?

void
cpCircleShapeSetRadius(phy_shape *shape, float radius)
{
	if (shape->klass != &cpCircleShapeClass) {
    utl_error_func("Shape is not a circle shape", utl_error_func);
  }
	phy_circle_shape *circle = (phy_circle_shape *)shape;
	
	circle->r = radius;
	
	float mass = shape->massInfo.m;
	shape->massInfo = cpCircleShapeMassInfo(mass, circle->r, circle->c);
	if(mass > 0.0f) cpBodyAccumulateMassFromShapes(shape->body);
}

void
cpCircleShapeSetOffset(phy_shape *shape, phy_vect offset)
{
	if (shape->klass != &cpCircleShapeClass) {
    utl_error_func("Shape is not a circle shape", utl_user_defined_data);
  }
	phy_circle_shape *circle = (phy_circle_shape *)shape;
	
	circle->c = offset;

	float mass = shape->massInfo.m;
	shape->massInfo = cpCircleShapeMassInfo(shape->massInfo.m, circle->r, circle->c);
	if(mass > 0.0f) cpBodyAccumulateMassFromShapes(shape->body);
}

void
cpSegmentShapeSetEndpoints(phy_shape *shape, phy_vect a, phy_vect b)
{
	if (shape->klass != &cpSegmentShapeClass) {
    utl_error_func("Shape is not a segment shape", utl_user_defined_data);
  }
	phy_segment_shape *seg = (phy_segment_shape *)shape;
	
	seg->a = a;
	seg->b = b;
	seg->n = cpvperp(cpvnormalize(cpvsub(b, a)));

	float mass = shape->massInfo.m;
	shape->massInfo = cpSegmentShapeMassInfo(shape->massInfo.m, seg->a, seg->b, seg->r);
	if(mass > 0.0f) cpBodyAccumulateMassFromShapes(shape->body);
}

void
cpSegmentShapeSetRadius(phy_shape *shape, float radius)
{
	if (shape->klass != &cpSegmentShapeClass) {
    utl_error_func("Shape is not a segment shape", utl_user_defined_data);
  }
	phy_segment_shape *seg = (phy_segment_shape *)shape;
	
	seg->r = radius;

	float mass = shape->massInfo.m;
	shape->massInfo = cpSegmentShapeMassInfo(shape->massInfo.m, seg->a, seg->b, seg->r);
	if(mass > 0.0f) cpBodyAccumulateMassFromShapes(shape->body);
}
