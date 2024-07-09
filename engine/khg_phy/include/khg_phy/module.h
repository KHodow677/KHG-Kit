#include "khg_phy/data.h"

KHGPHYDEF void init_physics(void);
KHGPHYDEF void run_physics_step(void);
KHGPHYDEF void set_physics_time_step(double delta);
KHGPHYDEF bool is_physics_enabled(void);
KHGPHYDEF void set_physics_gravity(float x, float y);
KHGPHYDEF physics_body create_physics_body_circle(vec2 pos, float radius, float density);
KHGPHYDEF physics_body create_physics_body_rectangle(vec2 pos, float width, float height, float density);
KHGPHYDEF physics_body create_physics_body_polygon(vec2 pos, float radius, int sides, float density);
KHGPHYDEF void physics_add_force(physics_body body, vec2 force);
KHGPHYDEF void physics_add_torque(physics_body body, float amount);
KHGPHYDEF void physics_shatter(physics_body body, vec2 position, float force);
KHGPHYDEF int get_physics_bodies_count(void);
KHGPHYDEF physics_body get_physics_body(int index);
KHGPHYDEF int get_physics_shape_type(int index);
KHGPHYDEF int get_physics_shape_vertices_count(int index);
KHGPHYDEF vec2 get_physics_shape_vertex(physics_body body, int vertex);
KHGPHYDEF void set_physics_body_rotation(physics_body body, int vertex);
KHGPHYDEF void destroy_physics_body(physics_body body);
KHGPHYDEF void close_physics(void);
