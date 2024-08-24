#include "khg_phy/arbiter.h"
#include "khg_phy/phy_private.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

//MARK: Post Step Callback Functions

cpPostStepCallback *
cpSpaceGetPostStepCallback(phy_space *space, void *key)
{
	phy_array *arr = space->postStepCallbacks;
	for(int i=0; i<arr->num; i++){
		cpPostStepCallback *callback = (cpPostStepCallback *)arr->arr[i];
		if(callback && callback->key == key) return callback;
	}
	
	return NULL;
}

static void PostStepDoNothing(phy_space *space, void *obj, void *data){}

bool
cpSpaceAddPostStepCallback(phy_space *space, cpPostStepFunc func, void *key, void *data)
{
	if (!space->locked) {
    utl_error_func("Adding a post-step callback when the space is not locked is unnecessary", utl_user_defined_data);
		utl_error_func("Post-step callbacks will not called until the end of the next call to step or the next query", utl_user_defined_data);
  }
	
	if(!cpSpaceGetPostStepCallback(space, key)){
		cpPostStepCallback *callback = (cpPostStepCallback *)calloc(1, sizeof(cpPostStepCallback));
		callback->func = (func ? func : PostStepDoNothing);
		callback->key = key;
		callback->data = data;
		
		cpArrayPush(space->postStepCallbacks, callback);
		return true;
	} else {
		return false;
	}
}

//MARK: Locking Functions

void
cpSpaceLock(phy_space *space)
{
	space->locked++;
}

void
cpSpaceUnlock(phy_space *space, bool runPostStep)
{
	space->locked--;
	if (space->locked < 0) {
    utl_error_func("Space lock underflow", utl_user_defined_data);
  }
	
	if(space->locked == 0){
		phy_array *waking = space->rousedBodies;
		
		for(int i=0, count=waking->num; i<count; i++){
			cpSpaceActivateBody(space, (phy_body *)waking->arr[i]);
			waking->arr[i] = NULL;
		}
		
		waking->num = 0;
		
		if(space->locked == 0 && runPostStep && !space->skipPostStep){
			space->skipPostStep = true;
			
			phy_array *arr = space->postStepCallbacks;
			for(int i=0; i<arr->num; i++){
				cpPostStepCallback *callback = (cpPostStepCallback *)arr->arr[i];
				cpPostStepFunc func = callback->func;
				
				// Mark the func as NULL in case calling it calls cpSpaceRunPostStepCallbacks() again.
				// TODO: need more tests around this case I think.
				callback->func = NULL;
				if(func) func(space, callback->key, callback->data);
				
				arr->arr[i] = NULL;
				free(callback);
			}
			
			arr->num = 0;
			space->skipPostStep = false;
		}
	}
}

//MARK: Contact Buffer Functions

struct cpContactBufferHeader {
	phy_timestamp stamp;
	cpContactBufferHeader *next;
	unsigned int numContacts;
};

#define CP_CONTACTS_BUFFER_SIZE ((PHY_BUFFER_BYTES - sizeof(cpContactBufferHeader))/sizeof(struct cpContact))
typedef struct cpContactBuffer {
	cpContactBufferHeader header;
	struct cpContact contacts[CP_CONTACTS_BUFFER_SIZE];
} cpContactBuffer;

static cpContactBufferHeader *
cpSpaceAllocContactBuffer(phy_space *space)
{
	cpContactBuffer *buffer = (cpContactBuffer *)calloc(1, sizeof(cpContactBuffer));
	cpArrayPush(space->allocatedBuffers, buffer);
	return (cpContactBufferHeader *)buffer;
}

static cpContactBufferHeader *
cpContactBufferHeaderInit(cpContactBufferHeader *header, phy_timestamp stamp, cpContactBufferHeader *splice)
{
	header->stamp = stamp;
	header->next = (splice ? splice->next : header);
	header->numContacts = 0;
	
	return header;
}

