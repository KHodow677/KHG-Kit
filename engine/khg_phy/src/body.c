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

#include <float.h>
#include <stdarg.h>

#include "khg_phy/phy_private.h"

phy_body*
cpBodyAlloc(void)
{
	return (phy_body *)calloc(1, sizeof(phy_body));
}

phy_body *
cpBodyInit(phy_body *body, float mass, float moment)
{
	body->space = NULL;
	body->shapeList = NULL;
	body->arbiterList = NULL;
	body->constraintList = NULL;
	
	body->velocity_func = cpBodyUpdateVelocity;
	body->position_func = cpBodyUpdatePosition;
	
	body->sleeping.root = NULL;
	body->sleeping.next = NULL;
	body->sleeping.idleTime = 0.0f;
	
	body->p = cpvzero;
	body->v = cpvzero;
	body->f = cpvzero;
	
	body->w = 0.0f;
	body->t = 0.0f;
	
	body->v_bias = cpvzero;
	body->w_bias = 0.0f;
	
	body->userData = NULL;
	
	// Setters must be called after full initialization so the sanity checks don't assert on garbage data.
	cpBodySetMass(body, mass);
	cpBodySetMoment(body, moment);
	cpBodySetAngle(body, 0.0f);
	
	return body;
}

phy_body*
cpBodyNew(float mass, float moment)
{
	return cpBodyInit(cpBodyAlloc(), mass, moment);
}

phy_body*
cpBodyNewKinematic()
{
	phy_body *body = cpBodyNew(0.0f, 0.0f);
	cpBodySetType(body, CP_BODY_TYPE_KINEMATIC);
	
	return body;
}

phy_body*
cpBodyNewStatic()
{
	phy_body *body = cpBodyNew(0.0f, 0.0f);
	cpBodySetType(body, CP_BODY_TYPE_STATIC);
	
	return body;
}

void cpBodyDestroy(phy_body *body){}

void
cpBodyFree(phy_body *body)
{
	if(body){
		cpBodyDestroy(body);
		free(body);
	}
}

#ifdef NDEBUG
	#define	cpAssertSaneBody(body)
#else
	static void cpv_assert_nan(phy_vect v, const char *message){cpAssertHard(v.x == v.x && v.y == v.y, message);}
	static void cpv_assert_infinite(phy_vect v, const char *message){cpAssertHard(phy_abs(v.x) != INFINITY && phy_abs(v.y) != INFINITY, message);}
	static void cpv_assert_sane(phy_vect v, const char *message){cpv_assert_nan(v, message); cpv_assert_infinite(v, message);}
	
	static void
	cpBodySanityCheck(const phy_body *body)
	{
		cpAssertHard(body->m == body->m && body->m_inv == body->m_inv, "Body's mass is NaN.");
		cpAssertHard(body->i == body->i && body->i_inv == body->i_inv, "Body's moment is NaN.");
		cpAssertHard(body->m >= 0.0f, "Body's mass is negative.");
		cpAssertHard(body->i >= 0.0f, "Body's moment is negative.");
		
		cpv_assert_sane(body->p, "Body's position is invalid.");
		cpv_assert_sane(body->v, "Body's velocity is invalid.");
		cpv_assert_sane(body->f, "Body's force is invalid.");

		cpAssertHard(body->a == body->a && phy_abs(body->a) != INFINITY, "Body's angle is invalid.");
		cpAssertHard(body->w == body->w && phy_abs(body->w) != INFINITY, "Body's angular velocity is invalid.");
		cpAssertHard(body->t == body->t && phy_abs(body->t) != INFINITY, "Body's torque is invalid.");
	}
	
	#define	cpAssertSaneBody(body) cpBodySanityCheck(body)
#endif

bool
cpBodyIsSleeping(const phy_body *body)
{
	return (body->sleeping.root != ((phy_body*)0));
}

cpBodyType
cpBodyGetType(phy_body *body)
{
	if(body->sleeping.idleTime == INFINITY){
		return CP_BODY_TYPE_STATIC;
	} else if(body->m == INFINITY){
		return CP_BODY_TYPE_KINEMATIC;
	} else {
		return CP_BODY_TYPE_DYNAMIC;
	}
}

