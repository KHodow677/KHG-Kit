#include <stdio.h>
#include <string.h>

#include "khg_phy/phy_private.h"
#include "khg_utl/error_func.h"

//MARK: Contact Set Helpers

// Equal function for arbiterSet.
static bool
arbiterSetEql(phy_shape **shapes, phy_arbiter *arb)
{
	phy_shape *a = shapes[0];
	phy_shape *b = shapes[1];
	
	return ((a == arb->a && b == arb->b) || (b == arb->a && a == arb->b));
}

//MARK: Collision Handler Set HelperFunctions

// Equals function for collisionHandlers.
static bool
handlerSetEql(phy_collision_handler *check, phy_collision_handler *pair)
{
	return (
		(check->typeA == pair->typeA && check->typeB == pair->typeB) ||
		(check->typeB == pair->typeA && check->typeA == pair->typeB)
	);
}

// Transformation function for collisionHandlers.
static void *
handlerSetTrans(phy_collision_handler *handler, void *unused)
{
	phy_collision_handler *copy = (phy_collision_handler *)calloc(1, sizeof(phy_collision_handler));
	memcpy(copy, handler, sizeof(phy_collision_handler));
	
	return copy;
}

//MARK: Misc Helper Funcs

// Default collision functions.

static bool
DefaultBegin(phy_arbiter *arb, phy_space *space, phy_data_pointer data){
	bool retA = phy_arbiter_call_wildcard_begin_A(arb, space);
	bool retB = phy_arbiter_call_wildcard_begin_B(arb, space);
	return retA && retB;
}

static bool
DefaultPreSolve(phy_arbiter *arb, phy_space *space, phy_data_pointer data){
	bool retA = phy_arbiter_call_wildcard_pre_solve_A(arb, space);
	bool retB = phy_arbiter_call_wildcard_pre_solve_B(arb, space);
	return retA && retB;
}

static void
DefaultPostSolve(phy_arbiter *arb, phy_space *space, phy_data_pointer data){
	phy_arbiter_call_wildcard_post_solve_A(arb, space);
	phy_arbiter_call_wildcard_post_solve_B(arb, space);
}

static void
DefaultSeparate(phy_arbiter *arb, phy_space *space, phy_data_pointer data){
	phy_arbiter_call_wildcard_separate_A(arb, space);
	phy_arbiter_call_wildcard_separate_B(arb, space);
}

// Use the wildcard identifier since  the default handler should never match any type pair.
static phy_collision_handler cpCollisionHandlerDefault = {
	PHY_WILDCARD_COLLISION_TYPE, PHY_WILDCARD_COLLISION_TYPE,
	DefaultBegin, DefaultPreSolve, DefaultPostSolve, DefaultSeparate, NULL
};

static bool AlwaysCollide(phy_arbiter *arb, phy_space *space, phy_data_pointer data){return true;}
static void DoNothing(phy_arbiter *arb, phy_space *space, phy_data_pointer data){}

phy_collision_handler cpCollisionHandlerDoNothing = {
	PHY_WILDCARD_COLLISION_TYPE, PHY_WILDCARD_COLLISION_TYPE,
	AlwaysCollide, AlwaysCollide, DoNothing, DoNothing, NULL
};

// function to get the estimated velocity of a shape for the cpBBTree.
static phy_vect ShapeVelocityFunc(phy_shape *shape){return shape->body->v;}

// Used for disposing of collision handlers.
static void FreeWrap(void *ptr, void *unused){free(ptr);}

//MARK: Memory Management Functions

phy_space *
cpSpaceAlloc(void)
{
	return (phy_space *)calloc(1, sizeof(phy_space));
}

