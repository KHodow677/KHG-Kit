#pragma once

#include "khg_phy/phy_types.h"
#include "khg_phy/bb.h"
#include "khg_phy/shape.h"

/// Collision begin event function callback type.
/// Returning false from a begin callback causes the collision to be ignored until
/// the the separate callback is called when the objects stop colliding.
typedef bool (*cpCollisionBeginFunc)(phy_arbiter *arb, phy_space *space, phy_data_pointer userData);
/// Collision pre-solve event function callback type.
/// Returning false from a pre-step callback causes the collision to be ignored until the next step.
typedef bool (*cpCollisionPreSolveFunc)(phy_arbiter *arb, phy_space *space, phy_data_pointer userData);
/// Collision post-solve event function callback type.
typedef void (*cpCollisionPostSolveFunc)(phy_arbiter *arb, phy_space *space, phy_data_pointer userData);
/// Collision separate event function callback type.
typedef void (*cpCollisionSeparateFunc)(phy_arbiter *arb, phy_space *space, phy_data_pointer userData);

/// Struct that holds function callback pointers to configure custom collision handling.
/// Collision handlers have a pair of types; when a collision occurs between two shapes that have these types, the collision handler functions are triggered.
struct phy_collision_handler {
	/// Collision type identifier of the first shape that this handler recognizes.
	/// In the collision handler callback, the shape with this type will be the first argument. Read only.
	const phy_collision_type typeA;
	/// Collision type identifier of the second shape that this handler recognizes.
	/// In the collision handler callback, the shape with this type will be the second argument. Read only.
	const phy_collision_type typeB;
	/// This function is called when two shapes with types that match this collision handler begin colliding.
	cpCollisionBeginFunc beginFunc;
	/// This function is called each step when two shapes with types that match this collision handler are colliding.
	/// It's called before the collision solver runs so that you can affect a collision's outcome.
	cpCollisionPreSolveFunc preSolveFunc;
	/// This function is called each step when two shapes with types that match this collision handler are colliding.
	/// It's called after the collision solver runs so that you can read back information about the collision to trigger events in your game.
	cpCollisionPostSolveFunc postSolveFunc;
	/// This function is called when two shapes with types that match this collision handler stop colliding.
	cpCollisionSeparateFunc separateFunc;
	/// This is a user definable context pointer that is passed to all of the collision handler functions.
	phy_data_pointer userData;
};

// TODO: Make timestep a parameter?


//MARK: Memory and Initialization

/// Allocate a cpSpace.
phy_space* cpSpaceAlloc(void);
/// Initialize a cpSpace.
phy_space* cpSpaceInit(phy_space *space);
/// Allocate and initialize a cpSpace.
phy_space* cpSpaceNew(void);

/// Destroy a cpSpace.
void cpSpaceDestroy(phy_space *space);
/// Destroy and free a cpSpace.
void cpSpaceFree(phy_space *space);


//MARK: Properties

/// Number of iterations to use in the impulse solver to solve contacts and other constraints.
int cpSpaceGetIterations(const phy_space *space);
void cpSpaceSetIterations(phy_space *space, int iterations);

/// Gravity to pass to rigid bodies when integrating velocity.
phy_vect cpSpaceGetGravity(const phy_space *space);
void cpSpaceSetGravity(phy_space *space, phy_vect gravity);

/// Damping rate expressed as the fraction of velocity bodies retain each second.
/// A value of 0.9 would mean that each body's velocity will drop 10% per second.
/// The default value is 1.0, meaning no damping is applied.
/// @note This damping value is different than those of cpDampedSpring and cpDampedRotarySpring.
float cpSpaceGetDamping(const phy_space *space);
void cpSpaceSetDamping(phy_space *space, float damping);

/// Speed threshold for a body to be considered idle.
/// The default value of 0 means to let the space guess a good threshold based on gravity.
float cpSpaceGetIdleSpeedThreshold(const phy_space *space);
void cpSpaceSetIdleSpeedThreshold(phy_space *space, float idleSpeedThreshold);