void
cpBodySetType(phy_body *body, cpBodyType type)
{
	cpBodyType oldType = cpBodyGetType(body);
	if(oldType == type) return;
	
	// Static bodies have their idle timers set to infinity.
	// Non-static bodies should have their idle timer reset.
	body->sleeping.idleTime = (type == CP_BODY_TYPE_STATIC ? INFINITY : 0.0f);
	
	if(type == CP_BODY_TYPE_DYNAMIC){
		body->m = body->i = 0.0f;
		body->m_inv = body->i_inv = INFINITY;
		
		cpBodyAccumulateMassFromShapes(body);
	} else {
		body->m = body->i = INFINITY;
		body->m_inv = body->i_inv = 0.0f;
		
		body->v = cpvzero;
		body->w = 0.0f;
	}
	
	// If the body is added to a space already, we'll need to update some space data structures.
	phy_space *space = cpBodyGetSpace(body);
	if(space != NULL){
		cpAssertSpaceUnlocked(space);
		
		if(oldType == CP_BODY_TYPE_STATIC){
			// TODO This is probably not necessary
//			cpBodyActivateStatic(body, NULL);
		} else {
			cpBodyActivate(body);
		}
		
		// Move the bodies to the correct array.
		phy_array *fromArray = cpSpaceArrayForBodyType(space, oldType);
		phy_array *toArray = cpSpaceArrayForBodyType(space, type);
		if(fromArray != toArray){
			cpArrayDeleteObj(fromArray, body);
			cpArrayPush(toArray, body);
		}
		
		// Move the body's shapes to the correct spatial index.
		cpSpatialIndex *fromIndex = (oldType == CP_BODY_TYPE_STATIC ? space->staticShapes : space->dynamicShapes);
		cpSpatialIndex *toIndex = (type == CP_BODY_TYPE_STATIC ? space->staticShapes : space->dynamicShapes);
		if(fromIndex != toIndex){
			CP_BODY_FOREACH_SHAPE(body, shape){
				cpSpatialIndexRemove(fromIndex, shape, shape->hashid);
				cpSpatialIndexInsert(toIndex, shape, shape->hashid);
			}
		}
	}
}



// Should *only* be called when shapes with mass info are modified, added or removed.
void
cpBodyAccumulateMassFromShapes(phy_body *body)
{
	if(body == NULL || cpBodyGetType(body) != CP_BODY_TYPE_DYNAMIC) return;
	
	// Reset the body's mass data.
	body->m = body->i = 0.0f;
	body->cog = cpvzero;
	
	// Cache the position to realign it at the end.
	phy_vect pos = cpBodyGetPosition(body);
	
	// Accumulate mass from shapes.
	CP_BODY_FOREACH_SHAPE(body, shape){
		struct cpShapeMassInfo *info = &shape->massInfo;
		float m = info->m;
		
		if(m > 0.0f){
			float msum = body->m + m;
			
			body->i += m*info->i + cpvdistsq(body->cog, info->cog)*(m*body->m)/msum;
			body->cog = cpvlerp(body->cog, info->cog, m/msum);
			body->m = msum;
		}
	}
	
	// Recalculate the inverses.
	body->m_inv = 1.0f/body->m;
	body->i_inv = 1.0f/body->i;
	
	// Realign the body since the CoG has probably moved.
	cpBodySetPosition(body, pos);
	cpAssertSaneBody(body);
}

phy_space *
cpBodyGetSpace(const phy_body *body)
{
	return body->space;
}

float
cpBodyGetMass(const phy_body *body)
{
	return body->m;
}

void
cpBodySetMass(phy_body *body, float mass)
{
	cpAssertHard(cpBodyGetType(body) == CP_BODY_TYPE_DYNAMIC, "You cannot set the mass of kinematic or static bodies.");
	cpAssertHard(0.0f <= mass && mass < INFINITY, "Mass must be positive and finite.");
	
	cpBodyActivate(body);
	body->m = mass;
	body->m_inv = mass == 0.0f ? INFINITY : 1.0f/mass;
	cpAssertSaneBody(body);
}

float
cpBodyGetMoment(const phy_body *body)
{
	return body->i;
}

void
cpBodySetMoment(phy_body *body, float moment)
{
	cpAssertHard(moment >= 0.0f, "Moment of Inertia must be positive.");
	
	cpBodyActivate(body);
	body->i = moment;
	body->i_inv = moment == 0.0f ? INFINITY : 1.0f/moment;
	cpAssertSaneBody(body);
}

phy_vect
cpBodyGetRotation(const phy_body *body)
{
	return cpv(body->transform.a, body->transform.b);
}

void
cpBodyAddShape(phy_body *body, phy_shape *shape)
{
	phy_shape *next = body->shapeList;
	if(next) next->prev = shape;
	
	shape->next = next;
	body->shapeList = shape;
	
	if(shape->massInfo.m > 0.0f){
		cpBodyAccumulateMassFromShapes(body);
	}
}