void
cpSpacePushFreshContactBuffer(phy_space *space)
{
	phy_timestamp stamp = space->stamp;
	
	cpContactBufferHeader *head = space->contactBuffersHead;
	
	if(!head){
		// No buffers have been allocated, make one
		space->contactBuffersHead = cpContactBufferHeaderInit(cpSpaceAllocContactBuffer(space), stamp, NULL);
	} else if(stamp - head->next->stamp > space->collisionPersistence){
		// The tail buffer is available, rotate the ring
	cpContactBufferHeader *tail = head->next;
		space->contactBuffersHead = cpContactBufferHeaderInit(tail, stamp, tail);
	} else {
		// Allocate a new buffer and push it into the ring
		cpContactBufferHeader *buffer = cpContactBufferHeaderInit(cpSpaceAllocContactBuffer(space), stamp, head);
		space->contactBuffersHead = head->next = buffer;
	}
}


struct cpContact *
cpContactBufferGetArray(phy_space *space)
{
	if(space->contactBuffersHead->numContacts + PHY_MAX_CONTACTS_PER_ARBITER > CP_CONTACTS_BUFFER_SIZE){
		// contact buffer could overflow on the next collision, push a fresh one.
		cpSpacePushFreshContactBuffer(space);
	}
	
	cpContactBufferHeader *head = space->contactBuffersHead;
	return ((cpContactBuffer *)head)->contacts + head->numContacts;
}

void
cpSpacePushContacts(phy_space *space, int count)
{
	if (count > PHY_MAX_CONTACTS_PER_ARBITER) {
    utl_error_func("Contact buffer overflow", utl_user_defined_data);
  }
	space->contactBuffersHead->numContacts += count;
}

static void
cpSpacePopContacts(phy_space *space, int count){
	space->contactBuffersHead->numContacts -= count;
}

//MARK: Collision Detection Functions

static void *
cpSpaceArbiterSetTrans(phy_shape **shapes, phy_space *space)
{
	if(space->pooledArbiters->num == 0){
		// arbiter pool is exhausted, make more
		int count = PHY_BUFFER_BYTES/sizeof(phy_arbiter);
		if (!count) {
      utl_error_func("Buffer size too small", utl_user_defined_data);
    }
		
		phy_arbiter *buffer = (phy_arbiter *)calloc(1, PHY_BUFFER_BYTES);
		cpArrayPush(space->allocatedBuffers, buffer);
		
		for(int i=0; i<count; i++) cpArrayPush(space->pooledArbiters, buffer + i);
	}
	
	return cpArbiterInit((phy_arbiter *)cpArrayPop(space->pooledArbiters), shapes[0], shapes[1]);
}

static inline bool
QueryRejectConstraint(phy_body *a, phy_body *b)
{
	CP_BODY_FOREACH_CONSTRAINT(a, constraint){
		if(
			!constraint->collideBodies && (
				(constraint->a == a && constraint->b == b) ||
				(constraint->a == b && constraint->b == a)
			)
		) return true;
	}
	
	return false;
}

static inline bool
QueryReject(phy_shape *a, phy_shape *b)
{
	return (
		// BBoxes must overlap
		!phy_bb_intersects(a->bb, b->bb)
		// Don't collide shapes attached to the same body.
		|| a->body == b->body
		// Don't collide shapes that are filtered.
		|| cpShapeFilterReject(a->filter, b->filter)
		// Don't collide bodies if they have a constraint with collideBodies == false.
		|| QueryRejectConstraint(a->body, b->body)
	);
}

