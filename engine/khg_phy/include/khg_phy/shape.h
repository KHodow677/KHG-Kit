#pragma once

#include "khg_phy/phy_types.h"
#include "khg_phy/bb.h"

/// Point query info struct.
typedef struct cpPointQueryInfo {
	/// The nearest shape, NULL if no shape was within range.
	const phy_shape *shape;
	/// The closest point on the shape's surface. (in world space coordinates)
	phy_vect point;
	/// The distance to the point. The distance is negative if the point is inside the shape.
	float distance;
	/// The gradient of the signed distance function.
	/// The value should be similar to info.p/info.d, but accurate even for very small values of info.d.
	phy_vect gradient;
} cpPointQueryInfo;

/// Segment query info struct.
typedef struct cpSegmentQueryInfo {
	/// The shape that was hit, or NULL if no collision occured.
	const phy_shape *shape;
	/// The point of impact.
	phy_vect point;
	/// The normal of the surface hit.
	phy_vect normal;
	/// The normalized distance along the query segment in the range [0, 1].
	float alpha;
} cpSegmentQueryInfo;

/// Fast collision filtering type that is used to determine if two objects collide before calling collision or query callbacks.
typedef struct cpShapeFilter {
	/// Two objects with the same non-zero group value do not collide.
	/// This is generally used to group objects in a composite object together to disable self collisions.
	phy_group group;
	/// A bitmask of user definable categories that this object belongs to.
	/// The category/mask combinations of both objects in a collision must agree for a collision to occur.
	phy_bitmask categories;
	/// A bitmask of user definable category types that this object object collides with.
	/// The category/mask combinations of both objects in a collision must agree for a collision to occur.
	phy_bitmask mask;
} cpShapeFilter;

/// Collision filter value for a shape that will collide with anything except CP_SHAPE_FILTER_NONE.
static const cpShapeFilter CP_SHAPE_FILTER_ALL = {PHY_NO_GROUP, PHY_ALL_CATEGORIES, PHY_ALL_CATEGORIES};
/// Collision filter value for a shape that does not collide with anything.
static const cpShapeFilter CP_SHAPE_FILTER_NONE = {PHY_NO_GROUP, ~PHY_ALL_CATEGORIES, ~PHY_ALL_CATEGORIES};

/// Create a new collision filter.
static inline cpShapeFilter
cpShapeFilterNew(phy_group group, phy_bitmask categories, phy_bitmask mask)
{
	cpShapeFilter filter = {group, categories, mask};
	return filter;
}

/// Destroy a shape.
void cpShapeDestroy(phy_shape *shape);
/// Destroy and Free a shape.
void cpShapeFree(phy_shape *shape);

/// Update, cache and return the bounding box of a shape based on the body it's attached to.
phy_bb cpShapeCacheBB(phy_shape *shape);
/// Update, cache and return the bounding box of a shape with an explicit transformation.
phy_bb cpShapeUpdate(phy_shape *shape, phy_transform transform);

/// Perform a nearest point query. It finds the closest point on the surface of shape to a specific point.
/// The value returned is the distance between the points. A negative distance means the point is inside the shape.
float cpShapePointQuery(const phy_shape *shape, phy_vect p, cpPointQueryInfo *out);

/// Perform a segment query against a shape. @c info must be a pointer to a valid cpSegmentQueryInfo structure.
bool cpShapeSegmentQuery(const phy_shape *shape, phy_vect a, phy_vect b, float radius, cpSegmentQueryInfo *info);

/// Return contact information about two shapes.
phy_contact_point_set cpShapesCollide(const phy_shape *a, const phy_shape *b);

/// The cpSpace this body is added to.
phy_space* cpShapeGetSpace(const phy_shape *shape);

/// The cpBody this shape is connected to.
phy_body* cpShapeGetBody(const phy_shape *shape);
/// Set the cpBody this shape is connected to.
/// Can only be used if the shape is not currently added to a space.
void cpShapeSetBody(phy_shape *shape, phy_body *body);

/// Get the mass of the shape if you are having Chipmunk calculate mass properties for you.
float cpShapeGetMass(phy_shape *shape);
/// Set the mass of this shape to have Chipmunk calculate mass properties for you.
void cpShapeSetMass(phy_shape *shape, float mass);