phy_space*
cpSpaceInit(phy_space *space)
{
#ifndef NDEBUG
	static bool done = false;
	if(!done){
		printf("Initializing cpSpace - Chipmunk v%s (Debug Enabled)\n", cpVersionString);
		printf("Compile with -DNDEBUG defined to disable debug mode and runtime assertion checks\n");
		done = true;
	}
#endif

	space->iterations = 10;
	
	space->gravity = cpvzero;
	space->damping = 1.0f;
	
	space->collisionSlop = 0.1f;
	space->collisionBias = powf(1.0f - 0.1f, 60.0f);
	space->collisionPersistence = 3;
	
	space->locked = 0;
	space->stamp = 0;
	
	space->shapeIDCounter = 0;
	space->staticShapes = cpBBTreeNew((cpSpatialIndexBBFunc)cpShapeGetBB, NULL);
	space->dynamicShapes = cpBBTreeNew((cpSpatialIndexBBFunc)cpShapeGetBB, space->staticShapes);
	cpBBTreeSetVelocityFunc(space->dynamicShapes, (cpBBTreeVelocityFunc)ShapeVelocityFunc);
	
	space->allocatedBuffers = cpArrayNew(0);
	
	space->dynamicBodies = cpArrayNew(0);
	space->staticBodies = cpArrayNew(0);
	space->sleepingComponents = cpArrayNew(0);
	space->rousedBodies = cpArrayNew(0);
	
	space->sleepTimeThreshold = INFINITY;
	space->idleSpeedThreshold = 0.0f;
	
	space->arbiters = cpArrayNew(0);
	space->pooledArbiters = cpArrayNew(0);
	
	space->contactBuffersHead = NULL;
	space->cachedArbiters = cpHashSetNew(0, (cpHashSetEqlFunc)arbiterSetEql);
	
	space->constraints = cpArrayNew(0);
	
	space->usesWildcards = false;
	memcpy(&space->defaultHandler, &cpCollisionHandlerDoNothing, sizeof(phy_collision_handler));
	space->collisionHandlers = cpHashSetNew(0, (cpHashSetEqlFunc)handlerSetEql);
	
	space->postStepCallbacks = cpArrayNew(0);
	space->skipPostStep = false;
	
	phy_body *staticBody = phy_body_init(&space->_staticBody, 0.0f, 0.0f);
	phy_body_set_type(staticBody, PHY_BODY_TYPE_STATIC);
	cpSpaceSetStaticBody(space, staticBody);
	
	return space;
}

phy_space*
cpSpaceNew(void)
{
	return cpSpaceInit(cpSpaceAlloc());
}

static void cpBodyActivateWrap(phy_body *body, void *unused){phy_body_activate(body);}

void
cpSpaceDestroy(phy_space *space)
{
	cpSpaceEachBody(space, (cpSpaceBodyIteratorFunc)cpBodyActivateWrap, NULL);
	
	cpSpatialIndexFree(space->staticShapes);
	cpSpatialIndexFree(space->dynamicShapes);
	
	cpArrayFree(space->dynamicBodies);
	cpArrayFree(space->staticBodies);
	cpArrayFree(space->sleepingComponents);
	cpArrayFree(space->rousedBodies);
	
	cpArrayFree(space->constraints);
	
	cpHashSetFree(space->cachedArbiters);
	
	cpArrayFree(space->arbiters);
	cpArrayFree(space->pooledArbiters);
	
	if(space->allocatedBuffers){
		cpArrayFreeEach(space->allocatedBuffers, free);
		cpArrayFree(space->allocatedBuffers);
	}
	
	if(space->postStepCallbacks){
		cpArrayFreeEach(space->postStepCallbacks, free);
		cpArrayFree(space->postStepCallbacks);
	}
	
	if(space->collisionHandlers) cpHashSetEach(space->collisionHandlers, FreeWrap, NULL);
	cpHashSetFree(space->collisionHandlers);
}

void
cpSpaceFree(phy_space *space)
{
	if(space){
		cpSpaceDestroy(space);
		free(space);
	}
}


//MARK: Basic properties:

int
cpSpaceGetIterations(const phy_space *space)
{
	return space->iterations;
}

void
cpSpaceSetIterations(phy_space *space, int iterations)
{
	if (iterations <= 0) {
    utl_error_func("Iterations must be positive and non-zero", utl_user_defined_data);
  }
	space->iterations = iterations;
}

phy_vect
cpSpaceGetGravity(const phy_space *space)
{
	return space->gravity;
}