// Callback from the spatial hash.
phy_collision_id
cpSpaceCollideShapes(phy_shape *a, phy_shape *b, phy_collision_id id, phy_space *space)
{
	// Reject any of the simple cases
	if(QueryReject(a,b)) return id;
	
	// Narrow-phase collision detection.
	struct cpCollisionInfo info = cpCollide(a, b, id, cpContactBufferGetArray(space));
	
	if(info.count == 0) return info.id; // Shapes are not colliding.
	cpSpacePushContacts(space, info.count);
	
	// Get an arbiter from space->arbiterSet for the two shapes.
	// This is where the persistant contact magic comes from.
	const phy_shape *shape_pair[] = {info.a, info.b};
	phy_hash_value arbHashID = CP_HASH_PAIR((phy_hash_value)info.a, (phy_hash_value)info.b);
	phy_arbiter *arb = (phy_arbiter *)cpHashSetInsert(space->cachedArbiters, arbHashID, shape_pair, (cpHashSetTransFunc)cpSpaceArbiterSetTrans, space);
	cpArbiterUpdate(arb, &info, space);
	
	phy_collision_handler *handler = arb->handler;
	
	// Call the begin function first if it's the first step
	if(arb->state == CP_ARBITER_STATE_FIRST_COLLISION && !handler->beginFunc(arb, space, handler->userData)){
		phy_arbiter_ignore(arb); // permanently ignore the collision until separation
	}
	
	if(
		// Ignore the arbiter if it has been flagged
		(arb->state != CP_ARBITER_STATE_IGNORE) && 
		// Call preSolve
		handler->preSolveFunc(arb, space, handler->userData) &&
		// Check (again) in case the pre-solve() callback called cpArbiterIgnored().
		arb->state != CP_ARBITER_STATE_IGNORE &&
		// Process, but don't add collisions for sensors.
		!(a->sensor || b->sensor) &&
		// Don't process collisions between two infinite mass bodies.
		// This includes collisions between two kinematic bodies, or a kinematic body and a static body.
		!(a->body->m == INFINITY && b->body->m == INFINITY)
	){
		cpArrayPush(space->arbiters, arb);
	} else {
		cpSpacePopContacts(space, info.count);
		
		arb->contacts = NULL;
		arb->count = 0;
		
		// Normally arbiters are set as used after calling the post-solve callback.
		// However, post-solve() callbacks are not called for sensors or arbiters rejected from pre-solve.
		if(arb->state != CP_ARBITER_STATE_IGNORE) arb->state = CP_ARBITER_STATE_NORMAL;
	}
	
	// Time stamp the arbiter so we know it was used recently.
	arb->stamp = space->stamp;
	return info.id;
}

// Hashset filter func to throw away old arbiters.
bool
cpSpaceArbiterSetFilter(phy_arbiter *arb, phy_space *space)
{
	phy_timestamp ticks = space->stamp - arb->stamp;
	
	phy_body *a = arb->body_a, *b = arb->body_b;
	
	// TODO: should make an arbiter state for this so it doesn't require filtering arbiters for dangling body pointers on body removal.
	// Preserve arbiters on sensors and rejected arbiters for sleeping objects.
	// This prevents errant separate callbacks from happenening.
	if(
		(phy_body_get_type(a) == PHY_BODY_TYPE_STATIC || phy_body_is_sleeping(a)) &&
		(phy_body_get_type(b) == PHY_BODY_TYPE_STATIC || phy_body_is_sleeping(b))
	){
		return true;
	}
	
	// Arbiter was used last frame, but not this one
	if(ticks >= 1 && arb->state != CP_ARBITER_STATE_CACHED){
		arb->state = CP_ARBITER_STATE_CACHED;
		phy_collision_handler *handler = arb->handler;
		handler->separateFunc(arb, space, handler->userData);
	}
	
	if(ticks >= space->collisionPersistence){
		arb->contacts = NULL;
		arb->count = 0;
		
		cpArrayPush(space->pooledArbiters, arb);
		return false;
	}
	
	return true;
}

//MARK: All Important cpSpaceStep() Function

 void
cpShapeUpdateFunc(phy_shape *shape, void *unused)
{
	cpShapeCacheBB(shape);
}

