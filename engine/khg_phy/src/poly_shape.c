#include "khg_phy/transform.h"
#include "khg_phy/phy.h"
#include "khg_phy/phy_private.h"
#include "khg_phy/poly_shape.h"
#include "khg_phy/phy_unsafe.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

phy_poly_shape *
phy_poly_shape_alloc(void)
{
	return (phy_poly_shape *)calloc(1, sizeof(phy_poly_shape));
}

static void
cpPolyShapeDestroy(phy_poly_shape *poly)
{
	if(poly->count > PHY_POLY_SHAPE_INLINE_ALLOC){
		free(poly->planes);
	}
}

static phy_bb
cpPolyShapeCacheData(phy_poly_shape *poly, phy_transform transform)
{
	int count = poly->count;
	struct phy_splitting_plane *dst = poly->planes;
	struct phy_splitting_plane *src = dst + count;
	
	float l = (float)INFINITY, r = -(float)INFINITY;
	float b = (float)INFINITY, t = -(float)INFINITY;
	
	for(int i=0; i<count; i++){
		phy_vect v = phy_transform_point(transform, src[i].v0);
		phy_vect n = phy_transform_vect(transform, src[i].n);
		
		dst[i].v0 = v;
		dst[i].n = n;
		
		l = phy_min(l, v.x);
		r = phy_max(r, v.x);
		b = phy_min(b, v.y);
		t = phy_max(t, v.y);
	}
	
	float radius = poly->r;
	return (poly->shape.bb = phy_bb_new(l - radius, b - radius, r + radius, t + radius));
}

static void
cpPolyShapePointQuery(phy_poly_shape *poly, phy_vect p, phy_point_query_info *info){
	int count = poly->count;
	struct phy_splitting_plane *planes = poly->planes;
	float r = poly->r;
	
	phy_vect v0 = planes[count - 1].v0;
	float minDist = INFINITY;
	phy_vect closestPoint = phy_v_zero;
	phy_vect closestNormal = phy_v_zero;
	bool outside = false;
	
	for(int i=0; i<count; i++){
		phy_vect v1 = planes[i].v0;
		outside = outside || (phy_v_dot(planes[i].n, phy_v_sub(p,v1)) > 0.0f);
		
		phy_vect closest = phy_closest_point_on_segment(p, v0, v1);
		
		float dist = phy_v_dist(p, closest);
		if(dist < minDist){
			minDist = dist;
			closestPoint = closest;
			closestNormal = planes[i].n;
		}
		
		v0 = v1;
	}
	
	float dist = (outside ? minDist : -minDist);
	phy_vect g = phy_v_mult(phy_v_sub(p, closestPoint), 1.0f/dist);
	
	info->shape = (phy_shape *)poly;
	info->point = phy_v_add(closestPoint, phy_v_mult(g, r));
	info->distance = dist - r;
	
	// Use the normal of the closest segment if the distance is small.
	info->gradient = (minDist > PHY_EPSILON ? g : closestNormal);
}

static void
cpPolyShapeSegmentQuery(phy_poly_shape *poly, phy_vect a, phy_vect b, float r2, phy_segment_query_info *info)
{
	struct phy_splitting_plane *planes = poly->planes;
	int count = poly->count;
	float r = poly->r;
	float rsum = r + r2;
	
	for(int i=0; i<count; i++){
		phy_vect n = planes[i].n;
		float an = phy_v_dot(a, n);
		float d =  an - phy_v_dot(planes[i].v0, n) - rsum;
		if(d < 0.0f) continue;
		
		float bn = phy_v_dot(b, n);
		// Avoid divide by zero. (d is always positive)
		float t = d/phy_max(an - bn, FLT_MIN);
		if(t < 0.0f || 1.0f < t) continue;
		
		phy_vect point = phy_v_lerp(a, b, t);
		float dt = phy_v_cross(n, point);
		float dtMin = phy_v_cross(n, planes[(i - 1 + count)%count].v0);
		float dtMax = phy_v_cross(n, planes[i].v0);
		
		if(dtMin <= dt && dt <= dtMax){
			info->shape = (phy_shape *)poly;
			info->point = phy_v_sub(phy_v_lerp(a, b, t), phy_v_mult(n, r2));
			info->normal = n;
			info->alpha = t;
		}
	}
	
	// Also check against the beveled vertexes.
	if(rsum > 0.0f){
		for(int i=0; i<count; i++){
			phy_segment_query_info circle_info = {NULL, b, phy_v_zero, 1.0f};
			phy_circle_segment_query(&poly->shape, planes[i].v0, r, a, b, r2, &circle_info);
			if(circle_info.alpha < info->alpha) (*info) = circle_info;
		}
	}
}

