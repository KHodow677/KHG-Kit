#include "khg_kin/collision.h"
#include "khg_kin/namespace.h"
#include "khg_kin/shape.h"
#include "khg_kin/vec.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

int kin_collide_bb(kin_shape s1, kin_shape s2) {
  if((s1->bb.top < s2->bb.bottom) || (s2->bb.top < s1->bb.bottom) || (s1->bb.right < s2->bb.left) || (s2->bb.right < s1->bb.left)) {
    return 0;
  }
  return 1;
}

static int bkCollideError(kin_collision_data* data, kin_shape s1, kin_shape s2) {
  printf("bkCollideError\n");
  abort();
}

static int bkCollideCircleCircle(kin_collision_data* data, kin_shape s1, kin_shape s2) {
  printf("bkCollideCircleCircle\n");
  kin_shape_print(s1);
  kin_shape_print(s2);
  kin_shape_circle c1 = (kin_shape_circle)s1;
  kin_shape_circle c2 = (kin_shape_circle)s2;
  kin_vec dc12 = kin_vec_sub(c2->world_center, c1->world_center);
  double d12 = kin_vec_length(dc12);
  if((d12 > c1->radius + c2->radius) ||
    (d12 < fabs(c1->radius - c2->radius))) {
    data->num_points = 0;
    return 0;
  }
  if((d12 == c1->radius + c2->radius) || (d12 == fabs(c1->radius - c2->radius))) {
    data->num_points = 1;
    kin_vec_copy(&data->points[0], kin_vec_add(c1->world_center, kin_vec_scale(dc12, c1->radius / d12)));
    return 1;
  }
  data->num_points = 2;
  double a = sqrt(c1->radius * c1->radius - c2->radius * c2->radius + d12 * d12) / (2.0 * d12);
  kin_vec Q = kin_vec_add(c1->world_center, kin_vec_scale(dc12, a));
  kin_vec H = kin_vec_scale(kin_vec_perp(dc12), sqrt(c1->radius * c1->radius - a * a) / d12);
  kin_vec_copy(&data->points[0], kin_vec_add(Q, H));
  kin_vec_copy(&data->points[1], kin_vec_sub(Q, H));
  return 1;
}

static int bkCollidePolygonPolygon(kin_collision_data* data, kin_shape s1, kin_shape s2) {
  printf("bkCollidePolygonPolygon\n");
  kin_shape_print(s1);
  kin_shape_print(s2);
  return 0;
}

static int bkCollideCirclePolygon(kin_collision_data* data, kin_shape s1, kin_shape s2) {
  printf("bkCollideCirclePolygon\n");
  kin_shape_print(s1);
  kin_shape_print(s2);
  return 0;
}

static int bkCollideSegmentSegment(kin_collision_data* data, kin_shape s1, kin_shape s2) {
  printf("bkCollideSegmentSegment\n");
  kin_shape_print(s1);
  kin_shape_print(s2);
  kin_shape_segment ss1 = (kin_shape_segment)s1;
  kin_shape_segment ss2 = (kin_shape_segment)s2;
  kin_vec dl1 = kin_vec_sub(ss1->end_point, ss1->start_point);
  kin_vec dl2 = kin_vec_sub(ss2->end_point, ss2->start_point);
  double llcp = kin_vec_cross(dl2, dl1);
  if(llcp == 0.0) {
    data->num_points = 0;
    return 0;
  }
  kin_vec ds = kin_vec_sub(ss2->start_point, ss1->start_point);
  double lscp = kin_vec_cross(dl1, ds);
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
  kin_vec_copy(&data->points[0], kin_vec_add(ss1->start_point, kin_vec_scale(dl1, t)));
  return 0;
}

static int bkCollideCircleSegment(kin_collision_data* data, kin_shape s1, kin_shape s2) {
  printf("bkCollideCircleSegment\n");
  kin_shape_print(s1);
  kin_shape_print(s2);
  kin_shape_circle c = (kin_shape_circle)s1;
  kin_shape_segment s = (kin_shape_segment)s2;
  kin_vec l = kin_vec_sub(s->end_point, s->start_point);
  kin_vec sc = kin_vec_sub(c->world_center, s->start_point);
  double A = kin_vec_length(l);
  double B = -2.0 * kin_vec_dot(sc, l);
  double C = kin_vec_sq_length(sc) - c->radius * c->radius;
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
    kin_vec_copy(&data->points[0], kin_vec_add(s->start_point, kin_vec_scale(l, t)));
    return 1;      
  }
  data->num_points = 0;
  double t = (-B + D) / (2.0 * A);
  if((t >= 0.0) && (t <= 1.0)) {
    kin_vec_copy(&data->points[data->num_points], kin_vec_add(s->start_point, kin_vec_scale(l, t)));
    ++data->num_points;
  }
  t = (-B - D) / (2.0 * A);
  if((t >= 0.0) && (t <= 1.0)) {
    kin_vec_copy(&data->points[data->num_points], kin_vec_add(s->start_point, kin_vec_scale(l, t)));
    ++data->num_points;
  }   
  return (data->num_points > 0);
}

static int bkCollidePolygonSegment(kin_collision_data* data, kin_shape s1, kin_shape s2) {
  printf("bkCollidePolygonSegment\n");
  kin_shape_print(s1);
  kin_shape_print(s2);
  return 0;
}

typedef int (*bkCollideShapeF)(kin_collision_data*, kin_shape, kin_shape);

static bkCollideShapeF bkCollideDispatch[] = {
   bkCollideError,
   bkCollideCircleCircle,
   bkCollidePolygonPolygon,
   bkCollideCirclePolygon,
   bkCollideSegmentSegment,
   bkCollideCircleSegment,
   bkCollidePolygonSegment,
   bkCollideError
};

int kin_collide_shapes(kin_collision_data* data, kin_shape s1, kin_shape s2) {
  if(s1->type > s2->type) {
    kin_shape tmp = s1;
    s1 = s2;
    s2 = tmp;
  }
  bkCollideDispatch[s1->type | s2->type](data, s1, s2);
  return data->num_points > 0;
}
