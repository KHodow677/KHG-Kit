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