static void
SetVerts(phy_poly_shape *poly, int count, const phy_vect *verts)
{
	poly->count = count;
	if(count <= PHY_POLY_SHAPE_INLINE_ALLOC){
		poly->planes = poly->fixed_planes;
	} else {
		poly->planes = (struct phy_splitting_plane *)calloc(2*count, sizeof(struct phy_splitting_plane));
	}
	
	for(int i=0; i<count; i++){
		phy_vect a = verts[(i - 1 + count)%count];
		phy_vect b = verts[i];
		phy_vect n = phy_v_normalize(phy_v_rperp(phy_v_sub(b, a)));
		
		poly->planes[i + count].v0 = b;
		poly->planes[i + count].n = n;
	}
}

static struct phy_shape_mass_info
cpPolyShapeMassInfo(float mass, int count, const phy_vect *verts, float radius)
{
	// TODO moment is approximate due to radius.
	
	phy_vect centroid = phy_centroid_for_poly(count, verts);
	struct phy_shape_mass_info info = {
		mass, phy_moment_for_poly(1.0f, count, verts, phy_v_neg(centroid), radius),
		centroid,
		phy_area_for_poly(count, verts, radius),
	};
	
	return info;
}

static const phy_shape_class polyClass = {
	PHY_POLY_SHAPE,
	(phy_shape_cache_data_impl)cpPolyShapeCacheData,
	(phy_shape_destroy_impl)cpPolyShapeDestroy,
	(phy_shape_point_query_impl)cpPolyShapePointQuery,
	(phy_shape_segment_query_impl)cpPolyShapeSegmentQuery,
};

phy_poly_shape *
phy_poly_shape_init(phy_poly_shape *poly, phy_body *body, int count, const phy_vect *verts, phy_transform transform, float radius)
{
	phy_vect *hullVerts = (phy_vect *)alloca(count*sizeof(phy_vect));
	
	// Transform the verts before building the hull in case of a negative scale.
	for(int i=0; i<count; i++) hullVerts[i] = phy_transform_point(transform, verts[i]);
	
	unsigned int hullCount = phy_convex_hull(count, hullVerts, hullVerts, NULL, 0.0);
	return phy_poly_shape_init_raw(poly, body, hullCount, hullVerts, radius);
}

phy_poly_shape *
phy_poly_shape_init_raw(phy_poly_shape *poly, phy_body *body, int count, const phy_vect *verts, float radius)
{
	phy_shape_init((phy_shape *)poly, &polyClass, body, cpPolyShapeMassInfo(0.0f, count, verts, radius));
	
	SetVerts(poly, count, verts);
	poly->r = radius;

	return poly;
}

phy_shape *
phy_poly_shape_new(phy_body *body, int count, const phy_vect *verts, phy_transform transform, float radius)
{
	return (phy_shape *)phy_poly_shape_init(phy_poly_shape_alloc(), body, count, verts, transform, radius);
}

phy_shape *
phy_poly_shape_new_raw(phy_body *body, int count, const phy_vect *verts, float radius)
{
	return (phy_shape *)phy_poly_shape_init_raw(phy_poly_shape_alloc(), body, count, verts, radius);
}

