#include "khg_phy/arbiter.h"
#include "khg_phy/phy_private.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

//MARK: Post Step Callback Functions

phy_post_step_callback *
phy_space_get_post_step_callback(phy_space *space, void *key)
{
	phy_array *arr = space->post_step_callbacks;
	for(int i=0; i<arr->num; i++){
		phy_post_step_callback *callback = (phy_post_step_callback *)arr->arr[i];
		if(callback && callback->key == key) return callback;
	}
	
	return NULL;
}

static void PostStepDoNothing(phy_space *space, void *obj, void *data){}

bool
phy_space_add_post_step_callback(phy_space *space, phy_post_step_func func, void *key, void *data)
{
	if (!space->locked) {
    utl_error_func("Adding a post-step callback when the space is not locked is unnecessary", utl_user_defined_data);
		utl_error_func("Post-step callbacks will not called until the end of the next call to step or the next query", utl_user_defined_data);
  }
	
	if(!phy_space_get_post_step_callback(space, key)){
		phy_post_step_callback *callback = (phy_post_step_callback *)calloc(1, sizeof(phy_post_step_callback));
		callback->func = (func ? func : PostStepDoNothing);
		callback->key = key;
		callback->data = data;
		
		phy_array_push(space->post_step_callbacks, callback);
		return true;
	} else {
		return false;
	}
}

//MARK: Locking Functions

void
phy_space_lock(phy_space *space)
{
	space->locked++;
}

void
phy_space_unlock(phy_space *space, bool runPostStep)
{
	space->locked--;
	if (space->locked < 0) {
    utl_error_func("Space lock underflow", utl_user_defined_data);
  }
	
	if(space->locked == 0){
		phy_array *waking = space->roused_bodies;
		
		for(int i=0, count=waking->num; i<count; i++){
			phy_space_activate_body(space, (phy_body *)waking->arr[i]);
			waking->arr[i] = NULL;
		}
		
		waking->num = 0;
		
		if(space->locked == 0 && runPostStep && !space->skip_post_step){
			space->skip_post_step = true;
			
			phy_array *arr = space->post_step_callbacks;
			for(int i=0; i<arr->num; i++){
				phy_post_step_callback *callback = (phy_post_step_callback *)arr->arr[i];
				phy_post_step_func func = callback->func;
				
				// Mark the func as NULL in case calling it calls cpSpaceRunPostStepCallbacks() again.
				// TODO: need more tests around this case I think.
				callback->func = NULL;
				if(func) func(space, callback->key, callback->data);
				
				arr->arr[i] = NULL;
				free(callback);
			}
			
			arr->num = 0;
			space->skip_post_step = false;
		}
	}
}

//MARK: Contact Buffer Functions

struct phy_contact_buffer_header {
	phy_timestamp stamp;
	phy_contact_buffer_header *next;
	unsigned int numContacts;
};

#define CP_CONTACTS_BUFFER_SIZE ((PHY_BUFFER_BYTES - sizeof(phy_contact_buffer_header))/sizeof(struct phy_contact))
typedef struct cpContactBuffer {
	phy_contact_buffer_header header;
	struct phy_contact contacts[CP_CONTACTS_BUFFER_SIZE];
} cpContactBuffer;

static phy_contact_buffer_header *
cpSpaceAllocContactBuffer(phy_space *space)
{
	cpContactBuffer *buffer = (cpContactBuffer *)calloc(1, sizeof(cpContactBuffer));
	phy_array_push(space->allocated_buffers, buffer);
	return (phy_contact_buffer_header *)buffer;
}

static phy_contact_buffer_header *
cpContactBufferHeaderInit(phy_contact_buffer_header *header, phy_timestamp stamp, phy_contact_buffer_header *splice)
{
	header->stamp = stamp;
	header->next = (splice ? splice->next : header);
	header->numContacts = 0;
	
	return header;
}

void
phy_space_push_fresh_contact_buffer(phy_space *space)
{
	phy_timestamp stamp = space->stamp;
	
	phy_contact_buffer_header *head = space->contact_buffers_head;
	
	if(!head){
		// No buffers have been allocated, make one
		space->contact_buffers_head = cpContactBufferHeaderInit(cpSpaceAllocContactBuffer(space), stamp, NULL);
	} else if(stamp - head->next->stamp > space->collision_persistence){
		// The tail buffer is available, rotate the ring
	phy_contact_buffer_header *tail = head->next;
		space->contact_buffers_head = cpContactBufferHeaderInit(tail, stamp, tail);
	} else {
		// Allocate a new buffer and push it into the ring
		phy_contact_buffer_header *buffer = cpContactBufferHeaderInit(cpSpaceAllocContactBuffer(space), stamp, head);
		space->contact_buffers_head = head->next = buffer;
	}
}