void
cpSpaceSetGravity(phy_space *space, phy_vect gravity)
{
	space->gravity = gravity;
	
	// Wake up all of the bodies since the gravity changed.
	phy_array *components = space->sleepingComponents;
	for(int i=0; i<components->num; i++){
		phy_body_activate((phy_body *)components->arr[i]);
	}
}

float
cpSpaceGetDamping(const phy_space *space)
{
	return space->damping;
}

void
cpSpaceSetDamping(phy_space *space, float damping)
{
	if (damping < 0.0) {
    utl_error_func("Damping must be positive", utl_user_defined_data);
  }
	space->damping = damping;
}

float
cpSpaceGetIdleSpeedThreshold(const phy_space *space)
{
	return space->idleSpeedThreshold;
}

void
cpSpaceSetIdleSpeedThreshold(phy_space *space, float idleSpeedThreshold)
{
	space->idleSpeedThreshold = idleSpeedThreshold;
}

float
cpSpaceGetSleepTimeThreshold(const phy_space *space)
{
	return space->sleepTimeThreshold;
}

void
cpSpaceSetSleepTimeThreshold(phy_space *space, float sleepTimeThreshold)
{
	space->sleepTimeThreshold = sleepTimeThreshold;
}

float
cpSpaceGetCollisionSlop(const phy_space *space)
{
	return space->collisionSlop;
}

void
cpSpaceSetCollisionSlop(phy_space *space, float collisionSlop)
{
	space->collisionSlop = collisionSlop;
}

float
cpSpaceGetCollisionBias(const phy_space *space)
{
	return space->collisionBias;
}

void
cpSpaceSetCollisionBias(phy_space *space, float collisionBias)
{
	space->collisionBias = collisionBias;
}

phy_timestamp
cpSpaceGetCollisionPersistence(const phy_space *space)
{
	return space->collisionPersistence;
}

void
cpSpaceSetCollisionPersistence(phy_space *space, phy_timestamp collisionPersistence)
{
	space->collisionPersistence = collisionPersistence;
}

phy_data_pointer
cpSpaceGetUserData(const phy_space *space)
{
	return space->userData;
}

void
cpSpaceSetUserData(phy_space *space, phy_data_pointer userData)
{
	space->userData = userData;
}

phy_body *
cpSpaceGetStaticBody(const phy_space *space)
{
	return space->staticBody;
}

float
cpSpaceGetCurrentTimeStep(const phy_space *space)
{
	return space->curr_dt;
}

void
cpSpaceSetStaticBody(phy_space *space, phy_body *body)
{
	if(space->staticBody != NULL){
		if (space->staticBody->shapeList != NULL) {
      utl_error_func("Changing the designated static body while the old one still had shapes attached", utl_user_defined_data);
    }
		space->staticBody->space = NULL;
	}
	
	space->staticBody = body;
	body->space = space;
}

bool
cpSpaceIsLocked(phy_space *space)
{
	return (space->locked > 0);
}

//MARK: Collision Handler Function Management

static void
cpSpaceUseWildcardDefaultHandler(phy_space *space)
{
	// Spaces default to using the slightly faster "do nothing" default handler until wildcards are potentially needed.
	if(!space->usesWildcards){
		space->usesWildcards = true;
		memcpy(&space->defaultHandler, &cpCollisionHandlerDefault, sizeof(phy_collision_handler));
	}
}

phy_collision_handler *cpSpaceAddDefaultCollisionHandler(phy_space *space)
{
	cpSpaceUseWildcardDefaultHandler(space);
	return &space->defaultHandler;
}

phy_collision_handler *cpSpaceAddCollisionHandler(phy_space *space, phy_collision_type a, phy_collision_type b)
{
	phy_hash_value hash = CP_HASH_PAIR(a, b);
	phy_collision_handler handler = {a, b, DefaultBegin, DefaultPreSolve, DefaultPostSolve, DefaultSeparate, NULL};
	return (phy_collision_handler*)cpHashSetInsert(space->collisionHandlers, hash, &handler, (cpHashSetTransFunc)handlerSetTrans, NULL);
}

