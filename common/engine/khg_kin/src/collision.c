#define NAMESPACE_KIN_IMPL

#include "khg_kin/namespace.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

static int collide_error(kin_collision_data *data, kin_shape shape1, kin_shape shape2);
static int collide_circle_circle(kin_collision_data *data, kin_shape shape1, kin_shape shape2);
static int collide_polygon_polygon(kin_collision_data *data, kin_shape shape1, kin_shape shape2);
static int collide_circle_polygon(kin_collision_data *data, kin_shape shape1, kin_shape shape2);
static int collide_segment_segment(kin_collision_data *data, kin_shape shape1, kin_shape shape2);
static int collide_circle_segment(kin_collision_data *data, kin_shape shape1, kin_shape shape2);
static int collide_polygon_segment(kin_collision_data *data, kin_shape shape1, kin_shape shape2);

static kin_collide_shape COLLIDE_DISPATCH[] = { collide_error, collide_circle_circle, collide_polygon_polygon, collide_circle_polygon, collide_segment_segment, collide_circle_segment, collide_polygon_segment, collide_error };

static int collide_error(kin_collision_data *data, kin_shape shape1, kin_shape shape2) {
  printf("Collide Error\n");
  abort();
}

static int collide_circle_circle(kin_collision_data *data, kin_shape shape1, kin_shape shape2) {
  printf("Collide Circle Circle\n");
  NAMESPACE_KIN_INTERNAL.shape_print(shape1);
  NAMESPACE_KIN_INTERNAL.shape_print(shape2);
  kin_shape_circle c1 = (kin_shape_circle)shape1;
  kin_shape_circle c2 = (kin_shape_circle)shape2;
  kin_vec dc12 = NAMESPACE_KIN_INTERNAL.vec_sub(c2->world_center, c1->world_center);
  double d12 = NAMESPACE_KIN_INTERNAL.vec_length(dc12);
  if((d12 > c1->radius + c2->radius) ||
    (d12 < fabs(c1->radius - c2->radius))) {
    data->num_points = 0;
    return 0;
  }
  if((d12 == c1->radius + c2->radius) || (d12 == fabs(c1->radius - c2->radius))) {
    data->num_points = 1;
    NAMESPACE_KIN_INTERNAL.vec_copy(&data->points[0], NAMESPACE_KIN_INTERNAL.vec_add(c1->world_center, NAMESPACE_KIN_INTERNAL.vec_scale(dc12, c1->radius / d12)));
    return 1;
  }
  data->num_points = 2;
  double a = sqrt(c1->radius * c1->radius - c2->radius * c2->radius + d12 * d12) / (2.0 * d12);
  kin_vec Q = NAMESPACE_KIN_INTERNAL.vec_add(c1->world_center, NAMESPACE_KIN_INTERNAL.vec_scale(dc12, a));
  kin_vec H = NAMESPACE_KIN_INTERNAL.vec_scale(NAMESPACE_KIN_INTERNAL.vec_perp(dc12), sqrt(c1->radius * c1->radius - a * a) / d12);
  NAMESPACE_KIN_INTERNAL.vec_copy(&data->points[0], NAMESPACE_KIN_INTERNAL.vec_add(Q, H));
  NAMESPACE_KIN_INTERNAL.vec_copy(&data->points[1], NAMESPACE_KIN_INTERNAL.vec_sub(Q, H));
  return 1;
}

static int collide_polygon_polygon(kin_collision_data *data, kin_shape shape1, kin_shape shape2) {
  printf("Collide Polygon Polygon\n");
  NAMESPACE_KIN_INTERNAL.shape_print(shape1);
  NAMESPACE_KIN_INTERNAL.shape_print(shape2);
  return 0;
}

static int collide_circle_polygon(kin_collision_data *data, kin_shape shape1, kin_shape shape2) {
  printf("Collide Circle Polygon\n");
  NAMESPACE_KIN_INTERNAL.shape_print(shape1);
  NAMESPACE_KIN_INTERNAL.shape_print(shape2);
  return 0;
}

