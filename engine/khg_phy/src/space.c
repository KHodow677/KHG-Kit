#include "khg_phy/arbiter.h"	
#include "khg_phy/phy_private.h"
#include "khg_utl/error_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
		(check->type_A == pair->type_A && check->type_B == pair->type_B) ||
		(check->type_B == pair->type_A && check->type_A == pair->type_B)
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

phy_collision_handler phy_collision_handler_do_nothing = {
	PHY_WILDCARD_COLLISION_TYPE, PHY_WILDCARD_COLLISION_TYPE,
	AlwaysCollide, AlwaysCollide, DoNothing, DoNothing, NULL
};

// function to get the estimated velocity of a shape for the cpBBTree.
static phy_vect ShapeVelocityFunc(phy_shape *shape){return shape->body->v;}

// Used for disposing of collision handlers.
static void FreeWrap(void *ptr, void *unused){free(ptr);}

//MARK: Memory Management Functions

phy_space *
phy_space_alloc(void)
{
	return (phy_space *)calloc(1, sizeof(phy_space));
}

phy_space*
phy_space_init(phy_space *space)
{
	space->iterations = 10;
	
	space->gravity = phy_v_zero;
	space->damping = 1.0f;
	
	space->collision_slop = 0.1f;
	space->collision_bias = powf(1.0f - 0.1f, 60.0f);
	space->collision_persistence = 3;
	
	space->locked = 0;
	space->stamp = 0;
	
	space->shape_id_Counter = 0;
	space->static_shapes = phy_BB_tree_new((phy_spatial_index_BB_func)phy_shape_get_BB, NULL);
	space->dynamic_shapes = phy_BB_tree_new((phy_spatial_index_BB_func)phy_shape_get_BB, space->static_shapes);
	phy_BB_tree_set_velocity_func(space->dynamic_shapes, (phy_bb_tree_velocity_func)ShapeVelocityFunc);
	
	space->allocated_buffers = phy_array_new(0);
	
	space->dynamic_bodies = phy_array_new(0);
	space->static_bodies = phy_array_new(0);
	space->sleeping_components = phy_array_new(0);
	space->roused_bodies = phy_array_new(0);
	
	space->sleep_time_threshold = INFINITY;
	space->idle_speed_threshold = 0.0f;
	
	space->arbiters = phy_array_new(0);
	space->pooled_arbiters = phy_array_new(0);
	
	space->contact_buffers_head = NULL;
	space->cached_arbiters = cp_hash_set_new(0, (phy_hash_set_eql_func)arbiterSetEql);
	
	space->constraints = phy_array_new(0);
	
	space->uses_wildcards = false;
	memcpy(&space->default_handler, &phy_collision_handler_do_nothing, sizeof(phy_collision_handler));
	space->collision_handlers = cp_hash_set_new(0, (phy_hash_set_eql_func)handlerSetEql);
	
	space->post_step_callbacks = phy_array_new(0);
	space->skip_post_step = false;
	
	phy_body *staticBody = phy_body_init(&space->fixed_static_body, 0.0f, 0.0f);
	phy_body_set_type(staticBody, PHY_BODY_TYPE_STATIC);
	phy_space_set_static_body(space, staticBody);
	
	return space;
}

phy_space*
phy_space_new(void)
{
	return phy_space_init(phy_space_alloc());
}

static void cpBodyActivateWrap(phy_body *body, void *unused){phy_body_activate(body);}

void
phy_space_destroy(phy_space *space)
{
	phy_space_each_body(space, (phy_space_body_iterator_func)cpBodyActivateWrap, NULL);
	
	phy_spatial_index_free(space->static_shapes);
	phy_spatial_index_free(space->dynamic_shapes);
	
	phy_array_free(space->dynamic_bodies);
	phy_array_free(space->static_bodies);
	phy_array_free(space->sleeping_components);
	phy_array_free(space->roused_bodies);
	
	phy_array_free(space->constraints);
	
	phy_hash_set_free(space->cached_arbiters);
	
	phy_array_free(space->arbiters);
	phy_array_free(space->pooled_arbiters);
	
	if(space->allocated_buffers){
		phy_array_free_each(space->allocated_buffers, free);
		phy_array_free(space->allocated_buffers);
	}
	
	if(space->post_step_callbacks){
		phy_array_free_each(space->post_step_callbacks, free);
		phy_array_free(space->post_step_callbacks);
	}
	
	if(space->collision_handlers) phy_hash_set_each(space->collision_handlers, FreeWrap, NULL);
	phy_hash_set_free(space->collision_handlers);
}

