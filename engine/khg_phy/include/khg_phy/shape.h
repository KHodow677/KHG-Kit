#pragma once

#include "khg_phy/phy_types.h"
#include "khg_phy/bb.h"

typedef struct phy_point_query_info {
	const phy_shape *shape;
	phy_vect point;
	float distance;
	phy_vect gradient;
} phy_point_query_info;

typedef struct phy_segment_query_info {
	const phy_shape *shape;
	phy_vect point;
	phy_vect normal;
	float alpha;
} phy_segment_query_info;

typedef struct phy_shape_filter {
	phy_group group;
	phy_bitmask categories;
	phy_bitmask mask;
} phy_shape_filter;

static const phy_shape_filter PHY_SHAPE_FILTER_ALL = { PHY_NO_GROUP, PHY_ALL_CATEGORIES, PHY_ALL_CATEGORIES };
static const phy_shape_filter PHY_SHAPE_FILTER_NONE = { PHY_NO_GROUP, ~PHY_ALL_CATEGORIES, ~PHY_ALL_CATEGORIES };

static inline phy_shape_filter phy_shape_filter_new(phy_group group, phy_bitmask categories, phy_bitmask mask) {
	phy_shape_filter filter = { group, categories, mask };
	return filter;
}

void phy_shape_destroy(phy_shape *shape);
void phy_shape_free(phy_shape *shape);

phy_bb phy_shape_cache_BB(phy_shape *shape);
phy_bb phy_shape_update(phy_shape *shape, phy_transform transform);

float phy_shape_point_query(const phy_shape *shape, phy_vect p, phy_point_query_info *out);
bool phy_shape_segment_query(const phy_shape *shape, phy_vect a, phy_vect b, float radius, phy_segment_query_info *info);

phy_contact_point_set phy_shapes_collide(const phy_shape *a, const phy_shape *b);
phy_space* phy_shape_get_space(const phy_shape *shape);

phy_body* phy_shape_get_body(const phy_shape *shape);
void phy_shape_set_body(phy_shape *shape, phy_body *body);

float phy_shape_get_mass(phy_shape *shape);
void phy_shape_set_mass(phy_shape *shape, float mass);

float phy_shape_get_density(phy_shape *shape);
void phy_shape_set_density(phy_shape *shape, float density);

float phy_shape_get_moment(phy_shape *shape);
float phy_shape_get_area(phy_shape *shape);
phy_vect phy_shape_get_center_of_gravity(phy_shape *shape);

phy_bb phy_shape_get_BB(const phy_shape *shape);

bool phy_shape_get_sensor(const phy_shape *shape);
void phy_shape_set_sensor(phy_shape *shape, bool sensor);

float phy_shape_get_elasticity(const phy_shape *shape);
void phy_shape_set_elasticity(phy_shape *shape, float elasticity);

float phy_shape_get_friction(const phy_shape *shape);
void phy_shape_set_friction(phy_shape *shape, float friction);

phy_vect phy_shape_get_surface_velocity(const phy_shape *shape);
void phy_shape_set_surface_velocity(phy_shape *shape, phy_vect surface_velocity);

phy_data_pointer phy_shape_get_user_data(const phy_shape *shape);
void phy_shape_set_user_data(phy_shape *shape, phy_data_pointer user_data);

phy_collision_type phy_shape_get_collision_type(const phy_shape *shape);
void phy_shape_set_collision_type(phy_shape *shape, phy_collision_type collision_type);

phy_shape_filter phy_shape_get_filter(const phy_shape *shape);
void phy_shape_set_filter(phy_shape *shape, phy_shape_filter filter);

phy_circle_shape *cpCircleShapeAlloc(void);
phy_shape *cpCircleShapeNew(phy_body *body, float radius, phy_vect offset);
phy_circle_shape *cpCircleShapeInit(phy_circle_shape *circle, phy_body *body, float radius, phy_vect offset);

phy_vect phy_circle_shape_get_offset(const phy_shape *shape);
float phy_circle_shape_get_radius(const phy_shape *shape);

phy_segment_shape *phy_segment_shape_alloc(void);
phy_shape *phy_segment_shape_new(phy_body *body, phy_vect a, phy_vect b, float radius);
phy_segment_shape *phy_segment_shape_init(phy_segment_shape *seg, phy_body *body, phy_vect a, phy_vect b, float radius);

void phy_segment_shape_set_neighbors(phy_shape *shape, phy_vect prev, phy_vect next);

phy_vect phy_segment_shape_get_A(const phy_shape *shape);
phy_vect phy_segment_shape_get_B(const phy_shape *shape);

phy_vect phy_segment_shape_get_normal(const phy_shape *shape);
float phy_segment_shape_get_radius(const phy_shape *shape);

