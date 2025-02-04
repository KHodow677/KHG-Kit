#pragma once

typedef struct kin_body_raw *kin_body;
typedef struct kin_engine_raw *kin_engine;
typedef struct kin_shape_raw *kin_shape;
typedef struct kin_shape_circle_raw *kin_shape_circle;
typedef struct kin_shape_polygon_raw *kin_shape_polygon;
typedef struct kin_shape_segment_raw *kin_shape_segment;

typedef void (*kin_shape_pose_update)(kin_shape s);
typedef void (*kin_shape_bb_update)(kin_shape s);

typedef struct kin_vec {
  float x;
  float y;
} kin_vec;

typedef struct kin_collision_bb {
  float top;
  float bottom;
  float right;
  float left;
} kin_collision_bb;

typedef struct kin_collision_data {
  unsigned int num_points;
  kin_vec points[2];
  kin_vec normals[2];
} kin_collision_data;

typedef int (*kin_collide_shape)(kin_collision_data *, kin_shape, kin_shape);

typedef enum kin_shape_type {
  KIN_SHAPE_CIRCLE  = 1 << 0,
  KIN_SHAPE_POLYGON = 1 << 1,
  KIN_SHAPE_SEGMENT = 1 << 2
} kin_shape_type;

struct kin_body_raw {
  kin_vec pos;
  kin_vec rot_vector;
  float rot_scalar;
  kin_vec lin_vel;
  float ang_vel;
  kin_vec force;
  float torque;
  float mass;
  float inertia;
  kin_shape shapes;
  kin_body next;
};

struct kin_engine_raw {
  kin_body bodies;
};

struct kin_shape_raw {
  kin_shape_type type;
  kin_body body;
  kin_vec pos_off;
  kin_vec ang_off;
  kin_collision_bb bb;
  kin_shape_pose_update pose_update;
  kin_shape_bb_update bb_update;
  kin_shape next;
};

struct kin_shape_circle_raw {
  struct kin_shape_raw generic;
  kin_vec world_center;
  float radius;
};

struct kin_shape_polygon_raw {
  struct kin_shape_raw generic;
  kin_vec *world_vertices;
  kin_vec *body_vertices;
  unsigned int num_vertices;
};

struct kin_shape_segment_raw {
  struct kin_shape_raw generic;
  kin_vec start_point;
  kin_vec end_point;
  kin_vec body_start_point;
  kin_vec world_end_point;
};

#if defined(NAMESPACE_KIN_IMPL) || defined(NAMESPACE_KIN_USE)
typedef struct kin_namespace {
  kin_body (*body_create)(void);
  void (*body_deallocate)(kin_body *);
  void (*body_step)(kin_body, float);
  void (*body_reset)(kin_body);
  void (*body_force_set)(kin_body, kin_vec);
  void (*body_torque_set)(kin_body, float);
  void (*body_force_add)(kin_body, kin_vec);
  void (*body_torque_add)(kin_body, float);
  void (*body_shape_add)(kin_body, kin_shape, kin_vec, kin_vec);
  void (*body_shape_remove)(kin_body, kin_shape);
  void (*body_print)(kin_body);
  kin_vec (*world_to_local)(kin_body, kin_vec);
  kin_vec (*local_to_world)(kin_body, kin_vec);
  int (*collide_bb)(kin_shape, kin_shape);
  int (*collide_shapes)(kin_collision_data *, kin_shape, kin_shape);
  kin_engine (*engine_create)(void);
  void (*engine_deallocate)(kin_engine *);
  void (*engine_step)(kin_engine, float);
  void (*engine_body_add)(kin_engine, kin_body);
  void (*engine_body_remove)(kin_engine, kin_body);
  void (*engine_print)(kin_engine);
  kin_shape_circle (*shape_circle_create)(float);
  kin_shape_polygon (*shape_polygon_create)(unsigned int, kin_vec *);
  kin_shape_segment (*shape_segment_create)(kin_vec, kin_vec);
  void (*shape_deallocate)(kin_shape *);
  void (*shape_circle_pose_update)(kin_shape);
  void (*shape_circle_bb_update)(kin_shape);
  void (*shape_polygon_pose_update)(kin_shape);
  void (*shape_polygon_bb_update)(kin_shape);
  void (*shape_segment_pose_update)(kin_shape);
  void (*shape_segment_bb_update)(kin_shape);
  void (*shape_print)(kin_shape);
  kin_vec (*vec_create)(float, float);
  void (*vec_copy)(kin_vec *, kin_vec);
  float (*vec_sq_length)(kin_vec);
  float (*vec_length)(kin_vec);
  kin_vec (*vec_perp)(kin_vec);
  kin_vec (*vec_add)(kin_vec, kin_vec);
  kin_vec (*vec_sub)(kin_vec, kin_vec);
  float (*vec_dot)(kin_vec, kin_vec);
  float (*vec_cross)(kin_vec, kin_vec);
  kin_vec (*vec_scale)(kin_vec, float);
  float (*vec_dist)(kin_vec, kin_vec);
  kin_vec (*vec_rot)(kin_vec, kin_vec);
} kin_namespace;
#endif

#ifdef NAMESPACE_KIN_IMPL
extern kin_namespace NAMESPACE_KIN_INTERNAL;
#endif

#ifdef NAMESPACE_KIN_USE
kin_namespace *NAMESPACE_KIN(void);
#endif
