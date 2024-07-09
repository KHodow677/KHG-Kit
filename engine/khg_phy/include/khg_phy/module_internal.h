#include "khg_phy/data.h"

static int find_available_body_index(void);
static polygon_data create_random_polygon(float radius, int sides);
static void *physics_loop(void *arg);
static void physics_step(void);
static int find_available_manifold_index(void);
static physics_manifold create_physics_manifold(physics_body a, physics_body b);
static void destroy_physics_manifold(physics_manifold manifold);
static void solve_physics_manifold(physics_manifold manifold);
static void solve_circle_to_circle(physics_manifold manifold);
static void solve_circle_to_polygon(physics_manifold manifold);
static void solve_polygon_to_circle(physics_manifold manifold);
static void solve_different_shapes(physics_manifold manifold, physics_body body_a, physics_body body_b);
static void solve_polygon_to_polygon(physics_manifold manifold);
static void integrate_physics_forces(physics_body body);
static void initialize_physics_manifolds(physics_manifold manifold);
static void integrate_physics_inpulses(physics_manifold manifold);
static void integrate_physics_velocity(physics_body body);
static void correct_physics_positions(physics_manifold manifold);
static float find_axis_least_penetration(int *face_index, physics_shape shape_a, physics_shape shape_b);
static void find_incident_face(vec2 *v0, vec2 *v1, physics_shape ref, physics_shape inc, int index);
static int clip(vec2 normal, float clip, vec2 *face_a, vec2 *face_b);
static bool bias_greater_than(float value_a, float value_b);
static vec2 triangle_barycenter(vec2 v1, vec2 v2, vec2 v3);

static void init_timer(void);
static uint64_t get_time_count(void);
static double get_current_time(void);
