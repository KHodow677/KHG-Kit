#include "khg_phy/phy_private.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>
#include <string.h>

//MARK: Sleeping Functions

void
phy_space_activate_body(phy_space *space, phy_body *body)
{
	if (phy_body_get_type(body) != PHY_BODY_TYPE_DYNAMIC) {
    utl_error_func("Attempting to activate a non-dynamic body", utl_user_defined_data);
  }
		
	if(space->locked){
		// cpSpaceActivateBody() is called again once the space is unlocked
		if(!phy_array_contains(space->roused_bodies, body)) phy_array_push(space->roused_bodies, body);
	} else {
		if (!(body->sleeping.root == NULL && body->sleeping.next == NULL)) {
      utl_error_func("Activating body non-NULL node pointers", utl_user_defined_data);
    }
		phy_array_push(space->dynamic_bodies, body);

		PHY_BODY_FOREACH_SHAPE(body, shape){
			phy_spatial_index_remove(space->static_shapes, shape, shape->hashid);
			phy_spatial_index_insert(space->dynamic_shapes, shape, shape->hashid);
		}
		
		PHY_BODY_FOREACH_ARBITER(body, arb){
			phy_body *bodyA = arb->body_a;
			
			// Arbiters are shared between two bodies that are always woken up together.
			// You only want to restore the arbiter once, so bodyA is arbitrarily chosen to own the arbiter.
			// The edge case is when static bodies are involved as the static bodies never actually sleep.
			// If the static body is bodyB then all is good. If the static body is bodyA, that can easily be checked.
			if(body == bodyA || phy_body_get_type(bodyA) == PHY_BODY_TYPE_STATIC){
				int numContacts = arb->count;
				struct phy_contact *contacts = arb->contacts;
				
				// Restore contact values back to the space's contact buffer memory
				arb->contacts = phy_contact_buffer_get_array(space);
				memcpy(arb->contacts, contacts, numContacts*sizeof(struct phy_contact));
				phy_space_push_contacts(space, numContacts);
				
				// Reinsert the arbiter into the arbiter cache
				const phy_shape *a = arb->a, *b = arb->b;
				const phy_shape *shape_pair[] = {a, b};
				phy_hash_value arbHashID = PHY_HASH_PAIR((phy_hash_value)a, (phy_hash_value)b);
				phy_hash_set_insert(space->cached_arbiters, arbHashID, shape_pair, NULL, arb);
				
				// Update the arbiter's state
				arb->stamp = space->stamp;
				phy_array_push(space->arbiters, arb);
				
				free(contacts);
			}
		}
		
		PHY_BODY_FOREACH_CONSTRAINT(body, constraint){
			phy_body *bodyA = constraint->a;
			if(body == bodyA || phy_body_get_type(bodyA) == PHY_BODY_TYPE_STATIC) phy_array_push(space->constraints, constraint);
		}
	}
}

static void
cpSpaceDeactivateBody(phy_space *space, phy_body *body)
{
	if (phy_body_get_type(body) != PHY_BODY_TYPE_DYNAMIC) {
    utl_error_func("Attempting to deactivate a non-dynamic body", utl_user_defined_data);
  }
	
	phy_array_delete_obj(space->dynamic_bodies, body);
	
	PHY_BODY_FOREACH_SHAPE(body, shape){
		phy_spatial_index_remove(space->dynamic_shapes, shape, shape->hashid);
		phy_spatial_index_insert(space->static_shapes, shape, shape->hashid);
	}
	
	PHY_BODY_FOREACH_ARBITER(body, arb){
		phy_body *bodyA = arb->body_a;
		if(body == bodyA || phy_body_get_type(bodyA) == PHY_BODY_TYPE_STATIC){
			phy_space_uncache_arbiter(space, arb);
			
			// Save contact values to a new block of memory so they won't time out
			size_t bytes = arb->count*sizeof(struct phy_contact);
			struct phy_contact *contacts = (struct phy_contact *)calloc(1, bytes);
			memcpy(contacts, arb->contacts, bytes);
			arb->contacts = contacts;
		}
	}
		
	PHY_BODY_FOREACH_CONSTRAINT(body, constraint){
		phy_body *bodyA = constraint->a;
		if(body == bodyA || phy_body_get_type(bodyA) == PHY_BODY_TYPE_STATIC) phy_array_delete_obj(space->constraints, constraint);
	}
}

static inline phy_body *
ComponentRoot(phy_body *body)
{
	return (body ? body->sleeping.root : NULL);
}