/// Time a group of bodies must remain idle in order to fall asleep.
/// Enabling sleeping also implicitly enables the the contact graph.
/// The default value of INFINITY disables the sleeping algorithm.
float cpSpaceGetSleepTimeThreshold(const phy_space *space);
void cpSpaceSetSleepTimeThreshold(phy_space *space, float sleepTimeThreshold);

/// Amount of encouraged penetration between colliding shapes.
/// Used to reduce oscillating contacts and keep the collision cache warm.
/// Defaults to 0.1. If you have poor simulation quality,
/// increase this number as much as possible without allowing visible amounts of overlap.
float cpSpaceGetCollisionSlop(const phy_space *space);
void cpSpaceSetCollisionSlop(phy_space *space, float collisionSlop);

/// Determines how fast overlapping shapes are pushed apart.
/// Expressed as a fraction of the error remaining after each second.
/// Defaults to pow(1.0 - 0.1, 60.0) meaning that Chipmunk fixes 10% of overlap each frame at 60Hz.
float cpSpaceGetCollisionBias(const phy_space *space);
void cpSpaceSetCollisionBias(phy_space *space, float collisionBias);

/// Number of frames that contact information should persist.
/// Defaults to 3. There is probably never a reason to change this value.
phy_timestamp cpSpaceGetCollisionPersistence(const phy_space *space);
void cpSpaceSetCollisionPersistence(phy_space *space, phy_timestamp collisionPersistence);

/// User definable data pointer.
/// Generally this points to your game's controller or game state
/// class so you can access it when given a cpSpace reference in a callback.
phy_data_pointer cpSpaceGetUserData(const phy_space *space);
void cpSpaceSetUserData(phy_space *space, phy_data_pointer userData);

/// The Space provided static body for a given cpSpace.
/// This is merely provided for convenience and you are not required to use it.
phy_body* cpSpaceGetStaticBody(const phy_space *space);

/// Returns the current (or most recent) time step used with the given space.
/// Useful from callbacks if your time step is not a compile-time global.
float cpSpaceGetCurrentTimeStep(const phy_space *space);

/// returns true from inside a callback when objects cannot be added/removed.
bool cpSpaceIsLocked(phy_space *space);


//MARK: Collision Handlers

/// Create or return the existing collision handler that is called for all collisions that are not handled by a more specific collision handler.
phy_collision_handler *cpSpaceAddDefaultCollisionHandler(phy_space *space);
/// Create or return the existing collision handler for the specified pair of collision types.
/// If wildcard handlers are used with either of the collision types, it's the responibility of the custom handler to invoke the wildcard handlers.
phy_collision_handler *cpSpaceAddCollisionHandler(phy_space *space, phy_collision_type a, phy_collision_type b);
/// Create or return the existing wildcard collision handler for the specified type.
phy_collision_handler *cpSpaceAddWildcardHandler(phy_space *space, phy_collision_type type);


//MARK: Add/Remove objects

/// Add a collision shape to the simulation.
/// If the shape is attached to a static body, it will be added as a static shape.
phy_shape* cpSpaceAddShape(phy_space *space, phy_shape *shape);
/// Add a rigid body to the simulation.
phy_body* cpSpaceAddBody(phy_space *space, phy_body *body);
/// Add a constraint to the simulation.
phy_constraint* cpSpaceAddConstraint(phy_space *space, phy_constraint *constraint);

/// Remove a collision shape from the simulation.
void cpSpaceRemoveShape(phy_space *space, phy_shape *shape);
/// Remove a rigid body from the simulation.
void cpSpaceRemoveBody(phy_space *space, phy_body *body);
/// Remove a constraint from the simulation.
void cpSpaceRemoveConstraint(phy_space *space, phy_constraint *constraint);

/// Test if a collision shape has been added to the space.
bool cpSpaceContainsShape(phy_space *space, phy_shape *shape);
/// Test if a rigid body has been added to the space.
bool cpSpaceContainsBody(phy_space *space, phy_body *body);
/// Test if a constraint has been added to the space.
bool cpSpaceContainsConstraint(phy_space *space, phy_constraint *constraint);