void
phy_space_free(phy_space *space)
{
	if(space){
		phy_space_destroy(space);
		free(space);
	}
}


//MARK: Basic properties:

int
phy_space_get_iterations(const phy_space *space)
{
	return space->iterations;
}

void
phy_space_set_iterations(phy_space *space, int iterations)
{
	if (iterations <= 0) {
    utl_error_func("Iterations must be positive and non-zero", utl_user_defined_data);
  }
	space->iterations = iterations;
}

phy_vect
phy_space_get_gravity(const phy_space *space)
{
	return space->gravity;
}

void
phy_space_set_gravity(phy_space *space, phy_vect gravity)
{
	space->gravity = gravity;
	
	// Wake up all of the bodies since the gravity changed.
	phy_array *components = space->sleeping_components;
	for(int i=0; i<components->num; i++){
		phy_body_activate((phy_body *)components->arr[i]);
	}
}

float
phy_space_get_damping(const phy_space *space)
{
	return space->damping;
}

void
phy_space_set_damping(phy_space *space, float damping)
{
	if (damping < 0.0) {
    utl_error_func("Damping must be positive", utl_user_defined_data);
  }
	space->damping = damping;
}

float
phy_space_get_idle_speed_threshold(const phy_space *space)
{
	return space->idle_speed_threshold;
}

void
phy_space_set_idle_speed_threshold(phy_space *space, float idleSpeedThreshold)
{
	space->idle_speed_threshold = idleSpeedThreshold;
}

float
phy_space_get_sleep_time_threshold(const phy_space *space)
{
	return space->sleep_time_threshold;
}

void
phy_space_set_sleep_time_threshold(phy_space *space, float sleepTimeThreshold)
{
	space->sleep_time_threshold = sleepTimeThreshold;
}

float
phy_space_get_collision_slop(const phy_space *space)
{
	return space->collision_slop;
}

void
phy_space_set_collision_slop(phy_space *space, float collisionSlop)
{
	space->collision_slop = collisionSlop;
}

float
phy_space_get_collision_bias(const phy_space *space)
{
	return space->collision_bias;
}

void
phy_space_set_collision_bias(phy_space *space, float collisionBias)
{
	space->collision_bias = collisionBias;
}

phy_timestamp
phy_space_get_collision_persistence(const phy_space *space)
{
	return space->collision_persistence;
}

void
phy_space_set_collision_persistence(phy_space *space, phy_timestamp collisionPersistence)
{
	space->collision_persistence = collisionPersistence;
}

phy_data_pointer
phy_space_get_user_data(const phy_space *space)
{
	return space->user_data;
}

void
phy_space_set_user_data(phy_space *space, phy_data_pointer userData)
{
	space->user_data = userData;
}

phy_body *
phy_space_get_static_body(const phy_space *space)
{
	return space->static_body;
}

float
phy_space_get_current_time_step(const phy_space *space)
{
	return space->curr_dt;
}

void
phy_space_set_static_body(phy_space *space, phy_body *body)
{
	if(space->static_body != NULL){
		if (space->static_body->shape_list != NULL) {
      utl_error_func("Changing the designated static body while the old one still had shapes attached", utl_user_defined_data);
    }
		space->static_body->space = NULL;
	}
	
	space->static_body = body;
	body->space = space;
}

bool
phy_space_is_locked(phy_space *space)
{
	return (space->locked > 0);
}

//MARK: Collision Handler Function Management

static void
cpSpaceUseWildcardDefaultHandler(phy_space *space)
{
	// Spaces default to using the slightly faster "do nothing" default handler until wildcards are potentially needed.
	if(!space->uses_wildcards){
		space->uses_wildcards = true;
		memcpy(&space->default_handler, &cpCollisionHandlerDefault, sizeof(phy_collision_handler));
	}
}

phy_collision_handler *phy_space_add_default_collision_handler(phy_space *space)
{
	cpSpaceUseWildcardDefaultHandler(space);
	return &space->default_handler;
}

phy_collision_handler *phy_space_add_collision_handler(phy_space *space, phy_collision_type a, phy_collision_type b)
{
	phy_hash_value hash = PHY_HASH_PAIR(a, b);
	phy_collision_handler handler = {a, b, DefaultBegin, DefaultPreSolve, DefaultPostSolve, DefaultSeparate, NULL};
	return (phy_collision_handler*)phy_hash_set_insert(space->collision_handlers, hash, &handler, (phy_hash_set_trans_func)handlerSetTrans, NULL);
}

