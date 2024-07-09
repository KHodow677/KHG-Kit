#pragma once

#include "khg_math/mat2.h"
#include "khg_math/minmax.h"
#include "khg_math/vec2.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

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
#define KHGPHY_FLT_MAX 3.402823466e+38f
#define KHGPHY_EPSILON 0.000001f
#define KHGPHY_K 1.0f / 3.0f
#define KHGPHY_VECTOR_0 (vec2) { 0.0f, 0.0f }

#if defined(_WIN32)
  int __stdcall QueryPerformanceCounter(unsigned long long int *lpPerformanceCount);
  int __stdcall QueryPerformanceFrequency(unsigned long long int *lpFrequency);
#elif defined(__linux__)
  #if _POSIX_C_SOURCE < 199309L
    #undef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 199309L
  #endif
  #include <sys/time.h>
#elif defined(__APPLE__)
  #include <mach/mach_time.h>
#elif defined(EMSCRIPTEN)
  #include <emscripten.h>
#endif

typedef enum physics_shape_type { PHYSICS_CIRCLE, PHYSICS_POLYGON } physics_shape_type;
typedef struct physics_body_data *physics_body;

typedef struct polygon_data {
  unsigned int vertex_count;
  vec2 positions[KHGPHY_MAX_VERTICES];
  vec2 normals[KHGPHY_MAX_VERTICES];
} polygon_data;

typedef struct physics_shape {
  physics_shape_type type;
  physics_body body;
  float radius;
  mat2 transform;
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

static pthread_t physics_thread_id;
static unsigned int used_memory = 0;
static volatile bool physics_thread_enabled = false;
static double base_time = 0.0;
static double start_time = 0.0;
static double delta_time = 1.0/60.0/10.0 * 1000;
static double current_time = 0.0;
static uint64_t frequency = 0;
static double accumulator = 0.0;
static unsigned int steps_count = 0;
static vec2 gravity_force = { 0.0f, 9.81f };
static physics_body bodies[KHGPHY_MAX_BODIES];
static unsigned int physics_bodies_count = 0;
static physics_manifold contacts[KHGPHY_MAX_MANIFOLDS];
static unsigned int physics_manifolds_count = 0;