struct phy_contact *
phy_contact_buffer_get_array(phy_space *space)
{
	if(space->contact_buffers_head->numContacts + PHY_MAX_CONTACTS_PER_ARBITER > CP_CONTACTS_BUFFER_SIZE){
		// contact buffer could overflow on the next collision, push a fresh one.
		phy_space_push_fresh_contact_buffer(space);
	}
	
	phy_contact_buffer_header *head = space->contact_buffers_head;
	return ((cpContactBuffer *)head)->contacts + head->numContacts;
}

void
phy_space_push_contacts(phy_space *space, int count)
{
	if (count > PHY_MAX_CONTACTS_PER_ARBITER) {
    utl_error_func("Contact buffer overflow", utl_user_defined_data);
  }
	space->contact_buffers_head->numContacts += count;
}

static void
cpSpacePopContacts(phy_space *space, int count){
	space->contact_buffers_head->numContacts -= count;
}

//MARK: Collision Detection Functions

static void *
cpSpaceArbiterSetTrans(phy_shape **shapes, phy_space *space)
{
	if(space->pooled_arbiters->num == 0){
		// arbiter pool is exhausted, make more
		int count = PHY_BUFFER_BYTES/sizeof(phy_arbiter);
		if (!count) {
      utl_error_func("Buffer size too small", utl_user_defined_data);
    }
		
		phy_arbiter *buffer = (phy_arbiter *)calloc(1, PHY_BUFFER_BYTES);
		phy_array_push(space->allocated_buffers, buffer);
		
		for(int i=0; i<count; i++) phy_array_push(space->pooled_arbiters, buffer + i);
	}
	
	return phy_arbiter_init((phy_arbiter *)phy_array_pop(space->pooled_arbiters), shapes[0], shapes[1]);
}

