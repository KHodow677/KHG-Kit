/* Copyright (c) 2007 Scott Lembcke
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
 
#include <string.h>

#include "khg_phy/phy_private.h"

//MARK: Sleeping Functions

void
cpSpaceActivateBody(phy_space *space, phy_body *body)
{
	cpAssertHard(phy_body_get_type(body) == CP_BODY_TYPE_DYNAMIC, "Internal error: Attempting to activate a non-dynamic body.");
		
	if(space->locked){
		// cpSpaceActivateBody() is called again once the space is unlocked
		if(!cpArrayContains(space->rousedBodies, body)) cpArrayPush(space->rousedBodies, body);
	} else {
		cpAssertSoft(body->sleeping.root == NULL && body->sleeping.next == NULL, "Internal error: Activating body non-NULL node pointers.");
		cpArrayPush(space->dynamicBodies, body);

		CP_BODY_FOREACH_SHAPE(body, shape){
			cpSpatialIndexRemove(space->staticShapes, shape, shape->hashid);
			cpSpatialIndexInsert(space->dynamicShapes, shape, shape->hashid);
		}
		
		CP_BODY_FOREACH_ARBITER(body, arb){
			phy_body *bodyA = arb->body_a;
			
			// Arbiters are shared between two bodies that are always woken up together.
			// You only want to restore the arbiter once, so bodyA is arbitrarily chosen to own the arbiter.
			// The edge case is when static bodies are involved as the static bodies never actually sleep.
			// If the static body is bodyB then all is good. If the static body is bodyA, that can easily be checked.
			if(body == bodyA || phy_body_get_type(bodyA) == CP_BODY_TYPE_STATIC){
				int numContacts = arb->count;
				struct cpContact *contacts = arb->contacts;
				
				// Restore contact values back to the space's contact buffer memory
				arb->contacts = cpContactBufferGetArray(space);
				memcpy(arb->contacts, contacts, numContacts*sizeof(struct cpContact));
				cpSpacePushContacts(space, numContacts);
				
				// Reinsert the arbiter into the arbiter cache
				const phy_shape *a = arb->a, *b = arb->b;
				const phy_shape *shape_pair[] = {a, b};
				phy_hash_value arbHashID = CP_HASH_PAIR((phy_hash_value)a, (phy_hash_value)b);
				cpHashSetInsert(space->cachedArbiters, arbHashID, shape_pair, NULL, arb);
				
				// Update the arbiter's state
				arb->stamp = space->stamp;
				cpArrayPush(space->arbiters, arb);
				
				free(contacts);
			}
		}
		
		CP_BODY_FOREACH_CONSTRAINT(body, constraint){
			phy_body *bodyA = constraint->a;
			if(body == bodyA || phy_body_get_type(bodyA) == CP_BODY_TYPE_STATIC) cpArrayPush(space->constraints, constraint);
		}
	}
}

static void
cpSpaceDeactivateBody(phy_space *space, phy_body *body)
{
	cpAssertHard(phy_body_get_type(body) == CP_BODY_TYPE_DYNAMIC, "Internal error: Attempting to deactivate a non-dynamic body.");
	
	cpArrayDeleteObj(space->dynamicBodies, body);
	
	CP_BODY_FOREACH_SHAPE(body, shape){
		cpSpatialIndexRemove(space->dynamicShapes, shape, shape->hashid);
		cpSpatialIndexInsert(space->staticShapes, shape, shape->hashid);
	}
	
	CP_BODY_FOREACH_ARBITER(body, arb){
		phy_body *bodyA = arb->body_a;
		if(body == bodyA || phy_body_get_type(bodyA) == CP_BODY_TYPE_STATIC){
			cpSpaceUncacheArbiter(space, arb);
			
			// Save contact values to a new block of memory so they won't time out
			size_t bytes = arb->count*sizeof(struct cpContact);
			struct cpContact *contacts = (struct cpContact *)calloc(1, bytes);
			memcpy(contacts, arb->contacts, bytes);
			arb->contacts = contacts;
		}
	}
		
	CP_BODY_FOREACH_CONSTRAINT(body, constraint){
		phy_body *bodyA = constraint->a;
		if(body == bodyA || phy_body_get_type(bodyA) == CP_BODY_TYPE_STATIC) cpArrayDeleteObj(space->constraints, constraint);
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
	if(body != NULL && phy_body_get_type(body) == CP_BODY_TYPE_DYNAMIC){
		body->sleeping.idleTime = 0.0f;
		
		phy_body *root = ComponentRoot(body);
		if(root && phy_body_is_sleeping(root)){
			// TODO should cpBodyIsSleeping(root) be an assertion?
			cpAssertSoft(phy_body_get_type(root) == CP_BODY_TYPE_DYNAMIC, "Internal Error: Non-dynamic body component root detected.");
			
			phy_space *space = root->space;
			phy_body *body = root;
			while(body){
				phy_body *next = body->sleeping.next;
				
				body->sleeping.idleTime = 0.0f;
				body->sleeping.root = NULL;
				body->sleeping.next = NULL;
				cpSpaceActivateBody(space, body);
				
				body = next;
			}
			
			cpArrayDeleteObj(space->sleepingComponents, root);
		}
		
		CP_BODY_FOREACH_ARBITER(body, arb){
			// Reset the idle timer of things the body is touching as well.
			// That way things don't get left hanging in the air.
			phy_body *other = (arb->body_a == body ? arb->body_b : arb->body_a);
			if(phy_body_get_type(other) != CP_BODY_TYPE_STATIC) other->sleeping.idleTime = 0.0f;
		}
	}
}

void
phy_body_activate_static(phy_body *body, phy_shape *filter)
{
	cpAssertHard(phy_body_get_type(body) == CP_BODY_TYPE_STATIC, "cpBodyActivateStatic() called on a non-static body.");
	
	CP_BODY_FOREACH_ARBITER(body, arb){
		if(!filter || filter == arb->a || filter == arb->b){
			phy_body_activate(arb->body_a == body ? arb->body_b : arb->body_a);
		}
	}
	
	// TODO: should also activate joints?
}

static inline void
cpBodyPushArbiter(phy_body *body, phy_arbiter *arb)
{
	cpAssertSoft(cpArbiterThreadForBody(arb, body)->next == NULL, "Internal Error: Dangling contact graph pointers detected. (A)");
	cpAssertSoft(cpArbiterThreadForBody(arb, body)->prev == NULL, "Internal Error: Dangling contact graph pointers detected. (B)");
	
	phy_arbiter *next = body->arbiterList;
	cpAssertSoft(next == NULL || cpArbiterThreadForBody(next, body)->prev == NULL, "Internal Error: Dangling contact graph pointers detected. (C)");
	cpArbiterThreadForBody(arb, body)->next = next;
	
	if(next) cpArbiterThreadForBody(next, body)->prev = arb;
	body->arbiterList = arb;
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
	if(phy_body_get_type(body) == CP_BODY_TYPE_DYNAMIC){
		phy_body *other_root = ComponentRoot(body);
		if(other_root == NULL){
			ComponentAdd(root, body);
			CP_BODY_FOREACH_ARBITER(body, arb) FloodFillComponent(root, (body == arb->body_a ? arb->body_b : arb->body_a));
			CP_BODY_FOREACH_CONSTRAINT(body, constraint) FloodFillComponent(root, (body == constraint->a ? constraint->b : constraint->a));
		} else {
			cpAssertSoft(other_root == root, "Internal Error: Inconsistency dectected in the contact graph.");
		}
	}
}

static inline bool
ComponentActive(phy_body *root, float threshold)
{
	CP_BODY_FOREACH_COMPONENT(root, body){
		if(body->sleeping.idleTime < threshold) return true;
	}
	
	return false;
}

void
cpSpaceProcessComponents(phy_space *space, float dt)
{
	bool sleep = (space->sleepTimeThreshold != INFINITY);
	phy_array *bodies = space->dynamicBodies;
	
#ifndef NDEBUG
	for(int i=0; i<bodies->num; i++){
		phy_body *body = (phy_body *)bodies->arr[i];
		
		cpAssertSoft(body->sleeping.next == NULL, "Internal Error: Dangling next pointer detected in contact graph.");
		cpAssertSoft(body->sleeping.root == NULL, "Internal Error: Dangling root pointer detected in contact graph.");
	}
#endif
	
	// Calculate the kinetic energy of all the bodies.
	if(sleep){
		float dv = space->idleSpeedThreshold;
		float dvsq = (dv ? dv*dv : cpvlengthsq(space->gravity)*dt*dt);
		
		// update idling and reset component nodes
		for(int i=0; i<bodies->num; i++){
			phy_body *body = (phy_body *)bodies->arr[i];
			
			// TODO should make a separate array for kinematic bodies.
			if(phy_body_get_type(body) != CP_BODY_TYPE_DYNAMIC) continue;
			
			// Need to deal with infinite mass objects
			float keThreshold = (dvsq ? body->m*dvsq : 0.0f);
			body->sleeping.idleTime = (cpBodyKineticEnergy(body) > keThreshold ? 0.0f : body->sleeping.idleTime + dt);
		}
	}
	
	// Awaken any sleeping bodies found and then push arbiters to the bodies' lists.
	phy_array *arbiters = space->arbiters;
	for(int i=0, count=arbiters->num; i<count; i++){
		phy_arbiter *arb = (phy_arbiter*)arbiters->arr[i];
		phy_body *a = arb->body_a, *b = arb->body_b;
		
		if(sleep){
			// TODO checking cpBodyIsSleepin() redundant?
			if(phy_body_get_type(b) == CP_BODY_TYPE_KINEMATIC || phy_body_is_sleeping(a)) phy_body_activate(a);
			if(phy_body_get_type(a) == CP_BODY_TYPE_KINEMATIC || phy_body_is_sleeping(b)) phy_body_activate(b);
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
			
			if(phy_body_get_type(b) == CP_BODY_TYPE_KINEMATIC) phy_body_activate(a);
			if(phy_body_get_type(a) == CP_BODY_TYPE_KINEMATIC) phy_body_activate(b);
		}
		
		// Generate components and deactivate sleeping ones
		for(int i=0; i<bodies->num;){
			phy_body *body = (phy_body *)bodies->arr[i];
			
			if(ComponentRoot(body) == NULL){
				// Body not in a component yet. Perform a DFS to flood fill mark 
				// the component in the contact graph using this body as the root.
				FloodFillComponent(body, body);
				
				// Check if the component should be put to sleep.
				if(!ComponentActive(body, space->sleepTimeThreshold)){
					cpArrayPush(space->sleepingComponents, body);
					CP_BODY_FOREACH_COMPONENT(body, other) cpSpaceDeactivateBody(space, other);
					
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
	cpAssertHard(phy_body_get_type(body) == CP_BODY_TYPE_DYNAMIC, "Non-dynamic bodies cannot be put to sleep.");
	
	phy_space *space = body->space;
	cpAssertHard(!cpSpaceIsLocked(space), "Bodies cannot be put to sleep during a query or a call to cpSpaceStep(). Put these calls into a post-step callback.");
	cpAssertHard(cpSpaceGetSleepTimeThreshold(space) < INFINITY, "Sleeping is not enabled on the space. You cannot sleep a body without setting a sleep time threshold on the space.");
	cpAssertHard(group == NULL || phy_body_is_sleeping(group), "Cannot use a non-sleeping body as a group identifier.");
	
	if(phy_body_is_sleeping(body)){
		cpAssertHard(ComponentRoot(body) == ComponentRoot(group), "The body is already sleeping and it's group cannot be reassigned.");
		return;
	}
	
	CP_BODY_FOREACH_SHAPE(body, shape) cpShapeCacheBB(shape);
	cpSpaceDeactivateBody(space, body);
	
	if(group){
		phy_body *root = ComponentRoot(group);
		
		body->sleeping.root = root;
		body->sleeping.next = root->sleeping.next;
		body->sleeping.idleTime = 0.0f;
		
		root->sleeping.next = body;
	} else {
		body->sleeping.root = body;
		body->sleeping.next = NULL;
		body->sleeping.idleTime = 0.0f;
		
		cpArrayPush(space->sleepingComponents, body);
	}
	
	cpArrayDeleteObj(space->dynamicBodies, body);
}