phy_poly_shape *
phy_box_shape_init(phy_poly_shape *poly, phy_body *body, float width, float height, float radius)
{
	float hw = width/2.0f;
	float hh = height/2.0f;
	
	return phy_box_shape_init_2(poly, body, phy_bb_new(-hw, -hh, hw, hh), radius);
}

phy_poly_shape *
phy_box_shape_init_2(phy_poly_shape *poly, phy_body *body, phy_bb box, float radius)
{
	phy_vect verts[4] = {
		phy_v(box.r, box.b),
		phy_v(box.r, box.t),
		phy_v(box.l, box.t),
		phy_v(box.l, box.b),
	};
	
	return phy_poly_shape_init_raw(poly, body, 4, verts, radius);
}

phy_shape *
phy_box_shape_new(phy_body *body, float width, float height, float radius)
{
	return (phy_shape *)phy_box_shape_init(phy_poly_shape_alloc(), body, width, height, radius);
}

phy_shape *
phy_box_shape_new_2(phy_body *body, phy_bb box, float radius)
{
	return (phy_shape *)phy_box_shape_init_2(phy_poly_shape_alloc(), body, box, radius);
}

int
phy_poly_shape_get_count(const phy_shape *shape)
{
	if (shape->class != &polyClass) {
    utl_error_func("Shape is not a poly shape", utl_user_defined_data);
  }
	return ((phy_poly_shape *)shape)->count;
}

phy_vect
phy_poly_shape_get_vert(const phy_shape *shape, int i)
{
	if (shape->class != &polyClass) {
    utl_error_func("Shape is not a poly shape", utl_user_defined_data);
  }
	
	int count = phy_poly_shape_get_count(shape);
	if (!(0 <= i && i < count)) {
    utl_error_func("Index out of range", utl_user_defined_data);
  }
	
	return ((phy_poly_shape *)shape)->planes[i + count].v0;
}

float
phy_poly_shape_get_radius(const phy_shape *shape)
{
	if (shape->class != &polyClass) {
    utl_error_func("Shape is not a poly shape", utl_user_defined_data);
  }
	return ((phy_poly_shape *)shape)->r;
}

// Unsafe API (chipmunk_unsafe.h)

void
phy_poly_shape_set_verts(phy_shape *shape, int count, phy_vect *verts, phy_transform transform)
{
	phy_vect *hullVerts = (phy_vect *)alloca(count*sizeof(phy_vect));
	
	// Transform the verts before building the hull in case of a negative scale.
	for(int i=0; i<count; i++) hullVerts[i] = phy_transform_point(transform, verts[i]);
	
	unsigned int hullCount = phy_convex_hull(count, hullVerts, hullVerts, NULL, 0.0);
	phy_poly_shape_set_verts_raw(shape, hullCount, hullVerts);
}

void
phy_poly_shape_set_verts_raw(phy_shape *shape, int count, phy_vect *verts)
{
	if (shape->class != &polyClass) {
    utl_error_func("Shape is not a poly shape", utl_user_defined_data);
  }
	phy_poly_shape *poly = (phy_poly_shape *)shape;
	cpPolyShapeDestroy(poly);
	
	SetVerts(poly, count, verts);
	
	float mass = shape->massInfo.m;
	shape->massInfo = cpPolyShapeMassInfo(shape->massInfo.m, count, verts, poly->r);
	if(mass > 0.0f) phy_body_accumulate_mass_from_shapes(shape->body);
}

void
phy_poly_shape_set_radius(phy_shape *shape, float radius)
{
	if (shape->class != &polyClass) {
    utl_error_func("Shape is not a poly shape", utl_user_defined_data);
  }
	phy_poly_shape *poly = (phy_poly_shape *)shape;
	poly->r = radius;
	
	
	// TODO radius is not handled by moment/area
//	float mass = shape->massInfo.m;
//	shape->massInfo = cpPolyShapeMassInfo(shape->massInfo.m, poly->count, poly->verts, poly->r);
//	if(mass > 0.0f) cpBodyAccumulateMassFromShapes(shape->body);
}