static inline bool
QueryRejectConstraint(phy_body *a, phy_body *b)
{
	PHY_BODY_FOREACH_CONSTRAINT(a, constraint){
		if(
			!constraint->collide_bodies && (
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
		|| phy_shape_filter_reject(a->filter, b->filter)
		// Don't collide bodies if they have a constraint with collideBodies == false.
		|| QueryRejectConstraint(a->body, b->body)
	);
}

// Callback from the spatial hash.
phy_collision_id
phy_space_collide_shapes(phy_shape *a, phy_shape *b, phy_collision_id id, phy_space *space)
{
	// Reject any of the simple cases
	if(QueryReject(a,b)) return id;
	
	// Narrow-phase collision detection.
	struct phy_collision_info info = phy_collide(a, b, id, phy_contact_buffer_get_array(space));
	
	if(info.count == 0) return info.id; // Shapes are not colliding.
	phy_space_push_contacts(space, info.count);
	
	// Get an arbiter from space->arbiterSet for the two shapes.
	// This is where the persistant contact magic comes from.
	const phy_shape *shape_pair[] = {info.a, info.b};
	phy_hash_value arbHashID = PHY_HASH_PAIR((phy_hash_value)info.a, (phy_hash_value)info.b);
	phy_arbiter *arb = (phy_arbiter *)phy_hash_set_insert(space->cached_arbiters, arbHashID, shape_pair, (phy_hash_set_trans_func)cpSpaceArbiterSetTrans, space);
	phy_arbiter_update(arb, &info, space);
	
	phy_collision_handler *handler = arb->handler;
	
	// Call the begin function first if it's the first step
	if(arb->state == PHY_ARBITER_STATE_FIRST_COLLISION && !handler->begin_func(arb, space, handler->user_data)){
		phy_arbiter_ignore(arb); // permanently ignore the collision until separation
	}
	
	if(
		// Ignore the arbiter if it has been flagged
		(arb->state != PHY_ARBITER_STATE_IGNORE) && 
		// Call preSolve
		handler->pre_solve_func(arb, space, handler->user_data) &&
		// Check (again) in case the pre-solve() callback called cpArbiterIgnored().
		arb->state != PHY_ARBITER_STATE_IGNORE &&
		// Process, but don't add collisions for sensors.
		!(a->sensor || b->sensor) &&
		// Don't process collisions between two infinite mass bodies.
		// This includes collisions between two kinematic bodies, or a kinematic body and a static body.
		!(a->body->m == INFINITY && b->body->m == INFINITY)
	){
		phy_array_push(space->arbiters, arb);
	} else {
		cpSpacePopContacts(space, info.count);
		
		arb->contacts = NULL;
		arb->count = 0;
		
		// Normally arbiters are set as used after calling the post-solve callback.
		// However, post-solve() callbacks are not called for sensors or arbiters rejected from pre-solve.
		if(arb->state != PHY_ARBITER_STATE_IGNORE) arb->state = PHY_ARBITER_STATE_NORMAL;
	}
	
	// Time stamp the arbiter so we know it was used recently.
	arb->stamp = space->stamp;
	return info.id;
}

// Hashset filter func to throw away old arbiters.
bool
phy_space_arbiter_set_filter(phy_arbiter *arb, phy_space *space)
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
	if(ticks >= 1 && arb->state != PHY_ARBITER_STATE_CACHED){
		arb->state = PHY_ARBITER_STATE_CACHED;
		phy_collision_handler *handler = arb->handler;
		handler->separate_func(arb, space, handler->user_data);
	}
	
	if(ticks >= space->collision_persistence){
		arb->contacts = NULL;
		arb->count = 0;
		
		phy_array_push(space->pooled_arbiters, arb);
		return false;
	}
	
	return true;
}

//MARK: All Important cpSpaceStep() Function

 void
phy_shape_update_func(phy_shape *shape, void *unused)
{
	phy_shape_cache_BB(shape);
}

void
phy_space_step(phy_space *space, float dt)
{
	// don't step if the timestep is 0!
	if(dt == 0.0f) return;
	
	space->stamp++;
	
	float prev_dt = space->curr_dt;
	space->curr_dt = dt;
		
	phy_array *bodies = space->dynamic_bodies;
	phy_array *constraints = space->constraints;
	phy_array *arbiters = space->arbiters;
	
	// Reset and empty the arbiter lists.
	for(int i=0; i<arbiters->num; i++){
		phy_arbiter *arb = (phy_arbiter *)arbiters->arr[i];
		arb->state = PHY_ARBITER_STATE_NORMAL;
		
		// If both bodies are awake, unthread the arbiter from the contact graph.
		if(!phy_body_is_sleeping(arb->body_a) && !phy_body_is_sleeping(arb->body_b)){
			phy_arbiter_unthread(arb);
		}
	}
	arbiters->num = 0;

	phy_space_lock(space); {
		// Integrate positions
		for(int i=0; i<bodies->num; i++){
			phy_body *body = (phy_body *)bodies->arr[i];
			body->position_func(body, dt);
		}
		
		// Find colliding pairs.
		phy_space_push_fresh_contact_buffer(space);
		phy_spatial_index_each(space->dynamic_shapes, (phy_spatial_index_iterator_func)phy_shape_update_func, NULL);
		phy_spatial_index_reindex_query(space->dynamic_shapes, (phy_spatial_index_query_func)phy_space_collide_shapes, space);
	} phy_space_unlock(space, false);
	
	// Rebuild the contact graph (and detect sleeping components if sleeping is enabled)
	phy_space_process_components(space, dt);
	
	phy_space_lock(space); {
		// Clear out old cached arbiters and call separate callbacks
		phy_hash_set_filter(space->cached_arbiters, (phy_hash_set_filter_func)phy_space_arbiter_set_filter, space);

		// Prestep the arbiters and constraints.
		float slop = space->collision_slop;
		float biasCoef = 1.0f - powf(space->collision_bias, dt);
		for(int i=0; i<arbiters->num; i++){
			phy_arbiter_pre_step((phy_arbiter *)arbiters->arr[i], dt, slop, biasCoef);
		}

		for(int i=0; i<constraints->num; i++){
			phy_constraint *constraint = (phy_constraint *)constraints->arr[i];
			
			phy_constraint_pre_solve_func preSolve = constraint->pre_solve;
			if(preSolve) preSolve(constraint, space);
			
			constraint->class->pre_step(constraint, dt);
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
			phy_arbiter_apply_cached_impulse((phy_arbiter *)arbiters->arr[i], dt_coef);
		}
		
		for(int i=0; i<constraints->num; i++){
			phy_constraint *constraint = (phy_constraint *)constraints->arr[i];
			constraint->class->apply_cached_impulse(constraint, dt_coef);
		}
		
		// Run the impulse solver.
		for(int i=0; i<space->iterations; i++){
			for(int j=0; j<arbiters->num; j++){
				phy_arbiter_apply_impulse((phy_arbiter *)arbiters->arr[j]);
			}
				
			for(int j=0; j<constraints->num; j++){
				phy_constraint *constraint = (phy_constraint *)constraints->arr[j];
				constraint->class->apply_impulse(constraint, dt);
			}
		}
		
		// Run the constraint post-solve callbacks
		for(int i=0; i<constraints->num; i++){
			phy_constraint *constraint = (phy_constraint *)constraints->arr[i];
			
			phy_constraint_post_solve_func postSolve = constraint->post_solve;
			if(postSolve) postSolve(constraint, space);
		}
		
		// run the post-solve callbacks
		for(int i=0; i<arbiters->num; i++){
			phy_arbiter *arb = (phy_arbiter *) arbiters->arr[i];
			
			phy_collision_handler *handler = arb->handler;
			handler->post_solve_func(arb, space, handler->user_data);
		}
	} phy_space_unlock(space, true);
}

