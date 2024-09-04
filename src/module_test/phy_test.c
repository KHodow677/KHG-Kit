#include "phy_test.h"
#include "khg_phy/body.h"
#include "khg_phy/constraint.h"
#include "khg_phy/gear_joint.h"
#include "khg_phy/phy.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/pivot_joint.h"
#include "khg_phy/poly_shape.h"
#include "khg_phy/shape.h"
#include "khg_phy/space.h"
#include "khg_phy/vect.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static phy_body *tankBody, *tankControlBody;
static phy_constraint *pivot, *gear;

static inline float frand(void) {
	return (float)rand()/(float)RAND_MAX;
}

static void update(phy_space *space, double dt) {
	phy_vect mouseDelta = phy_v(0.5f, 0.5f);
	float turn = phy_v_to_angle(phy_v_unrotate(phy_body_get_rotation(tankBody), mouseDelta));
	phy_body_set_angle(tankControlBody, phy_body_get_angle(tankBody) - turn);
  if(phy_v_near(phy_v(0.0f, 0.0f), phy_body_get_position(tankBody), 30.0)){
		phy_body_set_velocity(tankControlBody, phy_v_zero);
	} 
  else {
		float direction = (phy_v_dot(mouseDelta, phy_body_get_position(tankBody)) > 0.0 ? 1.0 : -1.0);
		phy_body_set_velocity(tankControlBody, phy_v_rotate(phy_body_get_rotation(tankBody), phy_v(30.0f*direction, 0.0f)));
	}
  phy_vect tank_body_pos = phy_body_get_position(tankBody);
  printf("Body -> x: %f, y: %f\n", tank_body_pos.x, tank_body_pos.y);
  phy_vect tank_control_body_pos = phy_body_get_position(tankControlBody);
  printf("Control -> x: %f, y: %f\n", tank_control_body_pos.x, tank_control_body_pos.y);
	phy_space_step(space, dt);
}

static phy_body *add_box(phy_space *space, float size, float mass) {
	float radius = phy_v_length(phy_v(size, size));
	phy_body *body = phy_space_add_body(space, phy_body_new(mass, phy_moment_for_box(mass, size, size)));
	phy_body_set_position(body, phy_v(frand()*(640 - 2*radius) - (320 - radius), frand()*(480 - 2*radius) - (240 - radius)));
	phy_shape *shape = phy_space_add_shape(space, phy_box_shape_new(body, size, size, 0.0));
	phy_shape_set_elasticity(shape, 0.0f);
	phy_shape_set_friction(shape, 0.7f);
	return body;
}

static phy_space *init(void) {
	phy_space *space = phy_space_new();
	phy_space_set_iterations(space, 10);
	phy_space_set_sleep_time_threshold(space, 0.5f);
	phy_body *staticBody = phy_space_get_static_body(space);
	phy_shape *shape;
	for(int i=0; i<50; i++) {
		phy_body *body = add_box(space, 20, 1);
		phy_constraint *pivot = phy_space_add_constraint(space, phy_pivot_joint_new_2(staticBody, body, phy_v_zero, phy_v_zero));
		phy_constraint_set_max_bias(pivot, 0);
		phy_constraint_set_max_force(pivot, 1000.0f);
		phy_constraint *gear = phy_space_add_constraint(space, phy_gear_joint_new(staticBody, body, 0.0f, 1.0f));
		phy_constraint_set_max_bias(gear, 0);
		phy_constraint_set_max_force(gear, 5000.0f);
	}
	tankControlBody = phy_space_add_body(space, phy_body_new_kinematic());
	tankBody = add_box(space, 30, 10);
	pivot = phy_space_add_constraint(space, phy_pivot_joint_new_2(tankControlBody, tankBody, phy_v_zero, phy_v_zero));
	phy_constraint_set_max_bias(pivot, 0);
	phy_constraint_set_max_force(pivot, 10000.0f);
	gear = phy_space_add_constraint(space, phy_gear_joint_new(tankControlBody, tankBody, 0.0f, 1.0f));
	phy_constraint_set_error_bias(gear, 0);
	phy_constraint_set_max_bias(gear, 1.2f);
	phy_constraint_set_max_force(gear, 50000.0f);
	return space;
}

static void ShapeFreeWrap(phy_space *space, phy_shape *shape, void *unused){
	phy_space_remove_shape(space, shape);
	phy_shape_free(shape);
}

static void PostShapeFree(phy_shape *shape, phy_space *space){
	phy_space_add_post_step_callback(space, (phy_post_step_func)ShapeFreeWrap, shape, NULL);
}

static void ConstraintFreeWrap(phy_space *space, phy_constraint *constraint, void *unused){
	phy_space_remove_constraint(space, constraint);
	phy_constraint_free(constraint);
}

static void PostConstraintFree(phy_constraint *constraint, phy_space *space){
	phy_space_add_post_step_callback(space, (phy_post_step_func)ConstraintFreeWrap, constraint, NULL);
}

static void BodyFreeWrap(phy_space *space, phy_body *body, void *unused){
	phy_space_remove_body(space, body);
	phy_body_free(body);
}

static void PostBodyFree(phy_body *body, phy_space *space){
	phy_space_add_post_step_callback(space, (phy_post_step_func)BodyFreeWrap, body, NULL);
}

static void free_space_children(phy_space *space) {
	phy_space_each_shape(space, (phy_space_shape_iterator_func)PostShapeFree, space);
	phy_space_each_constraint(space, (phy_space_constraint_iterator_func)PostConstraintFree, space);
	phy_space_each_body(space, (phy_space_body_iterator_func)PostBodyFree, space);
}

static void destroy(phy_space *space) {
	free_space_children(space);
	phy_space_free(space);
}

int phy_test() {
  float dt = 0.016;
  phy_space *space = init();
  while (1) {
    update(space, dt);
    usleep(dt*1000000);
  }
  destroy(space);
  return 0;
}