static int collide_segment_segment(kin_collision_data *data, kin_shape shape1, kin_shape shape2) {
  printf("Collide Segment Segment\n");
  NAMESPACE_KIN_INTERNAL.shape_print(shape1);
  NAMESPACE_KIN_INTERNAL.shape_print(shape2);
  kin_shape_segment ss1 = (kin_shape_segment)shape1;
  kin_shape_segment ss2 = (kin_shape_segment)shape2;
  kin_vec dl1 = NAMESPACE_KIN_INTERNAL.vec_sub(ss1->end_point, ss1->start_point);
  kin_vec dl2 = NAMESPACE_KIN_INTERNAL.vec_sub(ss2->end_point, ss2->start_point);
  double llcp = NAMESPACE_KIN_INTERNAL.vec_cross(dl2, dl1);
  if(llcp == 0.0) {
    data->num_points = 0;
    return 0;
  }
  kin_vec ds = NAMESPACE_KIN_INTERNAL.vec_sub(ss2->start_point, ss1->start_point);
  double lscp = NAMESPACE_KIN_INTERNAL.vec_cross(dl1, ds);
  double u = lscp / llcp;
  if((u < 0.0) || (u > 1.0)) {
    data->num_points = 0;
    return 0;
  }
  double t;
  if(dl1.x != 0.0) {
    t = (ds.x + u * dl2.x) / dl1.x;
  }
  else {
    t = (ds.y + u * dl2.y) / dl1.y;
  }
  if((t < 0.0) || (t > 1.0)) {
    data->num_points = 0;
    return 0;
  }
  data->num_points = 1;
  NAMESPACE_KIN_INTERNAL.vec_copy(&data->points[0], NAMESPACE_KIN_INTERNAL.vec_add(ss1->start_point, NAMESPACE_KIN_INTERNAL.vec_scale(dl1, t)));
  return 0;
}

static int collide_circle_segment(kin_collision_data *data, kin_shape shape1, kin_shape shape2) {
  printf("Collide Circle Segment\n");
  NAMESPACE_KIN_INTERNAL.shape_print(shape1);
  NAMESPACE_KIN_INTERNAL.shape_print(shape2);
  kin_shape_circle c = (kin_shape_circle)shape1;
  kin_shape_segment s = (kin_shape_segment)shape2;
  kin_vec l = NAMESPACE_KIN_INTERNAL.vec_sub(s->end_point, s->start_point);
  kin_vec sc = NAMESPACE_KIN_INTERNAL.vec_sub(c->world_center, s->start_point);
  double A = NAMESPACE_KIN_INTERNAL.vec_length(l);
  double B = -2.0 * NAMESPACE_KIN_INTERNAL.vec_dot(sc, l);
  double C = NAMESPACE_KIN_INTERNAL.vec_sq_length(sc) - c->radius * c->radius;
  double D = B*B - 4.0 * A * C;
  if(D < 0) {
    data->num_points = 0;
    return 0;
  }
  if(D == 0) {
    data->num_points = 0;
    double t = - B / (2.0 * A);
    if((t < 0.0) || (t > 1.0)) {
      data->num_points = 0;
      return 0;
    }
    data->num_points = 1;
    NAMESPACE_KIN_INTERNAL.vec_copy(&data->points[0], NAMESPACE_KIN_INTERNAL.vec_add(s->start_point, NAMESPACE_KIN_INTERNAL.vec_scale(l, t)));
    return 1;      
  }
  data->num_points = 0;
  double t = (-B + D) / (2.0 * A);
  if((t >= 0.0) && (t <= 1.0)) {
    NAMESPACE_KIN_INTERNAL.vec_copy(&data->points[data->num_points], NAMESPACE_KIN_INTERNAL.vec_add(s->start_point, NAMESPACE_KIN_INTERNAL.vec_scale(l, t)));
    ++data->num_points;
  }
  t = (-B - D) / (2.0 * A);
  if((t >= 0.0) && (t <= 1.0)) {
    NAMESPACE_KIN_INTERNAL.vec_copy(&data->points[data->num_points], NAMESPACE_KIN_INTERNAL.vec_add(s->start_point, NAMESPACE_KIN_INTERNAL.vec_scale(l, t)));
    ++data->num_points;
  }   
  return (data->num_points > 0);
}

static int collide_polygon_segment(kin_collision_data *data, kin_shape shape1, kin_shape shape2) {
  printf("Collide Polygon Segment\n");
  NAMESPACE_KIN_INTERNAL.shape_print(shape1);
  NAMESPACE_KIN_INTERNAL.shape_print(shape2);
  return 0;
}

int collide_bb(kin_shape shape1, kin_shape shape2) {
  if((shape1->bb.top < shape2->bb.bottom) || (shape2->bb.top < shape1->bb.bottom) || (shape1->bb.right < shape2->bb.left) || (shape2->bb.right < shape1->bb.left)) {
    return 0;
  }
  return 1;
}

int collide_shapes(kin_collision_data *data, kin_shape shape1, kin_shape shape2) {
  if(shape1->type > shape2->type) {
    kin_shape tmp = shape1;
    shape1 = shape2;
    shape2 = tmp;
  }
  COLLIDE_DISPATCH[shape1->type | shape2->type](data, shape1, shape2);
  return data->num_points > 0;
}
