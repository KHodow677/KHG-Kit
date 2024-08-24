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
#include "khg_utl/error_func.h"

phy_body*
phy_body_alloc(void)
{
	return (phy_body *)calloc(1, sizeof(phy_body));
}

phy_body *
phy_body_init(phy_body *body, float mass, float moment)
{
	body->space = NULL;
	body->shapeList = NULL;
	body->arbiterList = NULL;
	body->constraintList = NULL;
	
	body->velocity_func = phy_body_update_velocity;
	body->position_func = phy_body_update_position;
	
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
	phy_body_set_mass(body, mass);
	phy_body_set_moment(body, moment);
	phy_body_set_angle(body, 0.0f);
	
	return body;
}

phy_body*
phy_body_new(float mass, float moment)
{
	return phy_body_init(phy_body_alloc(), mass, moment);
}

phy_body*
phy_body_new_kinematic()
{
	phy_body *body = phy_body_new(0.0f, 0.0f);
	phy_body_set_type(body, PHY_BODY_TYPE_KINEMATIC);
	
	return body;
}

phy_body*
phy_body_new_static()
{
	phy_body *body = phy_body_new(0.0f, 0.0f);
	phy_body_set_type(body, PHY_BODY_TYPE_STATIC);
	
	return body;
}

void phy_body_destroy(phy_body *body){}

void
phy_body_free(phy_body *body)
{
	if(body){
		phy_body_destroy(body);
		free(body);
	}
}

static void
cpBodySanityCheck(const phy_body *body)
{
  if(!(body->m == body->m && body->m_inv == body->m_inv)) {
    utl_error_func("Body's mass is NaN", utl_user_defined_data);
  }
  if(body->m < 0.0f) {
    utl_error_func("Body's mass is negative", utl_user_defined_data);
  }
  if(body->i < 0.0f) {
    utl_error_func("Body's moment is negative", utl_user_defined_data);
  }
  if (!(phy_abs(body->p.x) != INFINITY && phy_abs(body->p.y) != INFINITY && body->p.x == body->p.x && body->p.y == body->p.y)) {
    utl_error_func("Body's position is invalid", utl_user_defined_data);
  }
  if (!(phy_abs(body->v.x) != INFINITY && phy_abs(body->v.y) != INFINITY && body->v.x == body->v.x && body->v.y == body->v.y)) {
    utl_error_func("Body's velocity is invalid", utl_user_defined_data);
  }
  if (!(phy_abs(body->f.x) != INFINITY && phy_abs(body->f.y) != INFINITY && body->f.x == body->f.x && body->f.y == body->f.y)) {
    utl_error_func("Body's force is invalid", utl_user_defined_data);
  }
  if(!(body->a == body->a && phy_abs(body->a) != INFINITY)) {
    utl_error_func("Body's angle is invalid", utl_user_defined_data);
  }
  if(!(body->w == body->w && phy_abs(body->w) != INFINITY)) {
    utl_error_func("Body's angular velocity is invalid", utl_user_defined_data);
  }
  if (!(body->t == body->t && phy_abs(body->t) != INFINITY)) {
    utl_error_func("Body's torque is invalid", utl_user_defined_data);
  }
}

bool
phy_body_is_sleeping(const phy_body *body)
{
	return (body->sleeping.root != ((phy_body*)0));
}

phy_body_type
phy_body_get_type(phy_body *body)
{
	if(body->sleeping.idleTime == INFINITY){
		return PHY_BODY_TYPE_STATIC;
	} else if(body->m == INFINITY){
		return PHY_BODY_TYPE_KINEMATIC;
	} else {
		return PHY_BODY_TYPE_DYNAMIC;
	}
}