//MARK: Post-Step Callbacks

/// Post Step callback function type.
typedef void (*cpPostStepFunc)(phy_space *space, void *key, void *data);
/// Schedule a post-step callback to be called when cpSpaceStep() finishes.
/// You can only register one callback per unique value for @c key.
/// Returns true only if @c key has never been scheduled before.
/// It's possible to pass @c NULL for @c func if you only want to mark @c key as being used.
bool cpSpaceAddPostStepCallback(phy_space *space, cpPostStepFunc func, void *key, void *data);


//MARK: Queries

// TODO: Queries and iterators should take a cpSpace parametery.
// TODO: They should also be abortable.

/// Nearest point query callback function type.
typedef void (*cpSpacePointQueryFunc)(phy_shape *shape, phy_vect point, float distance, phy_vect gradient, void *data);
/// Query the space at a point and call @c func for each shape found.
void cpSpacePointQuery(phy_space *space, phy_vect point, float maxDistance, cpShapeFilter filter, cpSpacePointQueryFunc func, void *data);
/// Query the space at a point and return the nearest shape found. Returns NULL if no shapes were found.
phy_shape *cpSpacePointQueryNearest(phy_space *space, phy_vect point, float maxDistance, cpShapeFilter filter, cpPointQueryInfo *out);

/// Segment query callback function type.
typedef void (*cpSpaceSegmentQueryFunc)(phy_shape *shape, phy_vect point, phy_vect normal, float alpha, void *data);
/// Perform a directed line segment query (like a raycast) against the space calling @c func for each shape intersected.
void cpSpaceSegmentQuery(phy_space *space, phy_vect start, phy_vect end, float radius, cpShapeFilter filter, cpSpaceSegmentQueryFunc func, void *data);
/// Perform a directed line segment query (like a raycast) against the space and return the first shape hit. Returns NULL if no shapes were hit.
phy_shape *cpSpaceSegmentQueryFirst(phy_space *space, phy_vect start, phy_vect end, float radius, cpShapeFilter filter, cpSegmentQueryInfo *out);

/// Rectangle Query callback function type.
typedef void (*cpSpaceBBQueryFunc)(phy_shape *shape, void *data);
/// Perform a fast rectangle query on the space calling @c func for each shape found.
/// Only the shape's bounding boxes are checked for overlap, not their full shape.
void cpSpaceBBQuery(phy_space *space, phy_bb bb, cpShapeFilter filter, cpSpaceBBQueryFunc func, void *data);

/// Shape query callback function type.
typedef void (*cpSpaceShapeQueryFunc)(phy_shape *shape, phy_contact_point_set *points, void *data);
/// Query a space for any shapes overlapping the given shape and call @c func for each shape found.
bool cpSpaceShapeQuery(phy_space *space, phy_shape *shape, cpSpaceShapeQueryFunc func, void *data);


//MARK: Iteration

/// Space/body iterator callback function type.
typedef void (*cpSpaceBodyIteratorFunc)(phy_body *body, void *data);
/// Call @c func for each body in the space.
void cpSpaceEachBody(phy_space *space, cpSpaceBodyIteratorFunc func, void *data);

/// Space/body iterator callback function type.
typedef void (*cpSpaceShapeIteratorFunc)(phy_shape *shape, void *data);
/// Call @c func for each shape in the space.
void cpSpaceEachShape(phy_space *space, cpSpaceShapeIteratorFunc func, void *data);

/// Space/constraint iterator callback function type.
typedef void (*cpSpaceConstraintIteratorFunc)(phy_constraint *constraint, void *data);
/// Call @c func for each shape in the space.
void cpSpaceEachConstraint(phy_space *space, cpSpaceConstraintIteratorFunc func, void *data);


//MARK: Indexing

