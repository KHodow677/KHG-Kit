#pragma once

#include "khg_math/vec2.h"
#include <stdbool.h>

#define KHGPHYDEF extern

#define KHGPHY_MAX_BODIES 64
#define KHGPHY_MAX_MANIFOLDS 4096
#define KHGPHY_MAX_VERTICES 24
#define KHGPHY_CIRCLE_VERTICES 24

#define KHGPHY_COLLISION_ITERATIONS 100
#define KHGPHY_PENETRATION_ALLOWANCE 0.05f
#define KHGPHY_PENETRATION_CORRECTION 0.4f

#define KHGPHY_PI 3.14159265358979323846
#define KHGPHY_DEG2RAD (KHGPHY_PI / 180.0f)

typedef enum physics_shape_type { PHYSICS_CIRCLE, PHYSICS_POLYGON } physics_shape_type;
typedef struct physics_body_data *physics_body;
typedef struct mat_2 {
  float m00;
  float m01;
  float m10;
  float m11;
} mat_2;

typedef struct polygon_data {
  unsigned int vertex_count;
  vec2 positions[KHGPHY_MAX_VERTICES];
  vec2 normals[KHGPHY_MAX_VERTICES];
} polygon_data;

typedef struct physics_shape {
  physics_shape_type type;
  physics_body body;
  float radius;
  mat_2 transform;
  polygon_data vertex_data;
} physics_shape;

typedef struct physics_body_data {
  unsigned int id;
  bool enabled;
  vec2 position;
  vec2 velocity;
  vec2 force;
  float angular_velocity;
  float torque;
  float orient;
  float inertia;
  float inverse_inertia;
  float mass;
  float inverse_mass;
  float dynamic_friction;
  float static_friction;
  float restitution;
  bool use_gravity;
  bool is_grounded;
  bool freeze_orient;
  physics_shape shape;
} physics_body_data;

typedef struct physics_manifold_data {
  unsigned int id;
  physics_body body_a;
  physics_body body_b;
  float penetration;
  vec2 normal;
  vec2 contacts[2];
  unsigned int contacts_count;
  float restitution;
  float dynamic_friction;
  float static_friction;
} physics_manifold_data, *physics_manifold;