phy_collision_handler *
phy_space_add_wildcard_handler(phy_space *space, phy_collision_type type)
{
	cpSpaceUseWildcardDefaultHandler(space);
	
	phy_hash_value hash = PHY_HASH_PAIR(type, PHY_WILDCARD_COLLISION_TYPE);
	phy_collision_handler handler = {type, PHY_WILDCARD_COLLISION_TYPE, AlwaysCollide, AlwaysCollide, DoNothing, DoNothing, NULL};
	return (phy_collision_handler*)phy_hash_set_insert(space->collision_handlers, hash, &handler, (phy_hash_set_trans_func)handlerSetTrans, NULL);
}


//MARK: Body, Shape, and Joint Management
phy_shape *
phy_space_add_shape(phy_space *space, phy_shape *shape)
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
	phy_body_add_shape(body, shape);
	
	shape->hashid = space->shape_id_Counter++;
	phy_shape_update(shape, body->transform);
	phy_spatial_index_insert(isStatic ? space->static_shapes : space->dynamic_shapes, shape, shape->hashid);
	shape->space = space;
		
	return shape;
}

phy_body *
phy_space_add_body(phy_space *space, phy_body *body)
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
	
	phy_array_push(phy_space_array_for_body_type(space, phy_body_get_type(body)), body);
	body->space = space;
	
	return body;
}

phy_constraint *
phy_space_add_constraint(phy_space *space, phy_constraint *constraint)
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
	phy_array_push(space->constraints, constraint);
	
	// Push onto the heads of the bodies' constraint lists
	constraint->next_a = a->constraint_list; a->constraint_list = constraint;
	constraint->next_b = b->constraint_list; b->constraint_list = constraint;
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
		if(shape && arb->state != PHY_ARBITER_STATE_CACHED){
			// Invalidate the arbiter since one of the shapes was removed.
			arb->state = PHY_ARBITER_STATE_INVALIDATED;
			
			phy_collision_handler *handler = arb->handler;
			handler->separate_func(arb, context->space, handler->user_data);
		}
		
		phy_arbiter_unthread(arb);
		phy_array_delete_obj(context->space->arbiters, arb);
		phy_array_push(context->space->pooled_arbiters, arb);
		
		return false;
	}
	
	return true;
}

void
phy_space_filter_arbiters(phy_space *space, phy_body *body, phy_shape *filter)
{
	phy_space_lock(space); {
		struct arbiterFilterContext context = {space, body, filter};
		phy_hash_set_filter(space->cached_arbiters, (phy_hash_set_filter_func)cachedArbitersFilter, &context);
	} phy_space_unlock(space, true);
}