/// Update the collision detection info for the static shapes in the space.
void cpSpaceReindexStatic(phy_space *space);
/// Update the collision detection data for a specific shape in the space.
void cpSpaceReindexShape(phy_space *space, phy_shape *shape);
/// Update the collision detection data for all shapes attached to a body.
void cpSpaceReindexShapesForBody(phy_space *space, phy_body *body);

/// Switch the space to use a spatial has as it's spatial index.
void cpSpaceUseSpatialHash(phy_space *space, float dim, int count);


//MARK: Time Stepping

/// Step the space forward in time by @c dt.
void cpSpaceStep(phy_space *space, float dt);


//MARK: Debug API

#ifndef CP_SPACE_DISABLE_DEBUG_API

/// Color type to use with the space debug drawing API.
typedef struct cpSpaceDebugColor {
	float r, g, b, a;
} cpSpaceDebugColor;

/// Callback type for a function that draws a filled, stroked circle.
typedef void (*cpSpaceDebugDrawCircleImpl)(phy_vect pos, float angle, float radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor, phy_data_pointer data);
/// Callback type for a function that draws a line segment.
typedef void (*cpSpaceDebugDrawSegmentImpl)(phy_vect a, phy_vect b, cpSpaceDebugColor color, phy_data_pointer data);
/// Callback type for a function that draws a thick line segment.
typedef void (*cpSpaceDebugDrawFatSegmentImpl)(phy_vect a, phy_vect b, float radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor, phy_data_pointer data);
/// Callback type for a function that draws a convex polygon.
typedef void (*cpSpaceDebugDrawPolygonImpl)(int count, const phy_vect *verts, float radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor, phy_data_pointer data);
/// Callback type for a function that draws a dot.
typedef void (*cpSpaceDebugDrawDotImpl)(float size, phy_vect pos, cpSpaceDebugColor color, phy_data_pointer data);
/// Callback type for a function that returns a color for a given shape. This gives you an opportunity to color shapes based on how they are used in your engine.
typedef cpSpaceDebugColor (*cpSpaceDebugDrawColorForShapeImpl)(phy_shape *shape, phy_data_pointer data);

typedef enum cpSpaceDebugDrawFlags {
	CP_SPACE_DEBUG_DRAW_SHAPES = 1<<0,
	CP_SPACE_DEBUG_DRAW_CONSTRAINTS = 1<<1,
	CP_SPACE_DEBUG_DRAW_COLLISION_POINTS = 1<<2,
} cpSpaceDebugDrawFlags;

/// Struct used with cpSpaceDebugDraw() containing drawing callbacks and other drawing settings.
typedef struct cpSpaceDebugDrawOptions {
	/// Function that will be invoked to draw circles.
	cpSpaceDebugDrawCircleImpl drawCircle;
	/// Function that will be invoked to draw line segments.
	cpSpaceDebugDrawSegmentImpl drawSegment;
	/// Function that will be invoked to draw thick line segments.
	cpSpaceDebugDrawFatSegmentImpl drawFatSegment;
	/// Function that will be invoked to draw convex polygons.
	cpSpaceDebugDrawPolygonImpl drawPolygon;
	/// Function that will be invoked to draw dots.
	cpSpaceDebugDrawDotImpl drawDot;
	
	/// Flags that request which things to draw (collision shapes, constraints, contact points).
	cpSpaceDebugDrawFlags flags;
	/// Outline color passed to the drawing function.
	cpSpaceDebugColor shapeOutlineColor;
	/// Function that decides what fill color to draw shapes using.
	cpSpaceDebugDrawColorForShapeImpl colorForShape;
	/// Color passed to drawing functions for constraints.
	cpSpaceDebugColor constraintColor;
	/// Color passed to drawing functions for collision points.
	cpSpaceDebugColor collisionPointColor;
	
	/// User defined context pointer passed to all of the callback functions as the 'data' argument.
	phy_data_pointer data;
} cpSpaceDebugDrawOptions;

/// Debug draw the current state of the space using the supplied drawing options.
void cpSpaceDebugDraw(phy_space *space, cpSpaceDebugDrawOptions *options);

#endif