void
cpBodyRemoveShape(phy_body *body, phy_shape *shape)
{
  phy_shape *prev = shape->prev;
  phy_shape *next = shape->next;
  
  if(prev){
		prev->next = next;
  } else {
		body->shapeList = next;
  }
  
  if(next){
		next->prev = prev;
	}
  
  shape->prev = NULL;
  shape->next = NULL;
	
	if(cpBodyGetType(body) == CP_BODY_TYPE_DYNAMIC && shape->massInfo.m > 0.0f){
		cpBodyAccumulateMassFromShapes(body);
	}
}

static phy_constraint *
filterConstraints(phy_constraint *node, phy_body *body, phy_constraint *filter)
{
	if(node == filter){
		return cpConstraintNext(node, body);
	} else if(node->a == body){
		node->next_a = filterConstraints(node->next_a, body, filter);
	} else {
		node->next_b = filterConstraints(node->next_b, body, filter);
	}
	
	return node;
}

void
cpBodyRemoveConstraint(phy_body *body, phy_constraint *constraint)
{
	body->constraintList = filterConstraints(body->constraintList, body, constraint);
}

// 'p' is the position of the CoG
static void
SetTransform(phy_body *body, phy_vect p, float a)
{
	phy_vect rot = cpvforangle(a);
	phy_vect c = body->cog;
	
	body->transform = cpTransformNewTranspose(
		rot.x, -rot.y, p.x - (c.x*rot.x - c.y*rot.y),
		rot.y,  rot.x, p.y - (c.x*rot.y + c.y*rot.x)
	);
}

static inline float
SetAngle(phy_body *body, float a)
{
	body->a = a;
	cpAssertSaneBody(body);
	
	return a;
}

phy_vect
cpBodyGetPosition(const phy_body *body)
{
	return cpTransformPoint(body->transform, cpvzero);
}

void
cpBodySetPosition(phy_body *body, phy_vect position)
{
	cpBodyActivate(body);
	phy_vect p = body->p = cpvadd(cpTransformVect(body->transform, body->cog), position);
	cpAssertSaneBody(body);
	
	SetTransform(body, p, body->a);
}

phy_vect
cpBodyGetCenterOfGravity(const phy_body *body)
{
	return body->cog;
}

void
cpBodySetCenterOfGravity(phy_body *body, phy_vect cog)
{
	cpBodyActivate(body);
	body->cog = cog;
	cpAssertSaneBody(body);
}

phy_vect
cpBodyGetVelocity(const phy_body *body)
{
	return body->v;
}

void
cpBodySetVelocity(phy_body *body, phy_vect velocity)
{
	cpBodyActivate(body);
	body->v = velocity;
	cpAssertSaneBody(body);
}

phy_vect
cpBodyGetForce(const phy_body *body)
{
	return body->f;
}

void
cpBodySetForce(phy_body *body, phy_vect force)
{
	cpBodyActivate(body);
	body->f = force;
	cpAssertSaneBody(body);
}

float
cpBodyGetAngle(const phy_body *body)
{
	return body->a;
}

void
cpBodySetAngle(phy_body *body, float angle)
{
	cpBodyActivate(body);
	SetAngle(body, angle);
	
	SetTransform(body, body->p, angle);
}

float
cpBodyGetAngularVelocity(const phy_body *body)
{
	return body->w;
}

void
cpBodySetAngularVelocity(phy_body *body, float angularVelocity)
{
	cpBodyActivate(body);
	body->w = angularVelocity;
	cpAssertSaneBody(body);
}

float
cpBodyGetTorque(const phy_body *body)
{
	return body->t;
}

void
cpBodySetTorque(phy_body *body, float torque)
{
	cpBodyActivate(body);
	body->t = torque;
	cpAssertSaneBody(body);
}

phy_data_pointer
cpBodyGetUserData(const phy_body *body)
{
	return body->userData;
}

void
cpBodySetUserData(phy_body *body, phy_data_pointer userData)
{
	body->userData = userData;
}

void
cpBodySetVelocityUpdateFunc(phy_body *body, cpBodyVelocityFunc velocityFunc)
{
	body->velocity_func = velocityFunc;
}

void
cpBodySetPositionUpdateFunc(phy_body *body, cpBodyPositionFunc positionFunc)
{
	body->position_func = positionFunc;
}