phy_collision_handler *
cpSpaceAddWildcardHandler(phy_space *space, phy_collision_type type)
{
	cpSpaceUseWildcardDefaultHandler(space);
	
	phy_hash_value hash = CP_HASH_PAIR(type, PHY_WILDCARD_COLLISION_TYPE);
	phy_collision_handler handler = {type, PHY_WILDCARD_COLLISION_TYPE, AlwaysCollide, AlwaysCollide, DoNothing, DoNothing, NULL};
	return (phy_collision_handler*)cpHashSetInsert(space->collisionHandlers, hash, &handler, (cpHashSetTransFunc)handlerSetTrans, NULL);
}


//MARK: Body, Shape, and Joint Management
phy_shape *
cpSpaceAddShape(phy_space *space, phy_shape *shape)
{
	if (shape->space == space) {
    utl_error_func("You have already added this shape to this space, you must not add it a second time", utl_user_defined_data);
  }
	if (shape->space) {
    utl_error_func("You have already added this shape to another space, you cannot add it to a second", utl_user_defined_data);
  }
	if (!shape->body) {
    utl_error_func("The shape's body is not defined", utl_user_defined_data);
  }
	if (shape->body->space != space) {
    utl_error_func("The shape's body must be added to the space before the shape", utl_user_defined_data);
  }
  if (space->locked) {
    utl_error_func("This operation cannot be done safely during a step call or during a query", utl_user_defined_data);
  }
	
	phy_body *body = shape->body;
	
	bool isStatic = (phy_body_get_type(body) == PHY_BODY_TYPE_STATIC);
	if(!isStatic) phy_body_activate(body);
	cpBodyAddShape(body, shape);
	
	shape->hashid = space->shapeIDCounter++;
	cpShapeUpdate(shape, body->transform);
	cpSpatialIndexInsert(isStatic ? space->staticShapes : space->dynamicShapes, shape, shape->hashid);
	shape->space = space;
		
	return shape;
}

phy_body *
cpSpaceAddBody(phy_space *space, phy_body *body)
{
	if (body->space == space) {
    utl_error_func("You have already added this body to this space, you must not add it a second time", utl_user_defined_data);
  }
	if (body->space) {
    utl_error_func("You have already added this body to another space, you cannot add it to a second", utl_user_defined_data);
  }
  if (space->locked) {
    utl_error_func("This operation cannot be done safely during a step call or during a query", utl_user_defined_data);
  }
	
	cpArrayPush(cpSpaceArrayForBodyType(space, phy_body_get_type(body)), body);
	body->space = space;
	
	return body;
}

phy_constraint *
cpSpaceAddConstraint(phy_space *space, phy_constraint *constraint)
{
	if (constraint->space == space) {
    utl_error_func("You have already added this constraint to this space, you must not add it a second time", utl_user_defined_data);
  }
	if (constraint->space) {
    utl_error_func("You have already added this constraint to another space, you cannot add it to a second", utl_user_defined_data);
  }
  if (space->locked) {
    utl_error_func("This operation cannot be done safely during a step call or during a query", utl_user_defined_data);
  }
	
	phy_body *a = constraint->a, *b = constraint->b;
	if (!(a != NULL && b != NULL)) {
    utl_error_func("Constraint is attached to a NULL body", utl_user_defined_data);
  }
	
	phy_body_activate(a);
	phy_body_activate(b);
	cpArrayPush(space->constraints, constraint);
	
	// Push onto the heads of the bodies' constraint lists
	constraint->next_a = a->constraintList; a->constraintList = constraint;
	constraint->next_b = b->constraintList; b->constraintList = constraint;
	constraint->space = space;
	
	return constraint;
}

struct arbiterFilterContext {
	phy_space *space;
	phy_body *body;
	phy_shape *shape;
};