void
cpSpaceStep(phy_space *space, float dt)
{
	// don't step if the timestep is 0!
	if(dt == 0.0f) return;
	
	space->stamp++;
	
	float prev_dt = space->curr_dt;
	space->curr_dt = dt;
		
	phy_array *bodies = space->dynamicBodies;
	phy_array *constraints = space->constraints;
	phy_array *arbiters = space->arbiters;
	
	// Reset and empty the arbiter lists.
	for(int i=0; i<arbiters->num; i++){
		phy_arbiter *arb = (phy_arbiter *)arbiters->arr[i];
		arb->state = CP_ARBITER_STATE_NORMAL;
		
		// If both bodies are awake, unthread the arbiter from the contact graph.
		if(!phy_body_is_sleeping(arb->body_a) && !phy_body_is_sleeping(arb->body_b)){
			cpArbiterUnthread(arb);
		}
	}
	arbiters->num = 0;

	cpSpaceLock(space); {
		// Integrate positions
		for(int i=0; i<bodies->num; i++){
			phy_body *body = (phy_body *)bodies->arr[i];
			body->position_func(body, dt);
		}
		
		// Find colliding pairs.
		cpSpacePushFreshContactBuffer(space);
		cpSpatialIndexEach(space->dynamicShapes, (cpSpatialIndexIteratorFunc)cpShapeUpdateFunc, NULL);
		cpSpatialIndexReindexQuery(space->dynamicShapes, (cpSpatialIndexQueryFunc)cpSpaceCollideShapes, space);
	} cpSpaceUnlock(space, false);
	
	// Rebuild the contact graph (and detect sleeping components if sleeping is enabled)
	cpSpaceProcessComponents(space, dt);
	
	cpSpaceLock(space); {
		// Clear out old cached arbiters and call separate callbacks
		cpHashSetFilter(space->cachedArbiters, (cpHashSetFilterFunc)cpSpaceArbiterSetFilter, space);

		// Prestep the arbiters and constraints.
		float slop = space->collisionSlop;
		float biasCoef = 1.0f - powf(space->collisionBias, dt);
		for(int i=0; i<arbiters->num; i++){
			cpArbiterPreStep((phy_arbiter *)arbiters->arr[i], dt, slop, biasCoef);
		}

		for(int i=0; i<constraints->num; i++){
			phy_constraint *constraint = (phy_constraint *)constraints->arr[i];
			
			phy_constraint_pre_solve_func preSolve = constraint->preSolve;
			if(preSolve) preSolve(constraint, space);
			
			constraint->klass->preStep(constraint, dt);
		}
	
		// Integrate velocities.
		float damping = powf(space->damping, dt);
		phy_vect gravity = space->gravity;
		for(int i=0; i<bodies->num; i++){
			phy_body *body = (phy_body *)bodies->arr[i];
			body->velocity_func(body, gravity, damping, dt);
		}
		
		// Apply cached impulses
		float dt_coef = (prev_dt == 0.0f ? 0.0f : dt/prev_dt);
		for(int i=0; i<arbiters->num; i++){
			cpArbiterApplyCachedImpulse((phy_arbiter *)arbiters->arr[i], dt_coef);
		}
		
		for(int i=0; i<constraints->num; i++){
			phy_constraint *constraint = (phy_constraint *)constraints->arr[i];
			constraint->klass->applyCachedImpulse(constraint, dt_coef);
		}
		
		// Run the impulse solver.
		for(int i=0; i<space->iterations; i++){
			for(int j=0; j<arbiters->num; j++){
				cpArbiterApplyImpulse((phy_arbiter *)arbiters->arr[j]);
			}
				
			for(int j=0; j<constraints->num; j++){
				phy_constraint *constraint = (phy_constraint *)constraints->arr[j];
				constraint->klass->applyImpulse(constraint, dt);
			}
		}
		
		// Run the constraint post-solve callbacks
		for(int i=0; i<constraints->num; i++){
			phy_constraint *constraint = (phy_constraint *)constraints->arr[i];
			
			phy_constraint_post_solve_func postSolve = constraint->postSolve;
			if(postSolve) postSolve(constraint, space);
		}
		
		// run the post-solve callbacks
		for(int i=0; i<arbiters->num; i++){
			phy_arbiter *arb = (phy_arbiter *) arbiters->arr[i];
			
			phy_collision_handler *handler = arb->handler;
			handler->postSolveFunc(arb, space, handler->userData);
		}
	} cpSpaceUnlock(space, true);
}

