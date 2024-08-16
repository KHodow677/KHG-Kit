/* Copyright (c) 2013 Scott Lembcke and Howling Moon Software
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

#include "khg_phy/phy_private.h"

// TODO: make this generic so I can reuse it for constraints also.
static inline void
unthreadHelper(phy_arbiter *arb, phy_body *body)
{
	struct cpArbiterThread *thread = cpArbiterThreadForBody(arb, body);
	phy_arbiter *prev = thread->prev;
	phy_arbiter *next = thread->next;
	
	if(prev){
		cpArbiterThreadForBody(prev, body)->next = next;
	} else if(body->arbiterList == arb) {
		// IFF prev is NULL and body->arbiterList == arb, is arb at the head of the list.
		// This function may be called for an arbiter that was never in a list.
		// In that case, we need to protect it from wiping out the body->arbiterList pointer.
		body->arbiterList = next;
	}
	
	if(next) cpArbiterThreadForBody(next, body)->prev = prev;
	
	thread->prev = NULL;
	thread->next = NULL;
}

void
cpArbiterUnthread(phy_arbiter *arb)
{
	unthreadHelper(arb, arb->body_a);
	unthreadHelper(arb, arb->body_b);
}

bool cpArbiterIsFirstContact(const phy_arbiter *arb)
{
	return arb->state == CP_ARBITER_STATE_FIRST_COLLISION;
}

bool cpArbiterIsRemoval(const phy_arbiter *arb)
{
	return arb->state == CP_ARBITER_STATE_INVALIDATED;
}

int cpArbiterGetCount(const phy_arbiter *arb)
{
	// Return 0 contacts if we are in a separate callback.
	return (arb->state < CP_ARBITER_STATE_CACHED ? arb->count : 0);
}

phy_vect
cpArbiterGetNormal(const phy_arbiter *arb)
{
	return cpvmult(arb->n, arb->swapped ? -1.0f : 1.0);
}

phy_vect
cpArbiterGetPointA(const phy_arbiter *arb, int i)
{
	cpAssertHard(0 <= i && i < cpArbiterGetCount(arb), "Index error: The specified contact index is invalid for this arbiter");
	return cpvadd(arb->body_a->p, arb->contacts[i].r1);
}

phy_vect
cpArbiterGetPointB(const phy_arbiter *arb, int i)
{
	cpAssertHard(0 <= i && i < cpArbiterGetCount(arb), "Index error: The specified contact index is invalid for this arbiter");
	return cpvadd(arb->body_b->p, arb->contacts[i].r2);
}

float
cpArbiterGetDepth(const phy_arbiter *arb, int i)
{
	cpAssertHard(0 <= i && i < cpArbiterGetCount(arb), "Index error: The specified contact index is invalid for this arbiter");
	
	struct cpContact *con = &arb->contacts[i];
	return cpvdot(cpvadd(cpvsub(con->r2, con->r1), cpvsub(arb->body_b->p, arb->body_a->p)), arb->n);
}

phy_contact_point_set
cpArbiterGetContactPointSet(const phy_arbiter *arb)
{
	phy_contact_point_set set;
	set.count = cpArbiterGetCount(arb);
	
	bool swapped = arb->swapped;
	phy_vect n = arb->n;
	set.normal = (swapped ? cpvneg(n) : n);
	
	for(int i=0; i<set.count; i++){
		// Contact points are relative to body CoGs;
		phy_vect p1 = cpvadd(arb->body_a->p, arb->contacts[i].r1);
		phy_vect p2 = cpvadd(arb->body_b->p, arb->contacts[i].r2);
		
		set.points[i].pointA = (swapped ? p2 : p1);
		set.points[i].pointB = (swapped ? p1 : p2);
		set.points[i].distance = cpvdot(cpvsub(p2, p1), n);
	}
	
	return set;
}

void
cpArbiterSetContactPointSet(phy_arbiter *arb, phy_contact_point_set *set)
{
	int count = set->count;
	cpAssertHard(count == arb->count, "The number of contact points cannot be changed.");
	
	bool swapped = arb->swapped;
	arb->n = (swapped ? cpvneg(set->normal) : set->normal);
	
	for(int i=0; i<count; i++){
		// Convert back to CoG relative offsets.
		phy_vect p1 = set->points[i].pointA;
		phy_vect p2 = set->points[i].pointB;
		
		arb->contacts[i].r1 = cpvsub(swapped ? p2 : p1, arb->body_a->p);
		arb->contacts[i].r2 = cpvsub(swapped ? p1 : p2, arb->body_b->p);
	}
}

phy_vect
phy_arbiter_total_impulse(const phy_arbiter *arb)
{
	struct cpContact *contacts = arb->contacts;
	phy_vect n = arb->n;
	phy_vect sum = cpvzero;
	
	for(int i=0, count=cpArbiterGetCount(arb); i<count; i++){
		struct cpContact *con = &contacts[i];
		sum = cpvadd(sum, cpvrotate(n, cpv(con->jnAcc, con->jtAcc)));
	}
		
	return (arb->swapped ? sum : cpvneg(sum));
	return cpvzero;
}

float
phy_arbiter_total_ke(const phy_arbiter *arb)
{
	float eCoef = (1 - arb->e)/(1 + arb->e);
	float sum = 0.0;
	
	struct cpContact *contacts = arb->contacts;
	for(int i=0, count=cpArbiterGetCount(arb); i<count; i++){
		struct cpContact *con = &contacts[i];
		float jnAcc = con->jnAcc;
		float jtAcc = con->jtAcc;
		
		sum += eCoef*jnAcc*jnAcc/con->nMass + jtAcc*jtAcc/con->tMass;
	}
	
	return sum;
}

bool
phy_arbiter_ignore(phy_arbiter *arb)
{
	arb->state = CP_ARBITER_STATE_IGNORE;
	return false;
}

float
phy_arbiter_get_restitution(const phy_arbiter *arb)
{
	return arb->e;
}

void
phy_arbiter_set_restitution(phy_arbiter *arb, float restitution)
{
	arb->e = restitution;
}

float
phy_arbiter_get_friction(const phy_arbiter *arb)
{
	return arb->u;
}

void
phy_arbiter_set_friction(phy_arbiter *arb, float friction)
{
	arb->u = friction;
}

phy_vect
phy_arbiter_get_surface_velocity(phy_arbiter *arb)
{
	return cpvmult(arb->surface_vr, arb->swapped ? -1.0f : 1.0);
}

void
phy_arbiter_set_surface_velocity(phy_arbiter *arb, phy_vect vr)
{
	arb->surface_vr = cpvmult(vr, arb->swapped ? -1.0f : 1.0);
}

phy_data_pointer
phy_arbiter_get_user_data(const phy_arbiter *arb)
{
	return arb->data;
}

void
phy_arbiter_set_user_data(phy_arbiter *arb, phy_data_pointer userData)
{
	arb->data = userData;
}

void
phy_arbiter_get_shapes(const phy_arbiter *arb, phy_shape **a, phy_shape **b)
{
	if(arb->swapped){
		(*a) = (phy_shape *)arb->b;
		(*b) = (phy_shape *)arb->a;
	} else {
		(*a) = (phy_shape *)arb->a;
		(*b) = (phy_shape *)arb->b;
	}
}

void phy_arbiter_get_bodies(const phy_arbiter *arb, phy_body **a, phy_body **b)
{
	PHY_ARBITER_GET_SHAPES(arb, shape_a, shape_b);
	(*a) = shape_a->body;
	(*b) = shape_b->body;
}

bool
cpArbiterCallWildcardBeginA(phy_arbiter *arb, phy_space *space)
{
	phy_collision_handler *handler = arb->handlerA;
	return handler->beginFunc(arb, space, handler->userData);
}

bool
cpArbiterCallWildcardBeginB(phy_arbiter *arb, phy_space *space)
{
	phy_collision_handler *handler = arb->handlerB;
	arb->swapped = !arb->swapped;
	bool retval = handler->beginFunc(arb, space, handler->userData);
	arb->swapped = !arb->swapped;
	return retval;
}

bool
cpArbiterCallWildcardPreSolveA(phy_arbiter *arb, phy_space *space)
{
	phy_collision_handler *handler = arb->handlerA;
	return handler->preSolveFunc(arb, space, handler->userData);
}

bool
cpArbiterCallWildcardPreSolveB(phy_arbiter *arb, phy_space *space)
{
	phy_collision_handler *handler = arb->handlerB;
	arb->swapped = !arb->swapped;
	bool retval = handler->preSolveFunc(arb, space, handler->userData);
	arb->swapped = !arb->swapped;
	return retval;
}

void
cpArbiterCallWildcardPostSolveA(phy_arbiter *arb, phy_space *space)
{
	phy_collision_handler *handler = arb->handlerA;
	handler->postSolveFunc(arb, space, handler->userData);
}

void
cpArbiterCallWildcardPostSolveB(phy_arbiter *arb, phy_space *space)
{
	phy_collision_handler *handler = arb->handlerB;
	arb->swapped = !arb->swapped;
	handler->postSolveFunc(arb, space, handler->userData);
	arb->swapped = !arb->swapped;
}

void
cpArbiterCallWildcardSeparateA(phy_arbiter *arb, phy_space *space)
{
	phy_collision_handler *handler = arb->handlerA;
	handler->separateFunc(arb, space, handler->userData);
}

void
cpArbiterCallWildcardSeparateB(phy_arbiter *arb, phy_space *space)
{
	phy_collision_handler *handler = arb->handlerB;
	arb->swapped = !arb->swapped;
	handler->separateFunc(arb, space, handler->userData);
	arb->swapped = !arb->swapped;
}

phy_arbiter*
cpArbiterInit(phy_arbiter *arb, phy_shape *a, phy_shape *b)
{
	arb->handler = NULL;
	arb->swapped = false;
	
	arb->handler = NULL;
	arb->handlerA = NULL;
	arb->handlerB = NULL;
	
	arb->e = 0.0f;
	arb->u = 0.0f;
	arb->surface_vr = cpvzero;
	
	arb->count = 0;
	arb->contacts = NULL;
	
	arb->a = a; arb->body_a = a->body;
	arb->b = b; arb->body_b = b->body;
	
	arb->thread_a.next = NULL;
	arb->thread_b.next = NULL;
	arb->thread_a.prev = NULL;
	arb->thread_b.prev = NULL;
	
	arb->stamp = 0;
	arb->state = CP_ARBITER_STATE_FIRST_COLLISION;
	
	arb->data = NULL;
	
	return arb;
}

static inline phy_collision_handler *
cpSpaceLookupHandler(phy_space *space, phy_collision_type a, phy_collision_type b, phy_collision_handler *defaultValue)
{
	phy_collision_type types[] = {a, b};
	phy_collision_handler *handler = (phy_collision_handler *)cpHashSetFind(space->collisionHandlers, CP_HASH_PAIR(a, b), types);
	return (handler ? handler : defaultValue);
}

void
cpArbiterUpdate(phy_arbiter *arb, struct cpCollisionInfo *info, phy_space *space)
{
	const phy_shape *a = info->a, *b = info->b;
	
	// For collisions between two similar primitive types, the order could have been swapped since the last frame.
	arb->a = a; arb->body_a = a->body;
	arb->b = b; arb->body_b = b->body;
	
	// Iterate over the possible pairs to look for hash value matches.
	for(int i=0; i<info->count; i++){
		struct cpContact *con = &info->arr[i];
		
		// r1 and r2 store absolute offsets at init time.
		// Need to convert them to relative offsets.
		con->r1 = cpvsub(con->r1, a->body->p);
		con->r2 = cpvsub(con->r2, b->body->p);
		
		// Cached impulses are not zeroed at init time.
		con->jnAcc = con->jtAcc = 0.0f;
		
		for(int j=0; j<arb->count; j++){
			struct cpContact *old = &arb->contacts[j];
			
			// This could trigger false positives, but is fairly unlikely nor serious if it does.
			if(con->hash == old->hash){
				// Copy the persistant contact information.
				con->jnAcc = old->jnAcc;
				con->jtAcc = old->jtAcc;
			}
		}
	}
	
	arb->contacts = info->arr;
	arb->count = info->count;
	arb->n = info->n;
	
	arb->e = a->e * b->e;
	arb->u = a->u * b->u;
	
	phy_vect surface_vr = cpvsub(b->surfaceV, a->surfaceV);
	arb->surface_vr = cpvsub(surface_vr, cpvmult(info->n, cpvdot(surface_vr, info->n)));
	
	phy_collision_type typeA = info->a->type, typeB = info->b->type;
	phy_collision_handler *defaultHandler = &space->defaultHandler;
	phy_collision_handler *handler = arb->handler = cpSpaceLookupHandler(space, typeA, typeB, defaultHandler);
	
	// Check if the types match, but don't swap for a default handler which use the wildcard for type A.
	bool swapped = arb->swapped = (typeA != handler->typeA && handler->typeA != PHY_WILDCARD_COLLISION_TYPE);
	
	if(handler != defaultHandler || space->usesWildcards){
		// The order of the main handler swaps the wildcard handlers too. Uffda.
		arb->handlerA = cpSpaceLookupHandler(space, (swapped ? typeB : typeA), PHY_WILDCARD_COLLISION_TYPE, &cpCollisionHandlerDoNothing);
		arb->handlerB = cpSpaceLookupHandler(space, (swapped ? typeA : typeB), PHY_WILDCARD_COLLISION_TYPE, &cpCollisionHandlerDoNothing);
	}
		
	// mark it as new if it's been cached
	if(arb->state == CP_ARBITER_STATE_CACHED) arb->state = CP_ARBITER_STATE_FIRST_COLLISION;
}

void
cpArbiterPreStep(phy_arbiter *arb, float dt, float slop, float bias)
{
	phy_body *a = arb->body_a;
	phy_body *b = arb->body_b;
	phy_vect n = arb->n;
	phy_vect body_delta = cpvsub(b->p, a->p);
	
	for(int i=0; i<arb->count; i++){
		struct cpContact *con = &arb->contacts[i];
		
		// Calculate the mass normal and mass tangent.
		con->nMass = 1.0f/k_scalar(a, b, con->r1, con->r2, n);
		con->tMass = 1.0f/k_scalar(a, b, con->r1, con->r2, cpvperp(n));
				
		// Calculate the target bias velocity.
		float dist = cpvdot(cpvadd(cpvsub(con->r2, con->r1), body_delta), n);
		con->bias = -bias*phy_min(0.0f, dist + slop)/dt;
		con->jBias = 0.0f;
		
		// Calculate the target bounce velocity.
		con->bounce = normal_relative_velocity(a, b, con->r1, con->r2, n)*arb->e;
	}
}

void
cpArbiterApplyCachedImpulse(phy_arbiter *arb, float dt_coef)
{
	if(cpArbiterIsFirstContact(arb)) return;
	
	phy_body *a = arb->body_a;
	phy_body *b = arb->body_b;
	phy_vect n = arb->n;
	
	for(int i=0; i<arb->count; i++){
		struct cpContact *con = &arb->contacts[i];
		phy_vect j = cpvrotate(n, cpv(con->jnAcc, con->jtAcc));
		apply_impulses(a, b, con->r1, con->r2, cpvmult(j, dt_coef));
	}
}

// TODO: is it worth splitting velocity/position correction?

void
cpArbiterApplyImpulse(phy_arbiter *arb)
{
	phy_body *a = arb->body_a;
	phy_body *b = arb->body_b;
	phy_vect n = arb->n;
	phy_vect surface_vr = arb->surface_vr;
	float friction = arb->u;

	for(int i=0; i<arb->count; i++){
		struct cpContact *con = &arb->contacts[i];
		float nMass = con->nMass;
		phy_vect r1 = con->r1;
		phy_vect r2 = con->r2;
		
		phy_vect vb1 = cpvadd(a->v_bias, cpvmult(cpvperp(r1), a->w_bias));
		phy_vect vb2 = cpvadd(b->v_bias, cpvmult(cpvperp(r2), b->w_bias));
		phy_vect vr = cpvadd(relative_velocity(a, b, r1, r2), surface_vr);
		
		float vbn = cpvdot(cpvsub(vb2, vb1), n);
		float vrn = cpvdot(vr, n);
		float vrt = cpvdot(vr, cpvperp(n));
		
		float jbn = (con->bias - vbn)*nMass;
		float jbnOld = con->jBias;
		con->jBias = phy_max(jbnOld + jbn, 0.0f);
		
		float jn = -(con->bounce + vrn)*nMass;
		float jnOld = con->jnAcc;
		con->jnAcc = phy_max(jnOld + jn, 0.0f);
		
		float jtMax = friction*con->jnAcc;
		float jt = -vrt*con->tMass;
		float jtOld = con->jtAcc;
		con->jtAcc = phy_clamp(jtOld + jt, -jtMax, jtMax);
		
		apply_bias_impulses(a, b, r1, r2, cpvmult(n, con->jBias - jbnOld));
		apply_impulses(a, b, r1, r2, cpvrotate(n, cpv(con->jnAcc - jnOld, con->jtAcc - jtOld)));
	}
}