static bool
cachedArbitersFilter(phy_arbiter *arb, struct arbiterFilterContext *context)
{
	phy_shape *shape = context->shape;
	phy_body *body = context->body;
	
	
	// Match on the filter shape, or if it's NULL the filter body
	if(
		(body == arb->body_a && (shape == arb->a || shape == NULL)) ||
		(body == arb->body_b && (shape == arb->b || shape == NULL))
	){
		// Call separate when removing shapes.
		if(shape && arb->state != CP_ARBITER_STATE_CACHED){
			// Invalidate the arbiter since one of the shapes was removed.
			arb->state = CP_ARBITER_STATE_INVALIDATED;
			
			phy_collision_handler *handler = arb->handler;
			handler->separateFunc(arb, context->space, handler->userData);
		}
		
		cpArbiterUnthread(arb);
		cpArrayDeleteObj(context->space->arbiters, arb);
		cpArrayPush(context->space->pooledArbiters, arb);
		
		return false;
	}
	
	return true;
}

void
cpSpaceFilterArbiters(phy_space *space, phy_body *body, phy_shape *filter)
{
	cpSpaceLock(space); {
		struct arbiterFilterContext context = {space, body, filter};
		cpHashSetFilter(space->cachedArbiters, (cpHashSetFilterFunc)cachedArbitersFilter, &context);
	} cpSpaceUnlock(space, true);
}

void
cpSpaceRemoveShape(phy_space *space, phy_shape *shape)
{
	phy_body *body = shape->body;
	if (!cpSpaceContainsShape(space, shape)) {
    utl_error_func("Cannot remove a shape that was not added to the space, likely removed twice", utl_user_defined_data);
  }
  if (space->locked) {
    utl_error_func("This operation cannot be done safely during a step call or during a query", utl_user_defined_data);
  }
	
	bool isStatic = (phy_body_get_type(body) == PHY_BODY_TYPE_STATIC);
	if(isStatic){
		phy_body_activate_static(body, shape);
	} else {
		phy_body_activate(body);
	}

	cpBodyRemoveShape(body, shape);
	cpSpaceFilterArbiters(space, body, shape);
	cpSpatialIndexRemove(isStatic ? space->staticShapes : space->dynamicShapes, shape, shape->hashid);
	shape->space = NULL;
	shape->hashid = 0;
}

void
cpSpaceRemoveBody(phy_space *space, phy_body *body)
{
	if (body == cpSpaceGetStaticBody(space)) {
    utl_error_func("Cannot remove the designated static body for the space", utl_user_defined_data);
  }
	if (!cpSpaceContainsBody(space, body)) {
    utl_error_func("Cannot remove a body that was not added to the space, likely removed twice", utl_user_defined_data);
  }
  if (space->locked) {
    utl_error_func("This operation cannot be done safely during a step call or during a query", utl_user_defined_data);
  }
	
	phy_body_activate(body);
//	cpSpaceFilterArbiters(space, body, NULL);
	cpArrayDeleteObj(cpSpaceArrayForBodyType(space, phy_body_get_type(body)), body);
	body->space = NULL;
}

void
cpSpaceRemoveConstraint(phy_space *space, phy_constraint *constraint)
{
	if (!cpSpaceContainsConstraint(space, constraint)) {
    utl_error_func("Cannot remove a constraint that was not added to the space, likely removed twice", utl_user_defined_data);
  }
  if (space->locked) {
    utl_error_func("This operation cannot be done safely during a step call or during a query", utl_user_defined_data);
  }
	
	phy_body_activate(constraint->a);
	phy_body_activate(constraint->b);
	cpArrayDeleteObj(space->constraints, constraint);
	
	cpBodyRemoveConstraint(constraint->a, constraint);
	cpBodyRemoveConstraint(constraint->b, constraint);
	constraint->space = NULL;
}

bool cpSpaceContainsShape(phy_space *space, phy_shape *shape)
{
	return (shape->space == space);
}

bool cpSpaceContainsBody(phy_space *space, phy_body *body)
{
	return (body->space == space);
}

bool cpSpaceContainsConstraint(phy_space *space, phy_constraint *constraint)
{
	return (constraint->space == space);
}

//MARK: Iteration