void
phy_body_set_type(phy_body *body, phy_body_type type)
{
	phy_body_type oldType = phy_body_get_type(body);
	if(oldType == type) return;
	
	// Static bodies have their idle timers set to infinity.
	// Non-static bodies should have their idle timer reset.
	body->sleeping.idleTime = (type == PHY_BODY_TYPE_STATIC ? INFINITY : 0.0f);
	
	if(type == PHY_BODY_TYPE_DYNAMIC){
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
	phy_space *space = phy_body_get_space(body);
	if(space != NULL){
    if (space->locked) {
      utl_error_func("This operation cannot be done safely during a step call or during a query", utl_user_defined_data);
    }
		if(oldType == PHY_BODY_TYPE_STATIC){
			// TODO This is probably not necessary
//			cpBodyActivateStatic(body, NULL);
		} else {
			phy_body_activate(body);
		}
		
		// Move the bodies to the correct array.
		phy_array *fromArray = cpSpaceArrayForBodyType(space, oldType);
		phy_array *toArray = cpSpaceArrayForBodyType(space, type);
		if(fromArray != toArray){
			cpArrayDeleteObj(fromArray, body);
			cpArrayPush(toArray, body);
		}
		
		// Move the body's shapes to the correct spatial index.
		cpSpatialIndex *fromIndex = (oldType == PHY_BODY_TYPE_STATIC ? space->staticShapes : space->dynamicShapes);
		cpSpatialIndex *toIndex = (type == PHY_BODY_TYPE_STATIC ? space->staticShapes : space->dynamicShapes);
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
	if(body == NULL || phy_body_get_type(body) != PHY_BODY_TYPE_DYNAMIC) return;
	
	// Reset the body's mass data.
	body->m = body->i = 0.0f;
	body->cog = cpvzero;
	
	// Cache the position to realign it at the end.
	phy_vect pos = phy_body_get_position(body);
	
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
	phy_body_set_position(body, pos);
	cpBodySanityCheck(body);
}

phy_space *
phy_body_get_space(const phy_body *body)
{
	return body->space;
}

float
phy_body_get_mass(const phy_body *body)
{
	return body->m;
}

void
phy_body_set_mass(phy_body *body, float mass)
{
	if(phy_body_get_type(body) != PHY_BODY_TYPE_DYNAMIC) {
    utl_error_func("You cannot set the mass of kinematic or static bodies", utl_user_defined_data);
  }
	if(!(0.0f <= mass && mass < INFINITY)) {
    utl_error_func("Mass must be positive and finite", utl_user_defined_data);
  }
	
	phy_body_activate(body);
	body->m = mass;
	body->m_inv = mass == 0.0f ? INFINITY : 1.0f/mass;
	cpBodySanityCheck(body);
}

float
phy_body_get_moment(const phy_body *body)
{
	return body->i;
}

void
phy_body_set_moment(phy_body *body, float moment)
{
	if (moment < 0.0f) {
    utl_error_func("Moment of Inertia must be positive", utl_user_defined_data);
  }
	
	phy_body_activate(body);
	body->i = moment;
	body->i_inv = moment == 0.0f ? INFINITY : 1.0f/moment;
	cpBodySanityCheck(body);
}

phy_vect
phy_body_get_rotation(const phy_body *body)
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
	
	if(phy_body_get_type(body) == PHY_BODY_TYPE_DYNAMIC && shape->massInfo.m > 0.0f){
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
	cpBodySanityCheck(body);
	
	return a;
}

phy_vect
phy_body_get_position(const phy_body *body)
{
	return cpTransformPoint(body->transform, cpvzero);
}

void
phy_body_set_position(phy_body *body, phy_vect position)
{
	phy_body_activate(body);
	phy_vect p = body->p = cpvadd(cpTransformVect(body->transform, body->cog), position);
	cpBodySanityCheck(body);
	
	SetTransform(body, p, body->a);
}

phy_vect
phy_body_get_center_of_gravity(const phy_body *body)
{
	return body->cog;
}

void
phy_body_set_center_of_gravity(phy_body *body, phy_vect cog)
{
	phy_body_activate(body);
	body->cog = cog;
	cpBodySanityCheck(body);
}

phy_vect
phy_body_get_velocity(const phy_body *body)
{
	return body->v;
}

void
phy_body_set_velocity(phy_body *body, phy_vect velocity)
{
	phy_body_activate(body);
	body->v = velocity;
	cpBodySanityCheck(body);
}

phy_vect
phy_body_get_force(const phy_body *body)
{
	return body->f;
}

void
phy_body_set_force(phy_body *body, phy_vect force)
{
	phy_body_activate(body);
	body->f = force;
	cpBodySanityCheck(body);
}

float
phy_body_get_angle(const phy_body *body)
{
	return body->a;
}

void
phy_body_set_angle(phy_body *body, float angle)
{
	phy_body_activate(body);
	SetAngle(body, angle);
	
	SetTransform(body, body->p, angle);
}

float
phy_body_get_angular_velocity(const phy_body *body)
{
	return body->w;
}

void
phy_body_set_angular_velocity(phy_body *body, float angularVelocity)
{
	phy_body_activate(body);
	body->w = angularVelocity;
	cpBodySanityCheck(body);
}

float
phy_body_get_torque(const phy_body *body)
{
	return body->t;
}

void
phy_body_set_torque(phy_body *body, float torque)
{
	phy_body_activate(body);
	body->t = torque;
	cpBodySanityCheck(body);
}

phy_data_pointer
phy_body_get_user_data(const phy_body *body)
{
	return body->userData;
}

void
phy_body_set_user_data(phy_body *body, phy_data_pointer userData)
{
	body->userData = userData;
}

void
phy_body_set_velocity_update_func(phy_body *body, phy_body_velocity_func velocityFunc)
{
	body->velocity_func = velocityFunc;
}

void
phy_body_set_position_update_func(phy_body *body, phy_body_position_func positionFunc)
{
	body->position_func = positionFunc;
}

void
phy_body_update_velocity(phy_body *body, phy_vect gravity, float damping, float dt)
{
	// Skip kinematic bodies.
	if(phy_body_get_type(body) == PHY_BODY_TYPE_KINEMATIC) return;
	
	if (!(body->m > 0.0f && body->i > 0.0f)) {
    utl_error_func("Body's mass and moment must be positive to simulate", utl_user_defined_data);
  }
	
	body->v = cpvadd(cpvmult(body->v, damping), cpvmult(cpvadd(gravity, cpvmult(body->f, body->m_inv)), dt));
	body->w = body->w*damping + body->t*body->i_inv*dt;
	
	// Reset forces.
	body->f = cpvzero;
	body->t = 0.0f;
	
	cpBodySanityCheck(body);
}

void
phy_body_update_position(phy_body *body, float dt)
{
	phy_vect p = body->p = cpvadd(body->p, cpvmult(cpvadd(body->v, body->v_bias), dt));
	float a = SetAngle(body, body->a + (body->w + body->w_bias)*dt);
	SetTransform(body, p, a);
	
	body->v_bias = cpvzero;
	body->w_bias = 0.0f;
	
	cpBodySanityCheck(body);
}

phy_vect
phy_body_local_to_world(const phy_body *body, const phy_vect point)
{
	return cpTransformPoint(body->transform, point);
}

phy_vect
phy_body_world_to_local(const phy_body *body, const phy_vect point)
{
	return cpTransformPoint(cpTransformRigidInverse(body->transform), point);
}

void
phy_body_apply_force_at_world_point(phy_body *body, phy_vect force, phy_vect point)
{
	phy_body_activate(body);
	body->f = cpvadd(body->f, force);
	
	phy_vect r = cpvsub(point, cpTransformPoint(body->transform, body->cog));
	body->t += cpvcross(r, force);
}

void
phy_body_apply_force_at_local_point(phy_body *body, phy_vect force, phy_vect point)
{
	phy_body_apply_force_at_world_point(body, cpTransformVect(body->transform, force), cpTransformPoint(body->transform, point));
}

void
phy_body_apply_impulse_at_world_point(phy_body *body, phy_vect impulse, phy_vect point)
{
	phy_body_activate(body);
	
	phy_vect r = cpvsub(point, cpTransformPoint(body->transform, body->cog));
	apply_impulse(body, impulse, r);
}

void
phy_body_apply_impulse_at_local_point(phy_body *body, phy_vect impulse, phy_vect point)
{
	phy_body_apply_impulse_at_world_point(body, cpTransformVect(body->transform, impulse), cpTransformPoint(body->transform, point));
}

phy_vect
phy_body_get_velocity_at_local_point(const phy_body *body, phy_vect point)
{
	phy_vect r = cpTransformVect(body->transform, cpvsub(point, body->cog));
	return cpvadd(body->v, cpvmult(cpvperp(r), body->w));
}

phy_vect
phy_body_get_velocity_at_world_point(const phy_body *body, phy_vect point)
{
	phy_vect r = cpvsub(point, cpTransformPoint(body->transform, body->cog));
	return cpvadd(body->v, cpvmult(cpvperp(r), body->w));
}

float
phy_body_kinetic_energy(const phy_body *body)
{
	// Need to do some fudging to avoid NaNs
	float vsq = cpvdot(body->v, body->v);
	float wsq = body->w*body->w;
	return (vsq ? vsq*body->m : 0.0f) + (wsq ? wsq*body->i : 0.0f);
}

void
phy_body_each_shape(phy_body *body, phy_body_shape_iterator_func func, void *data)
{
	phy_shape *shape = body->shapeList;
	while(shape){
		phy_shape *next = shape->next;
		func(body, shape, data);
		shape = next;
	}
}

void
phy_body_each_constraint(phy_body *body, phy_body_constraint_iterator_func func, void *data)
{
	phy_constraint *constraint = body->constraintList;
	while(constraint){
		phy_constraint *next = cpConstraintNext(constraint, body);
		func(body, constraint, data);
		constraint = next;
	}
}

void
phy_body_each_arbiter(phy_body *body, phy_body_arbiter_iterator_func func, void *data)
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
