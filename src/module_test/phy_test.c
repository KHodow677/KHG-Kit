#include "phy_test.h"
#include "khg_phy/phy.h"
#include "khg_phy/body.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/space.h"
#include <stdio.h>

int phy_test() {
  // phy_vect is a 2D vector and cpv() is a shortcut for initializing them.
  phy_vect gravity = phy_v(0, -100);
  
  // Create an empty space.
  phy_space *space = phy_space_new();
  phy_space_set_gravity(space, gravity);
  
  // Add a static line segment shape for the ground.
  // We'll make it slightly tilted so the ball will roll off.
  // We attach it to a static body to tell Chipmunk it shouldn't be movable.
  phy_shape *ground = phy_segment_shape_new(phy_space_get_static_body(space), phy_v(-20, 5), phy_v(20, -5), 0);
  phy_shape_set_friction(ground, 1);
  phy_space_add_shape(space, ground);
  
  // Now let's make a ball that falls onto the line and rolls off.
  // First we need to make a cpBody to hold the physical properties of the object.
  // These include the mass, position, velocity, angle, etc. of the object.
  // Then we attach collision shapes to the cpBody to give it a size and shape.
  
  float radius = 5;
  float mass = 1;
  
  // The moment of inertia is like mass for rotation
  // Use the cpMomentFor*() functions to help you approximate it.
  float moment = phy_moment_for_circle(mass, 0, radius, phy_v_zero);
  
  // The cpSpaceAdd*() functions return the thing that you are adding.
  // It's convenient to create and add an object in one line.
  phy_body *ballBody = phy_space_add_body(space, phy_body_new(mass, moment));
  phy_body_set_position(ballBody, phy_v(0, 15));
  
  // Now we create the collision shape for the ball.
  // You can create multiple collision shapes that point to the same body.
  // They will all be attached to the body and move around to follow it.
  phy_shape *ballShape = phy_space_add_shape(space, cpCircleShapeNew(ballBody, radius, phy_v_zero));
  phy_shape_set_friction(ballShape, 0.7);
  
  // Now that it's all set up, we simulate all the objects in the space by
  // stepping forward through time in small increments called steps.
  // It is *highly* recommended to use a fixed size time step.
  float timeStep = 1.0/60.0;
  for(float time = 0; time < 2; time += timeStep){
    phy_vect pos = phy_body_get_position(ballBody);
    phy_vect vel = phy_body_get_velocity(ballBody);
    printf( "Time is %5.2f. ballBody is at (%5.2f, %5.2f). It's velocity is (%5.2f, %5.2f)\n", time, pos.x, pos.y, vel.x, vel.y);
    phy_space_step(space, timeStep);
  }
  
  // Clean up our objects and exit!
  phy_shape_free(ballShape);
  phy_body_free(ballBody);
  phy_shape_free(ground);
  phy_space_free(space);
  
  return 0;
}