void
phy_body_activate(phy_body *body)
{
	if(body != NULL && phy_body_get_type(body) == PHY_BODY_TYPE_DYNAMIC){
		body->sleeping.idle_time = 0.0f;
		
		phy_body *root = ComponentRoot(body);
		if(root && phy_body_is_sleeping(root)){
			// TODO should cpBodyIsSleeping(root) be an assertion?
			if (phy_body_get_type(root) != PHY_BODY_TYPE_DYNAMIC) {
        utl_error_func("Non-dynamic body component root detected", utl_user_defined_data);
      }
			
			phy_space *space = root->space;
			phy_body *body = root;
			while(body){
				phy_body *next = body->sleeping.next;
				
				body->sleeping.idle_time = 0.0f;
				body->sleeping.root = NULL;
				body->sleeping.next = NULL;
				phy_space_activate_body(space, body);
				
				body = next;
			}
			
			phy_array_delete_obj(space->sleeping_components, root);
		}
		
		PHY_BODY_FOREACH_ARBITER(body, arb){
			// Reset the idle timer of things the body is touching as well.
			// That way things don't get left hanging in the air.
			phy_body *other = (arb->body_a == body ? arb->body_b : arb->body_a);
			if(phy_body_get_type(other) != PHY_BODY_TYPE_STATIC) other->sleeping.idle_time = 0.0f;
		}
	}
}

void
phy_body_activate_static(phy_body *body, phy_shape *filter)
{
	if (phy_body_get_type(body) != PHY_BODY_TYPE_STATIC) {
    utl_error_func("Called on a non-static body", utl_user_defined_data);
  }
	
	PHY_BODY_FOREACH_ARBITER(body, arb){
		if(!filter || filter == arb->a || filter == arb->b){
			phy_body_activate(arb->body_a == body ? arb->body_b : arb->body_a);
		}
	}
	
	// TODO: should also activate joints?
}

static inline void
cpBodyPushArbiter(phy_body *body, phy_arbiter *arb)
{
	if (phy_arbiter_thread_for_body(arb, body)->next != NULL) {
    utl_error_func("Dangling contact graph pointers detected", utl_user_defined_data);
  }
	if (phy_arbiter_thread_for_body(arb, body)->prev != NULL) {
    utl_error_func("Dangling contact graph pointers detected", utl_user_defined_data);
  }
	
	phy_arbiter *next = body->arbiter_list;
	if (!(next == NULL || phy_arbiter_thread_for_body(next, body)->prev == NULL)) {
    utl_error_func("Dangling contact graph pointers detected", utl_user_defined_data);
  }
	phy_arbiter_thread_for_body(arb, body)->next = next;
	
	if(next) phy_arbiter_thread_for_body(next, body)->prev = arb;
	body->arbiter_list = arb;
}

static inline void
ComponentAdd(phy_body *root, phy_body *body){
	body->sleeping.root = root;

	if(body != root){
		body->sleeping.next = root->sleeping.next;
		root->sleeping.next = body;
	}
}

static inline void
FloodFillComponent(phy_body *root, phy_body *body)
{
	// Kinematic bodies cannot be put to sleep and prevent bodies they are touching from sleeping.
	// Static bodies are effectively sleeping all the time.
	if(phy_body_get_type(body) == PHY_BODY_TYPE_DYNAMIC){
		phy_body *other_root = ComponentRoot(body);
		if(other_root == NULL){
			ComponentAdd(root, body);
			PHY_BODY_FOREACH_ARBITER(body, arb) FloodFillComponent(root, (body == arb->body_a ? arb->body_b : arb->body_a));
			PHY_BODY_FOREACH_CONSTRAINT(body, constraint) FloodFillComponent(root, (body == constraint->a ? constraint->b : constraint->a));
		} else {
			if (other_root != root) {
        utl_error_func("Inconsistency dectected in the contact graph", utl_user_defined_data);
      }
		}
	}
}

static inline bool
ComponentActive(phy_body *root, float threshold)
{
	PHY_BODY_FOREACH_COMPONENT(root, body){
		if(body->sleeping.idle_time < threshold) return true;
	}
	
	return false;
}