/// Get the density of the shape if you are having Chipmunk calculate mass properties for you.
float cpShapeGetDensity(phy_shape *shape);
/// Set the density  of this shape to have Chipmunk calculate mass properties for you.
void cpShapeSetDensity(phy_shape *shape, float density);

/// Get the calculated moment of inertia for this shape.
float cpShapeGetMoment(phy_shape *shape);
/// Get the calculated area of this shape.
float cpShapeGetArea(phy_shape *shape);
/// Get the centroid of this shape.
phy_vect cpShapeGetCenterOfGravity(phy_shape *shape);

/// Get the bounding box that contains the shape given it's current position and angle.
phy_bb cpShapeGetBB(const phy_shape *shape);

/// Get if the shape is set to be a sensor or not.
bool cpShapeGetSensor(const phy_shape *shape);
/// Set if the shape is a sensor or not.
void cpShapeSetSensor(phy_shape *shape, bool sensor);

/// Get the elasticity of this shape.
float cpShapeGetElasticity(const phy_shape *shape);
/// Set the elasticity of this shape.
void cpShapeSetElasticity(phy_shape *shape, float elasticity);

/// Get the friction of this shape.
float cpShapeGetFriction(const phy_shape *shape);
/// Set the friction of this shape.
void cpShapeSetFriction(phy_shape *shape, float friction);

/// Get the surface velocity of this shape.
phy_vect cpShapeGetSurfaceVelocity(const phy_shape *shape);
/// Set the surface velocity of this shape.
void cpShapeSetSurfaceVelocity(phy_shape *shape, phy_vect surfaceVelocity);

/// Get the user definable data pointer of this shape.
phy_data_pointer cpShapeGetUserData(const phy_shape *shape);
/// Set the user definable data pointer of this shape.
void cpShapeSetUserData(phy_shape *shape, phy_data_pointer userData);

/// Set the collision type of this shape.
phy_collision_type cpShapeGetCollisionType(const phy_shape *shape);
/// Get the collision type of this shape.
void cpShapeSetCollisionType(phy_shape *shape, phy_collision_type collisionType);

/// Get the collision filtering parameters of this shape.
cpShapeFilter cpShapeGetFilter(const phy_shape *shape);
/// Set the collision filtering parameters of this shape.
void cpShapeSetFilter(phy_shape *shape, cpShapeFilter filter);


/// @}
/// @defgroup cpCircleShape cpCircleShape

/// Allocate a circle shape.
phy_circle_shape* cpCircleShapeAlloc(void);
/// Initialize a circle shape.
phy_circle_shape* cpCircleShapeInit(phy_circle_shape *circle, phy_body *body, float radius, phy_vect offset);
/// Allocate and initialize a circle shape.
phy_shape* cpCircleShapeNew(phy_body *body, float radius, phy_vect offset);

/// Get the offset of a circle shape.
phy_vect cpCircleShapeGetOffset(const phy_shape *shape);
/// Get the radius of a circle shape.
float cpCircleShapeGetRadius(const phy_shape *shape);

/// @}
/// @defgroup cpSegmentShape cpSegmentShape

/// Allocate a segment shape.
phy_segment_shape* cpSegmentShapeAlloc(void);
/// Initialize a segment shape.
phy_segment_shape* cpSegmentShapeInit(phy_segment_shape *seg, phy_body *body, phy_vect a, phy_vect b, float radius);
/// Allocate and initialize a segment shape.
phy_shape* cpSegmentShapeNew(phy_body *body, phy_vect a, phy_vect b, float radius);

/// Let Chipmunk know about the geometry of adjacent segments to avoid colliding with endcaps.
void cpSegmentShapeSetNeighbors(phy_shape *shape, phy_vect prev, phy_vect next);

/// Get the first endpoint of a segment shape.
phy_vect cpSegmentShapeGetA(const phy_shape *shape);
/// Get the second endpoint of a segment shape.
phy_vect cpSegmentShapeGetB(const phy_shape *shape);
/// Get the normal of a segment shape.
phy_vect cpSegmentShapeGetNormal(const phy_shape *shape);
/// Get the first endpoint of a segment shape.
float cpSegmentShapeGetRadius(const phy_shape *shape);

/// @}