void
cpBodyUpdateVelocity(phy_body *body, phy_vect gravity, float damping, float dt)
{
	// Skip kinematic bodies.
	if(cpBodyGetType(body) == CP_BODY_TYPE_KINEMATIC) return;
	
	cpAssertSoft(body->m > 0.0f && body->i > 0.0f, "Body's mass and moment must be positive to simulate. (Mass: %f Moment: %f)", body->m, body->i);
	
	body->v = cpvadd(cpvmult(body->v, damping), cpvmult(cpvadd(gravity, cpvmult(body->f, body->m_inv)), dt));
	body->w = body->w*damping + body->t*body->i_inv*dt;
	
	// Reset forces.
	body->f = cpvzero;
	body->t = 0.0f;
	
	cpAssertSaneBody(body);
}

void
cpBodyUpdatePosition(phy_body *body, float dt)
{
	phy_vect p = body->p = cpvadd(body->p, cpvmult(cpvadd(body->v, body->v_bias), dt));
	float a = SetAngle(body, body->a + (body->w + body->w_bias)*dt);
	SetTransform(body, p, a);
	
	body->v_bias = cpvzero;
	body->w_bias = 0.0f;
	
	cpAssertSaneBody(body);
}

phy_vect
cpBodyLocalToWorld(const phy_body *body, const phy_vect point)
{
	return cpTransformPoint(body->transform, point);
}

phy_vect
cpBodyWorldToLocal(const phy_body *body, const phy_vect point)
{
	return cpTransformPoint(cpTransformRigidInverse(body->transform), point);
}

void
cpBodyApplyForceAtWorldPoint(phy_body *body, phy_vect force, phy_vect point)
{
	cpBodyActivate(body);
	body->f = cpvadd(body->f, force);
	
	phy_vect r = cpvsub(point, cpTransformPoint(body->transform, body->cog));
	body->t += cpvcross(r, force);
}

void
cpBodyApplyForceAtLocalPoint(phy_body *body, phy_vect force, phy_vect point)
{
	cpBodyApplyForceAtWorldPoint(body, cpTransformVect(body->transform, force), cpTransformPoint(body->transform, point));
}

void
cpBodyApplyImpulseAtWorldPoint(phy_body *body, phy_vect impulse, phy_vect point)
{
	cpBodyActivate(body);
	
	phy_vect r = cpvsub(point, cpTransformPoint(body->transform, body->cog));
	apply_impulse(body, impulse, r);
}

void
cpBodyApplyImpulseAtLocalPoint(phy_body *body, phy_vect impulse, phy_vect point)
{
	cpBodyApplyImpulseAtWorldPoint(body, cpTransformVect(body->transform, impulse), cpTransformPoint(body->transform, point));
}

phy_vect
cpBodyGetVelocityAtLocalPoint(const phy_body *body, phy_vect point)
{
	phy_vect r = cpTransformVect(body->transform, cpvsub(point, body->cog));
	return cpvadd(body->v, cpvmult(cpvperp(r), body->w));
}

phy_vect
cpBodyGetVelocityAtWorldPoint(const phy_body *body, phy_vect point)
{
	phy_vect r = cpvsub(point, cpTransformPoint(body->transform, body->cog));
	return cpvadd(body->v, cpvmult(cpvperp(r), body->w));
}

float
cpBodyKineticEnergy(const phy_body *body)
{
	// Need to do some fudging to avoid NaNs
	float vsq = cpvdot(body->v, body->v);
	float wsq = body->w*body->w;
	return (vsq ? vsq*body->m : 0.0f) + (wsq ? wsq*body->i : 0.0f);
}

void
cpBodyEachShape(phy_body *body, cpBodyShapeIteratorFunc func, void *data)
{
	phy_shape *shape = body->shapeList;
	while(shape){
		phy_shape *next = shape->next;
		func(body, shape, data);
		shape = next;
	}
}

void
cpBodyEachConstraint(phy_body *body, cpBodyConstraintIteratorFunc func, void *data)
{
	phy_constraint *constraint = body->constraintList;
	while(constraint){
		phy_constraint *next = cpConstraintNext(constraint, body);
		func(body, constraint, data);
		constraint = next;
	}
}

void
cpBodyEachArbiter(phy_body *body, cpBodyArbiterIteratorFunc func, void *data)
{
	phy_arbiter *arb = body->arbiterList;
	while(arb){
		phy_arbiter *next = cpArbiterNext(arb, body);
		
		bool swapped = arb->swapped; {
			arb->swapped = (body == arb->body_b);
			func(body, arb, data);
		} arb->swapped = swapped;
		
		arb = next;
	}
}
