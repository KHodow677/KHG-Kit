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
phy_shape_init(phy_shape *shape, const phy_shape_class *klass, phy_body *body, struct phy_shape_mass_info massInfo)
{
	shape->class = klass;
	
	shape->body = body;
	shape->massInfo = massInfo;
	
	shape->sensor = 0;
	
	shape->e = 0.0f;
	shape->u = 0.0f;
	shape->surfaceV = phy_v_zero;
	
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
phy_shape_destroy(phy_shape *shape)
{
	if(shape->class && shape->class->destroy) shape->class->destroy(shape);
}

void
phy_shape_free(phy_shape *shape)
{
	if(shape){
		phy_shape_destroy(shape);
		free(shape);
	}
}

phy_space *
phy_shape_get_space(const phy_shape *shape)
{
	return shape->space;
}

phy_body *
phy_shape_get_body(const phy_shape *shape)
{
	return shape->body;
}

void
phy_shape_set_body(phy_shape *shape, phy_body *body)
{
	if (phy_shape_active(shape)) {
    utl_error_func("You cannot change the body on an active shape, you must remove the shape from the space before changing the body", utl_user_defined_data);
  }
	shape->body = body;
}

float phy_shape_get_mass(phy_shape *shape){ return shape->massInfo.m; }

void
phy_shape_set_mass(phy_shape *shape, float mass){
	phy_body *body = shape->body;
	phy_body_activate(body);
	
	shape->massInfo.m = mass;
	phy_body_accumulate_mass_from_shapes(body);
}

float phy_shape_get_density(phy_shape *shape){ return shape->massInfo.m/shape->massInfo.area; }
void phy_shape_set_density(phy_shape *shape, float density){ phy_shape_set_mass(shape, density*shape->massInfo.area); }

float phy_shape_get_moment(phy_shape *shape){ return shape->massInfo.m*shape->massInfo.i; }
float phy_shape_get_area(phy_shape *shape){ return shape->massInfo.area; }
phy_vect phy_shape_get_center_of_gravity(phy_shape *shape) { return shape->massInfo.cog; }

phy_bb
phy_shape_get_BB(const phy_shape *shape)
{
	return shape->bb;
}

bool
phy_shape_get_sensor(const phy_shape *shape)
{
	return shape->sensor;
}

void
phy_shape_set_sensor(phy_shape *shape, bool sensor)
{
	phy_body_activate(shape->body);
	shape->sensor = sensor;
}

float
phy_shape_get_elasticity(const phy_shape *shape)
{
	return shape->e;
}

void
phy_shape_set_elasticity(phy_shape *shape, float elasticity)
{
	if (elasticity < 0.0f) {
    utl_error_func("Elasticity must be positive", utl_user_defined_data);
  }
	phy_body_activate(shape->body);
	shape->e = elasticity;
}

float
phy_shape_get_friction(const phy_shape *shape)
{
	return shape->u;
}

void
phy_shape_set_friction(phy_shape *shape, float friction)
{
	if (friction < 0.0f) {
    utl_error_func("Friction must be postive", utl_user_defined_data);
  }
	phy_body_activate(shape->body);
	shape->u = friction;
}

phy_vect
phy_shape_get_surface_velocity(const phy_shape *shape)
{
	return shape->surfaceV;
}

void
phy_shape_set_surface_velocity(phy_shape *shape, phy_vect surfaceVelocity)
{
	phy_body_activate(shape->body);
	shape->surfaceV = surfaceVelocity;
}

phy_data_pointer
phy_shape_get_user_data(const phy_shape *shape)
{
	return shape->userData;
}

void
phy_shape_set_user_data(phy_shape *shape, phy_data_pointer userData)
{
	shape->userData = userData;
}

phy_collision_type
phy_shape_get_collision_type(const phy_shape *shape)
{
	return shape->type;
}

void
phy_shape_set_collision_type(phy_shape *shape, phy_collision_type collisionType)
{
	phy_body_activate(shape->body);
	shape->type = collisionType;
}

phy_shape_filter
phy_shape_get_filter(const phy_shape *shape)
{
	return shape->filter;
}

void
phy_shape_set_filter(phy_shape *shape, phy_shape_filter filter)
{
	phy_body_activate(shape->body);
	shape->filter = filter;
}

phy_bb
phy_shape_cache_BB(phy_shape *shape)
{
	return phy_shape_update(shape, shape->body->transform);
}

phy_bb
phy_shape_update(phy_shape *shape, phy_transform transform)
{
	return (shape->bb = shape->class->cache_data(shape, transform));
}

float
phy_shape_point_query(const phy_shape *shape, phy_vect p, phy_point_query_info *info)
{
	phy_point_query_info blank = {NULL, phy_v_zero, INFINITY, phy_v_zero};
	if(info){
		(*info) = blank;
	} else {
		info = &blank;
	}
	
	shape->class->point_query(shape, p, info);
	return info->distance;
}


bool
phy_shape_segment_query(const phy_shape *shape, phy_vect a, phy_vect b, float radius, phy_segment_query_info *info){
	phy_segment_query_info blank = {NULL, b, phy_v_zero, 1.0f};
	if(info){
		(*info) = blank;
	} else {
		info = &blank;
	}
	
	phy_point_query_info nearest;
	shape->class->point_query(shape, a, &nearest);
	if(nearest.distance <= radius){
		info->shape = shape;
		info->alpha = 0.0;
		info->normal = phy_v_normalize(phy_v_sub(a, nearest.point));
	} else {
		shape->class->segment_query(shape, a, b, radius, info);
	}
	
	return (info->shape != NULL);
}

phy_contact_point_set
phy_shapes_collide(const phy_shape *a, const phy_shape *b)
{
	struct phy_contact contacts[PHY_MAX_CONTACTS_PER_ARBITER];
	struct phy_collision_info info = phy_collide(a, b, 0, contacts);
	
	phy_contact_point_set set;
	set.count = info.count;
	
	// cpCollideShapes() may have swapped the contact order. Flip the normal.
	bool swapped = (a != info.a);
	set.normal = (swapped ? phy_v_neg(info.n) : info.n);
	
	for(int i=0; i<info.count; i++){
		// cpCollideShapesInfo() returns contacts with absolute positions.
		phy_vect p1 = contacts[i].r1;
		phy_vect p2 = contacts[i].r2;
		
		set.points[i].pointA = (swapped ? p2 : p1);
		set.points[i].pointB = (swapped ? p1 : p2);
		set.points[i].distance = phy_v_dot(phy_v_sub(p2, p1), set.normal);
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
	phy_vect c = circle->tc = phy_transform_point(transform, circle->c);
	return phy_bb_new_for_circle(c, circle->r);
}

static void
cpCircleShapePointQuery(phy_circle_shape *circle, phy_vect p, phy_point_query_info *info)
{
	phy_vect delta = phy_v_sub(p, circle->tc);
	float d = phy_v_length(delta);
	float r = circle->r;
	
	info->shape = (phy_shape *)circle;
	float r_over_d = d > 0.0f ? r/d : r;
	info->point = phy_v_add(circle->tc, phy_v_mult(delta, r_over_d)); // TODO: div/0
	info->distance = d - r;
	
	// Use up for the gradient if the distance is very small.
	info->gradient = (d > PHY_EPSILON ? phy_v_mult(delta, 1.0f/d) : phy_v(0.0f, 1.0f));
}

static void
cpCircleShapeSegmentQuery(phy_circle_shape *circle, phy_vect a, phy_vect b, float radius, phy_segment_query_info *info)
{
	phy_circle_segment_query((phy_shape *)circle, circle->tc, circle->r, a, b, radius, info);
}

static struct phy_shape_mass_info
cpCircleShapeMassInfo(float mass, float radius, phy_vect center)
{
	struct phy_shape_mass_info info = {
		mass, phy_moment_for_circle(1.0f, 0.0f, radius, phy_v_zero),
		center,
		phy_area_for_circle(0.0f, radius),
	};
	
	return info;
}

static const phy_shape_class cpCircleShapeClass = {
	PHY_CIRCLE_SHAPE,
	(phy_shape_cache_data_impl)cpCircleShapeCacheData,
	NULL,
	(phy_shape_point_query_impl)cpCircleShapePointQuery,
	(phy_shape_segment_query_impl)cpCircleShapeSegmentQuery,
};

phy_circle_shape *
cpCircleShapeInit(phy_circle_shape *circle, phy_body *body, float radius, phy_vect offset)
{
	circle->c = offset;
	circle->r = radius;
	
	phy_shape_init((phy_shape *)circle, &cpCircleShapeClass, body, cpCircleShapeMassInfo(0.0f, radius, offset));
	
	return circle;
}

phy_shape *
cpCircleShapeNew(phy_body *body, float radius, phy_vect offset)
{
	return (phy_shape *)cpCircleShapeInit(cpCircleShapeAlloc(), body, radius, offset);
}

phy_vect
phy_circle_shape_get_offset(const phy_shape *shape)
{
	if (shape->class != &cpCircleShapeClass) {
    utl_error_func("Shape is not a circle shape", utl_user_defined_data);
  }
	return ((phy_circle_shape *)shape)->c;
}

float
phy_circle_shape_get_radius(const phy_shape *shape)
{
	if (shape->class != &cpCircleShapeClass) {
    utl_error_func("Shape is not a circle shape", utl_user_defined_data);
  }
	return ((phy_circle_shape *)shape)->r;
}


phy_segment_shape *
phy_segment_shape_alloc(void)
{
	return (phy_segment_shape *)calloc(1, sizeof(phy_segment_shape));
}

static phy_bb
cpSegmentShapeCacheData(phy_segment_shape *seg, phy_transform transform)
{
	seg->ta = phy_transform_point(transform, seg->a);
	seg->tb = phy_transform_point(transform, seg->b);
	seg->tn = phy_transform_vect(transform, seg->n);
	
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
cpSegmentShapePointQuery(phy_segment_shape *seg, phy_vect p, phy_point_query_info *info)
{
	phy_vect closest = phy_closest_point_on_segment(p, seg->ta, seg->tb);
	
	phy_vect delta = phy_v_sub(p, closest);
	float d = phy_v_length(delta);
	float r = seg->r;
	phy_vect g = phy_v_mult(delta, 1.0f/d);
	
	info->shape = (phy_shape *)seg;
	info->point = (d ? phy_v_add(closest, phy_v_mult(g, r)) : closest);
	info->distance = d - r;
	
	// Use the segment's normal if the distance is very small.
	info->gradient = (d > PHY_EPSILON ? g : seg->n);
}

static void
cpSegmentShapeSegmentQuery(phy_segment_shape *seg, phy_vect a, phy_vect b, float r2, phy_segment_query_info *info)
{
	phy_vect n = seg->tn;
	float d = phy_v_dot(phy_v_sub(seg->ta, a), n);
	float r = seg->r + r2;
	
	phy_vect flipped_n = (d > 0.0f ? phy_v_neg(n) : n);
	phy_vect seg_offset = phy_v_sub(phy_v_mult(flipped_n, r), a);
	
	// Make the endpoints relative to 'a' and move them by the thickness of the segment.
	phy_vect seg_a = phy_v_add(seg->ta, seg_offset);
	phy_vect seg_b = phy_v_add(seg->tb, seg_offset);
	phy_vect delta = phy_v_sub(b, a);
	
	if(phy_v_cross(delta, seg_a)*phy_v_cross(delta, seg_b) <= 0.0f){
		float d_offset = d + (d > 0.0f ? -r : r);
		float ad = -d_offset;
		float bd = phy_v_dot(delta, n) - d_offset;
		
		if(ad*bd < 0.0f){
			float t = ad/(ad - bd);
			
			info->shape = (phy_shape *)seg;
			info->point = phy_v_sub(phy_v_lerp(a, b, t), phy_v_mult(flipped_n, r2));
			info->normal = flipped_n;
			info->alpha = t;
		}
	} else if(r != 0.0f){
		phy_segment_query_info info1 = {NULL, b, phy_v_zero, 1.0f};
		phy_segment_query_info info2 = {NULL, b, phy_v_zero, 1.0f};
		phy_circle_segment_query((phy_shape *)seg, seg->ta, seg->r, a, b, r2, &info1);
		phy_circle_segment_query((phy_shape *)seg, seg->tb, seg->r, a, b, r2, &info2);
		
		if(info1.alpha < info2.alpha){
			(*info) = info1;
		} else {
			(*info) = info2;
		}
	}
}

static struct phy_shape_mass_info
cpSegmentShapeMassInfo(float mass, phy_vect a, phy_vect b, float r)
{
	struct phy_shape_mass_info info = {
		mass, phy_moment_for_box(1.0f, phy_v_dist(a, b) + 2.0f*r, 2.0f*r), // TODO is an approximation.
		phy_v_lerp(a, b, 0.5f),
		phy_area_for_segment(a, b, r),
	};
	
	return info;
}

static const phy_shape_class cpSegmentShapeClass = {
	PHY_SEGMENT_SHAPE,
	(phy_shape_cache_data_impl)cpSegmentShapeCacheData,
	NULL,
	(phy_shape_point_query_impl)cpSegmentShapePointQuery,
	(phy_shape_segment_query_impl)cpSegmentShapeSegmentQuery,
};

phy_segment_shape *
phy_segment_shape_init(phy_segment_shape *seg, phy_body *body, phy_vect a, phy_vect b, float r)
{
	seg->a = a;
	seg->b = b;
	seg->n = phy_v_rperp(phy_v_normalize(phy_v_sub(b, a)));
	
	seg->r = r;
	
	seg->a_tangent = phy_v_zero;
	seg->b_tangent = phy_v_zero;
	
	phy_shape_init((phy_shape *)seg, &cpSegmentShapeClass, body, cpSegmentShapeMassInfo(0.0f, a, b, r));
	
	return seg;
}

phy_shape*
phy_segment_shape_new(phy_body *body, phy_vect a, phy_vect b, float r)
{
	return (phy_shape *)phy_segment_shape_init(phy_segment_shape_alloc(), body, a, b, r);
}

phy_vect
phy_segment_shape_get_A(const phy_shape *shape)
{
	if (shape->class != &cpSegmentShapeClass) {
    utl_error_func("Shape is not a segment shape", utl_user_defined_data);
  }
	return ((phy_segment_shape *)shape)->a;
}

phy_vect
phy_segment_shape_get_B(const phy_shape *shape)
{
	if (shape->class != &cpSegmentShapeClass) {
    utl_error_func("Shape is not a segment shape", utl_user_defined_data);
  }
	return ((phy_segment_shape *)shape)->b;
}

phy_vect
phy_segment_shape_get_normal(const phy_shape *shape)
{
	if (shape->class != &cpSegmentShapeClass) {
    utl_error_func("Shape is not a segment shape", utl_user_defined_data);
  }
	return ((phy_segment_shape *)shape)->n;
}

float
phy_segment_shape_get_radius(const phy_shape *shape)
{
	if (shape->class != &cpSegmentShapeClass) {
    utl_error_func("Shape is not a segment shape", utl_user_defined_data);
  }
	return ((phy_segment_shape *)shape)->r;
}

void
phy_segment_shape_set_neighbors(phy_shape *shape, phy_vect prev, phy_vect next)
{
	if (shape->class != &cpSegmentShapeClass) {
    utl_error_func("Shape is not a segment shape", utl_user_defined_data);
  }
	phy_segment_shape *seg = (phy_segment_shape *)shape;
	
	seg->a_tangent = phy_v_sub(prev, seg->a);
	seg->b_tangent = phy_v_sub(next, seg->b);
}

// Unsafe API (chipmunk_unsafe.h)

// TODO setters should wake the shape up?

void
phy_circle_shape_set_radius(phy_shape *shape, float radius)
{
	if (shape->class != &cpCircleShapeClass) {
    utl_error_func("Shape is not a circle shape", utl_error_func);
  }
	phy_circle_shape *circle = (phy_circle_shape *)shape;
	
	circle->r = radius;
	
	float mass = shape->massInfo.m;
	shape->massInfo = cpCircleShapeMassInfo(mass, circle->r, circle->c);
	if(mass > 0.0f) phy_body_accumulate_mass_from_shapes(shape->body);
}

void
phy_circle_shape_set_offset(phy_shape *shape, phy_vect offset)
{
	if (shape->class != &cpCircleShapeClass) {
    utl_error_func("Shape is not a circle shape", utl_user_defined_data);
  }
	phy_circle_shape *circle = (phy_circle_shape *)shape;
	
	circle->c = offset;

	float mass = shape->massInfo.m;
	shape->massInfo = cpCircleShapeMassInfo(shape->massInfo.m, circle->r, circle->c);
	if(mass > 0.0f) phy_body_accumulate_mass_from_shapes(shape->body);
}

void
phy_segment_shape_set_endpoints(phy_shape *shape, phy_vect a, phy_vect b)
{
	if (shape->class != &cpSegmentShapeClass) {
    utl_error_func("Shape is not a segment shape", utl_user_defined_data);
  }
	phy_segment_shape *seg = (phy_segment_shape *)shape;
	
	seg->a = a;
	seg->b = b;
	seg->n = phy_v_perp(phy_v_normalize(phy_v_sub(b, a)));

	float mass = shape->massInfo.m;
	shape->massInfo = cpSegmentShapeMassInfo(shape->massInfo.m, seg->a, seg->b, seg->r);
	if(mass > 0.0f) phy_body_accumulate_mass_from_shapes(shape->body);
}

void
phy_segment_shape_set_radius(phy_shape *shape, float radius)
{
	if (shape->class != &cpSegmentShapeClass) {
    utl_error_func("Shape is not a segment shape", utl_user_defined_data);
  }
	phy_segment_shape *seg = (phy_segment_shape *)shape;
	
	seg->r = radius;

	float mass = shape->massInfo.m;
	shape->massInfo = cpSegmentShapeMassInfo(shape->massInfo.m, seg->a, seg->b, seg->r);
	if(mass > 0.0f) phy_body_accumulate_mass_from_shapes(shape->body);
}