void
phy_space_remove_shape(phy_space *space, phy_shape *shape)
{
	phy_body *body = shape->body;
	if (!phy_space_contains_shape(space, shape)) {
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

	phy_body_remove_shape(body, shape);
	phy_space_filter_arbiters(space, body, shape);
	phy_spatial_index_remove(isStatic ? space->static_shapes : space->dynamic_shapes, shape, shape->hashid);
	shape->space = NULL;
	shape->hashid = 0;
}

void
phy_space_remove_body(phy_space *space, phy_body *body)
{
	if (body == phy_space_get_static_body(space)) {
    utl_error_func("Cannot remove the designated static body for the space", utl_user_defined_data);
  }
	if (!phy_space_contains_body(space, body)) {
    utl_error_func("Cannot remove a body that was not added to the space, likely removed twice", utl_user_defined_data);
  }
  if (space->locked) {
    utl_error_func("This operation cannot be done safely during a step call or during a query", utl_user_defined_data);
  }
	
	phy_body_activate(body);
//	cpSpaceFilterArbiters(space, body, NULL);
	phy_array_delete_obj(phy_space_array_for_body_type(space, phy_body_get_type(body)), body);
	body->space = NULL;
}

void
phy_space_remove_constraint(phy_space *space, phy_constraint *constraint)
{
	if (!phy_space_contains_constraint(space, constraint)) {
    utl_error_func("Cannot remove a constraint that was not added to the space, likely removed twice", utl_user_defined_data);
  }
  if (space->locked) {
    utl_error_func("This operation cannot be done safely during a step call or during a query", utl_user_defined_data);
  }
	
	phy_body_activate(constraint->a);
	phy_body_activate(constraint->b);
	phy_array_delete_obj(space->constraints, constraint);
	
	phy_body_remove_constraint(constraint->a, constraint);
	phy_body_remove_constraint(constraint->b, constraint);
	constraint->space = NULL;
}

bool phy_space_contains_shape(phy_space *space, phy_shape *shape)
{
	return (shape->space == space);
}

bool phy_space_contains_body(phy_space *space, phy_body *body)
{
	return (body->space == space);
}

bool phy_space_contains_constraint(phy_space *space, phy_constraint *constraint)
{
	return (constraint->space == space);
}

//MARK: Iteration

void phy_space_each_body(phy_space *space, phy_space_body_iterator_func func, void *data)
{
	phy_space_lock(space); {
		phy_array *bodies = space->dynamic_bodies;
		for(int i=0; i<bodies->num; i++){
			func((phy_body *)bodies->arr[i], data);
		}
		
		phy_array *otherBodies = space->static_bodies;
		for(int i=0; i<otherBodies->num; i++){
			func((phy_body *)otherBodies->arr[i], data);
		}
		
		phy_array *components = space->sleeping_components;
		for(int i=0; i<components->num; i++){
			phy_body *root = (phy_body *)components->arr[i];
			
			phy_body *body = root;
			while(body){
				phy_body *next = body->sleeping.next;
				func(body, data);
				body = next;
			}
		}
	} phy_space_unlock(space, true);
}

typedef struct spaceShapeContext {
	phy_space_shape_iterator_func func;
	void *data;
} spaceShapeContext;

static void
spaceEachShapeIterator(phy_shape *shape, spaceShapeContext *context)
{
	context->func(shape, context->data);
}

void
phy_space_each_shape(phy_space *space, phy_space_shape_iterator_func func, void *data)
{
	phy_space_lock(space); {
		spaceShapeContext context = {func, data};
		phy_spatial_index_each(space->dynamic_shapes, (phy_spatial_index_iterator_func)spaceEachShapeIterator, &context);
		phy_spatial_index_each(space->static_shapes, (phy_spatial_index_iterator_func)spaceEachShapeIterator, &context);
	} phy_space_unlock(space, true);
}

void
phy_space_each_constraint(phy_space *space, phy_space_constraint_iterator_func func, void *data)
{
	phy_space_lock(space); {
		phy_array *constraints = space->constraints;
		
		for(int i=0; i<constraints->num; i++){
			func((phy_constraint *)constraints->arr[i], data);
		}
	} phy_space_unlock(space, true);
}

//MARK: Spatial Index Management

void 
phy_space_reindex_static(phy_space *space)
{
	if (space->locked) {
    utl_error_func("You cannot manually reindex objects while the space is locked, wait until the current query or step is complete", utl_user_defined_data);
  }
	
	phy_spatial_index_each(space->static_shapes, (phy_spatial_index_iterator_func)&phy_shape_update_func, NULL);
	phy_spatial_index_reindex(space->static_shapes);
}

void
phy_space_reindex_shape(phy_space *space, phy_shape *shape)
{
  if (space->locked) {
    utl_error_func("You cannot manually reindex objects while the space is locked, wait until the current query or step is complete", utl_user_defined_data);
  }
	
	phy_shape_cache_BB(shape);
	
	// attempt to rehash the shape in both hashes
	phy_spatial_index_reindex_object(space->dynamic_shapes, shape, shape->hashid);
	phy_spatial_index_reindex_object(space->static_shapes, shape, shape->hashid);
}

void
phy_space_reindex_shapes_for_body(phy_space *space, phy_body *body)
{
	PHY_BODY_FOREACH_SHAPE(body, shape) phy_space_reindex_shape(space, shape);
}


static void
copyShapes(phy_shape *shape, phy_spatial_index *index)
{
	phy_spatial_index_insert(index, shape, shape->hashid);
}

void
phy_space_use_spatial_hash(phy_space *space, float dim, int count)
{
	phy_spatial_index *staticShapes = phy_space_hash_new(dim, count, (phy_spatial_index_BB_func)phy_shape_get_BB, NULL);
	phy_spatial_index *dynamicShapes = phy_space_hash_new(dim, count, (phy_spatial_index_BB_func)phy_shape_get_BB, staticShapes);
	
	phy_spatial_index_each(space->static_shapes, (phy_spatial_index_iterator_func)copyShapes, staticShapes);
	phy_spatial_index_each(space->dynamic_shapes, (phy_spatial_index_iterator_func)copyShapes, dynamicShapes);
	
	phy_spatial_index_free(space->static_shapes);
	phy_spatial_index_free(space->dynamic_shapes);
	
	space->static_shapes = staticShapes;
	space->dynamic_shapes = dynamicShapes;
}
