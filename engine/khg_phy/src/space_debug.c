#include "khg_phy/phy_private.h"
#include "khg_phy/pin_joint.h"
#include "khg_phy/slide_joint.h"
#include "khg_phy/pivot_joint.h"
#include "khg_phy/groove_joint.h"
#include "khg_phy/damped_spring.h"
#include "khg_phy/transform.h"

#ifndef CP_SPACE_DISABLE_DEBUG_API

static void
cpSpaceDebugDrawShape(phy_shape *shape, cpSpaceDebugDrawOptions *options)
{
	phy_body *body = shape->body;
	phy_data_pointer data = options->data;
	
	cpSpaceDebugColor outline_color = options->shapeOutlineColor;
	cpSpaceDebugColor fill_color = options->colorForShape(shape, data);
	
	switch(shape->class->type){
		case PHY_CIRCLE_SHAPE: {
			phy_circle_shape *circle = (phy_circle_shape *)shape;
			options->drawCircle(circle->tc, body->a, circle->r, outline_color, fill_color, data);
			break;
		}
		case PHY_SEGMENT_SHAPE: {
			phy_segment_shape *seg = (phy_segment_shape *)shape;
			options->drawFatSegment(seg->ta, seg->tb, seg->r, outline_color, fill_color, data);
			break;
		}
		case PHY_POLY_SHAPE: {
			phy_poly_shape *poly = (phy_poly_shape *)shape;
			
			int count = poly->count;
			struct phy_splitting_plane *planes = poly->planes;
			phy_vect *verts = (phy_vect *)alloca(count*sizeof(phy_vect));
			
			for(int i=0; i<count; i++) verts[i] = planes[i].v0;
			options->drawPolygon(count, verts, poly->r, outline_color, fill_color, data);
			break;
		}
		default: break;
	}
}

static const phy_vect spring_verts[] = {
	{0.00f, 0.0f},
	{0.20f, 0.0f},
	{0.25f, 3.0f},
	{0.30f,-6.0f},
	{0.35f, 6.0f},
	{0.40f,-6.0f},
	{0.45f, 6.0f},
	{0.50f,-6.0f},
	{0.55f, 6.0f},
	{0.60f,-6.0f},
	{0.65f, 6.0f},
	{0.70f,-3.0f},
	{0.75f, 6.0f},
	{0.80f, 0.0f},
	{1.00f, 0.0f},
};
static const int spring_count = sizeof(spring_verts)/sizeof(phy_vect);

static void
cpSpaceDebugDrawConstraint(phy_constraint *constraint, cpSpaceDebugDrawOptions *options)
{
	phy_data_pointer data = options->data;
	cpSpaceDebugColor color = options->constraintColor;
	
	phy_body *body_a = constraint->a;
	phy_body *body_b = constraint->b;

	if(cpConstraintIsPinJoint(constraint)){
		phy_pin_joint *joint = (phy_pin_joint *)constraint;
		
		phy_vect a = cpTransformPoint(body_a->transform, joint->anchor_A);
		phy_vect b = cpTransformPoint(body_b->transform, joint->anchor_B);
		
		options->drawDot(5, a, color, data);
		options->drawDot(5, b, color, data);
		options->drawSegment(a, b, color, data);
	} else if(cpConstraintIsSlideJoint(constraint)){
		phy_slide_joint *joint = (phy_slide_joint *)constraint;
	
		phy_vect a = cpTransformPoint(body_a->transform, joint->anchor_A);
		phy_vect b = cpTransformPoint(body_b->transform, joint->anchor_B);
		
		options->drawDot(5, a, color, data);
		options->drawDot(5, b, color, data);
		options->drawSegment(a, b, color, data);
	} else if(cpConstraintIsPivotJoint(constraint)){
		phy_pivot_joint *joint = (phy_pivot_joint *)constraint;
	
		phy_vect a = cpTransformPoint(body_a->transform, joint->anchor_A);
		phy_vect b = cpTransformPoint(body_b->transform, joint->anchor_B);

		options->drawDot(5, a, color, data);
		options->drawDot(5, b, color, data);
	} else if(phy_constraint_is_groove_joint(constraint)){
		phy_groove_joint *joint = (phy_groove_joint *)constraint;
	
		phy_vect a = cpTransformPoint(body_a->transform, joint->grv_a);
		phy_vect b = cpTransformPoint(body_a->transform, joint->grv_b);
		phy_vect c = cpTransformPoint(body_b->transform, joint->anchor_B);
		
		options->drawDot(5, c, color, data);
		options->drawSegment(a, b, color, data);
	} else if(phy_constraint_is_damped_spring(constraint)){
		phy_damped_spring *spring = (phy_damped_spring *)constraint;
		
		phy_vect a = cpTransformPoint(body_a->transform, spring->anchor_A);
		phy_vect b = cpTransformPoint(body_b->transform, spring->anchor_B);
		
		options->drawDot(5, a, color, data);
		options->drawDot(5, b, color, data);

		phy_vect delta = cpvsub(b, a);
		float cos = delta.x;
		float sin = delta.y;
		float s = 1.0f/cpvlength(delta);
		
		phy_vect r1 = cpv(cos, -sin*s);
		phy_vect r2 = cpv(sin,  cos*s);
		
		phy_vect *verts = (phy_vect *)alloca(spring_count*sizeof(phy_vect));
		for(int i=0; i<spring_count; i++){
			phy_vect v = spring_verts[i];
			verts[i] = cpv(cpvdot(v, r1) + a.x, cpvdot(v, r2) + a.y);
		}
		
		for(int i=0; i<spring_count-1; i++){
			options->drawSegment(verts[i], verts[i + 1], color, data);
		}
	}
}

void
cpSpaceDebugDraw(phy_space *space, cpSpaceDebugDrawOptions *options)
{
	if(options->flags & CP_SPACE_DEBUG_DRAW_SHAPES){
		cpSpaceEachShape(space, (cpSpaceShapeIteratorFunc)cpSpaceDebugDrawShape, options);
	}
	
	if(options->flags & CP_SPACE_DEBUG_DRAW_CONSTRAINTS){
		cpSpaceEachConstraint(space, (cpSpaceConstraintIteratorFunc)cpSpaceDebugDrawConstraint, options);
	}
	
	if(options->flags & CP_SPACE_DEBUG_DRAW_COLLISION_POINTS){
		phy_array *arbiters = space->arbiters;
		cpSpaceDebugColor color = options->collisionPointColor;
		cpSpaceDebugDrawSegmentImpl draw_seg = options->drawSegment;
		phy_data_pointer data = options->data;
		
		for(int i=0; i<arbiters->num; i++){
			phy_arbiter *arb = (phy_arbiter*)arbiters->arr[i];
			phy_vect n = arb->n;
			
			for(int j=0; j<arb->count; j++){
				phy_vect p1 = cpvadd(arb->body_a->p, arb->contacts[j].r1);
				phy_vect p2 = cpvadd(arb->body_b->p, arb->contacts[j].r2);
				
				float d = 2.0f;
				phy_vect a = cpvadd(p1, cpvmult(n, -d));
				phy_vect b = cpvadd(p2, cpvmult(n,  d));
				draw_seg(a, b, color, data);
			}
		}
	}
}

#endif