void
phy_space_process_components(phy_space *space, float dt)
{
	bool sleep = (space->sleep_time_threshold != INFINITY);
	phy_array *bodies = space->dynamic_bodies;
	
#ifndef NDEBUG
	for(int i=0; i<bodies->num; i++){
		phy_body *body = (phy_body *)bodies->arr[i];
		
		if (body->sleeping.next != NULL) {
      utl_error_func("Dangling next pointer detected in contact graph", utl_user_defined_data);
    }
		if (body->sleeping.root != NULL) {
      utl_error_func("Dangling root pointer detected in contact graph", utl_user_defined_data);
    }
	}
#endif
	
	// Calculate the kinetic energy of all the bodies.
	if(sleep){
		float dv = space->idle_speed_threshold;
		float dvsq = (dv ? dv*dv : phy_v_length_sq(space->gravity)*dt*dt);
		
		// update idling and reset component nodes
		for(int i=0; i<bodies->num; i++){
			phy_body *body = (phy_body *)bodies->arr[i];
			
			// TODO should make a separate array for kinematic bodies.
			if(phy_body_get_type(body) != PHY_BODY_TYPE_DYNAMIC) continue;
			
			// Need to deal with infinite mass objects
			float keThreshold = (dvsq ? body->m*dvsq : 0.0f);
			body->sleeping.idle_time = (phy_body_kinetic_energy(body) > keThreshold ? 0.0f : body->sleeping.idle_time + dt);
		}
	}
	
	// Awaken any sleeping bodies found and then push arbiters to the bodies' lists.
	phy_array *arbiters = space->arbiters;
	for(int i=0, count=arbiters->num; i<count; i++){
		phy_arbiter *arb = (phy_arbiter*)arbiters->arr[i];
		phy_body *a = arb->body_a, *b = arb->body_b;
		
		if(sleep){
			// TODO checking cpBodyIsSleepin() redundant?
			if(phy_body_get_type(b) == PHY_BODY_TYPE_KINEMATIC || phy_body_is_sleeping(a)) phy_body_activate(a);
			if(phy_body_get_type(a) == PHY_BODY_TYPE_KINEMATIC || phy_body_is_sleeping(b)) phy_body_activate(b);
		}
		
		cpBodyPushArbiter(a, arb);
		cpBodyPushArbiter(b, arb);
	}
	
	if(sleep){
		// Bodies should be held active if connected by a joint to a kinematic.
		phy_array *constraints = space->constraints;
		for(int i=0; i<constraints->num; i++){
			phy_constraint *constraint = (phy_constraint *)constraints->arr[i];
			phy_body *a = constraint->a, *b = constraint->b;
			
			if(phy_body_get_type(b) == PHY_BODY_TYPE_KINEMATIC) phy_body_activate(a);
			if(phy_body_get_type(a) == PHY_BODY_TYPE_KINEMATIC) phy_body_activate(b);
		}
		
		// Generate components and deactivate sleeping ones
		for(int i=0; i<bodies->num;){
			phy_body *body = (phy_body *)bodies->arr[i];
			
			if(ComponentRoot(body) == NULL){
				// Body not in a component yet. Perform a DFS to flood fill mark 
				// the component in the contact graph using this body as the root.
				FloodFillComponent(body, body);
				
				// Check if the component should be put to sleep.
				if(!ComponentActive(body, space->sleep_time_threshold)){
					phy_array_push(space->sleeping_components, body);
					PHY_BODY_FOREACH_COMPONENT(body, other) cpSpaceDeactivateBody(space, other);
					
					// cpSpaceDeactivateBody() removed the current body from the list.
					// Skip incrementing the index counter.
					continue;
				}
			}
			
			i++;
			
			// Only sleeping bodies retain their component node pointers.
			body->sleeping.root = NULL;
			body->sleeping.next = NULL;
		}
	}
}

void
phy_body_sleep(phy_body *body)
{
	phy_body_sleep_with_group(body, NULL);
}

void
phy_body_sleep_with_group(phy_body *body, phy_body *group){
	if (phy_body_get_type(body) != PHY_BODY_TYPE_DYNAMIC) {
    utl_error_func("Non-dynamic bodies cannot be put to sleep", utl_user_defined_data);
  }
	
	phy_space *space = body->space;
	if (phy_space_is_locked(space)) {
    utl_error_func("Bodies cannot be put to sleep during a query or a call step, put these calls into a post-step callback", utl_user_defined_data);
  }
	if (phy_space_get_sleep_time_threshold(space) >= INFINITY) {
    utl_error_func("Sleeping is not enabled on the space, you cannot sleep a body without setting a sleep time threshold on the space", utl_user_defined_data);
  }
	if (!(group == NULL || phy_body_is_sleeping(group))) {
    utl_error_func("Cannot use a non-sleeping body as a group identifier", utl_user_defined_data);
  }
	
	if(phy_body_is_sleeping(body)){
		if (ComponentRoot(body) != ComponentRoot(group)) {
      utl_error_func("The body is already sleeping and it's group cannot be reassigned", utl_user_defined_data);
    }
		return;
	}
	
	PHY_BODY_FOREACH_SHAPE(body, shape) phy_shape_cache_BB(shape);
	cpSpaceDeactivateBody(space, body);
	
	if(group){
		phy_body *root = ComponentRoot(group);
		
		body->sleeping.root = root;
		body->sleeping.next = root->sleeping.next;
		body->sleeping.idle_time = 0.0f;
		
		root->sleeping.next = body;
	} else {
		body->sleeping.root = body;
		body->sleeping.next = NULL;
		body->sleeping.idle_time = 0.0f;
		
		phy_array_push(space->sleeping_components, body);
	}
	
	phy_array_delete_obj(space->dynamic_bodies, body);
}