void
cpSpaceEachBody(phy_space *space, cpSpaceBodyIteratorFunc func, void *data)
{
	cpSpaceLock(space); {
		phy_array *bodies = space->dynamicBodies;
		for(int i=0; i<bodies->num; i++){
			func((phy_body *)bodies->arr[i], data);
		}
		
		phy_array *otherBodies = space->staticBodies;
		for(int i=0; i<otherBodies->num; i++){
			func((phy_body *)otherBodies->arr[i], data);
		}
		
		phy_array *components = space->sleepingComponents;
		for(int i=0; i<components->num; i++){
			phy_body *root = (phy_body *)components->arr[i];
			
			phy_body *body = root;
			while(body){
				phy_body *next = body->sleeping.next;
				func(body, data);
				body = next;
			}
		}
	} cpSpaceUnlock(space, true);
}

typedef struct spaceShapeContext {
	cpSpaceShapeIteratorFunc func;
	void *data;
} spaceShapeContext;

static void
spaceEachShapeIterator(phy_shape *shape, spaceShapeContext *context)
{
	context->func(shape, context->data);
}

void
cpSpaceEachShape(phy_space *space, cpSpaceShapeIteratorFunc func, void *data)
{
	cpSpaceLock(space); {
		spaceShapeContext context = {func, data};
		cpSpatialIndexEach(space->dynamicShapes, (cpSpatialIndexIteratorFunc)spaceEachShapeIterator, &context);
		cpSpatialIndexEach(space->staticShapes, (cpSpatialIndexIteratorFunc)spaceEachShapeIterator, &context);
	} cpSpaceUnlock(space, true);
}

void
cpSpaceEachConstraint(phy_space *space, cpSpaceConstraintIteratorFunc func, void *data)
{
	cpSpaceLock(space); {
		phy_array *constraints = space->constraints;
		
		for(int i=0; i<constraints->num; i++){
			func((phy_constraint *)constraints->arr[i], data);
		}
	} cpSpaceUnlock(space, true);
}

//MARK: Spatial Index Management

void 
cpSpaceReindexStatic(phy_space *space)
{
	if (space->locked) {
    utl_error_func("You cannot manually reindex objects while the space is locked, wait until the current query or step is complete", utl_user_defined_data);
  }
	
	cpSpatialIndexEach(space->staticShapes, (cpSpatialIndexIteratorFunc)&cpShapeUpdateFunc, NULL);
	cpSpatialIndexReindex(space->staticShapes);
}

void
cpSpaceReindexShape(phy_space *space, phy_shape *shape)
{
  if (space->locked) {
    utl_error_func("You cannot manually reindex objects while the space is locked, wait until the current query or step is complete", utl_user_defined_data);
  }
	
	cpShapeCacheBB(shape);
	
	// attempt to rehash the shape in both hashes
	cpSpatialIndexReindexObject(space->dynamicShapes, shape, shape->hashid);
	cpSpatialIndexReindexObject(space->staticShapes, shape, shape->hashid);
}

void
cpSpaceReindexShapesForBody(phy_space *space, phy_body *body)
{
	CP_BODY_FOREACH_SHAPE(body, shape) cpSpaceReindexShape(space, shape);
}


static void
copyShapes(phy_shape *shape, cpSpatialIndex *index)
{
	cpSpatialIndexInsert(index, shape, shape->hashid);
}

void
cpSpaceUseSpatialHash(phy_space *space, float dim, int count)
{
	cpSpatialIndex *staticShapes = cpSpaceHashNew(dim, count, (cpSpatialIndexBBFunc)cpShapeGetBB, NULL);
	cpSpatialIndex *dynamicShapes = cpSpaceHashNew(dim, count, (cpSpatialIndexBBFunc)cpShapeGetBB, staticShapes);
	
	cpSpatialIndexEach(space->staticShapes, (cpSpatialIndexIteratorFunc)copyShapes, staticShapes);
	cpSpatialIndexEach(space->dynamicShapes, (cpSpatialIndexIteratorFunc)copyShapes, dynamicShapes);
	
	cpSpatialIndexFree(space->staticShapes);
	cpSpatialIndexFree(space->dynamicShapes);
	
	space->staticShapes = staticShapes;
	space->dynamicShapes = dynamicShapes;
}
