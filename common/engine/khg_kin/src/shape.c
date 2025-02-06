#define NAMESPACE_KIN_IMPL

#include "khg_kin/namespace.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void shape_generic_print(kin_shape s) {
  printf("p_off=<%f,%f> a_off=<%f,%f> ", s->pos_off.x, s->pos_off.y, s->ang_off.x, s->ang_off.y);
}

static void shape_circle_print(kin_shape_circle s) {
  printf("circle ");
  shape_generic_print((kin_shape)s);
  printf("r=%f\n", s->radius);
}

static void shape_polygon_print(kin_shape_polygon s) {
  printf("polygon ");
  shape_generic_print((kin_shape)s);
  printf("n=%u v=", s->num_vertices);
  for(unsigned int i = 0; i < s->num_vertices; ++i) {
    printf("<%f,%f>", s->world_vertices[i].x, s->world_vertices[i].y);
  }
  printf("\n");
}

static void shape_segment_print(kin_shape_segment s) {
  printf("segment ");
  shape_generic_print((kin_shape)s);
  printf("s=<%f,%f> e=<%f,%f>\n", s->start_point.x, s->start_point.y, s->end_point.x, s->end_point.y);
}

kin_shape_circle shape_circle_create(float radius) {
  kin_shape_circle c = calloc(1, sizeof(struct kin_shape_circle_raw));
  c->generic.type = KIN_SHAPE_CIRCLE;
  c->generic.pose_update = NAMESPACE_KIN_INTERNAL.shape_circle_pose_update;
  c->generic.bb_update = NAMESPACE_KIN_INTERNAL.shape_circle_bb_update;
  c->radius = radius;
  return c;
}

kin_shape_polygon shape_polygon_create(unsigned int num_vertices, kin_vec *vertices) {
  kin_shape_polygon p = calloc(1, sizeof(struct kin_shape_polygon_raw));
  p->generic.type = KIN_SHAPE_POLYGON;
  p->generic.pose_update = NAMESPACE_KIN_INTERNAL.shape_polygon_pose_update;
  p->generic.bb_update = NAMESPACE_KIN_INTERNAL.shape_polygon_bb_update;
  p->num_vertices = num_vertices;
  p->world_vertices = calloc(num_vertices, sizeof(kin_vec));
  p->body_vertices = calloc(num_vertices, sizeof(kin_vec));
  memcpy(p->body_vertices, vertices, num_vertices * sizeof(kin_vec));
  return p;
}

kin_shape_segment shape_segment_create(kin_vec start, kin_vec end) {
  kin_shape_segment g = (kin_shape_segment)calloc(1, sizeof(struct kin_shape_segment_raw));
  g->generic.type = KIN_SHAPE_SEGMENT;
  g->generic.pose_update = NAMESPACE_KIN_INTERNAL.shape_segment_pose_update;
  g->generic.bb_update = NAMESPACE_KIN_INTERNAL.shape_segment_bb_update;
  NAMESPACE_KIN_INTERNAL.vec_copy(&g->body_start_point, start);
  NAMESPACE_KIN_INTERNAL.vec_copy(&g->world_end_point, end);
  return g;
}

void shape_deallocate(kin_shape *shape) {
  kin_shape next = (*shape)->next;
  free(*shape);
  *shape = next;
}

void shape_circle_pose_update(kin_shape shape) {
  kin_shape_circle c = (kin_shape_circle)shape;
  NAMESPACE_KIN_INTERNAL.vec_copy(&c->world_center, NAMESPACE_KIN_INTERNAL.local_to_world(shape->body, shape->pos_off));
}

void shape_circle_bb_update(kin_shape shape) {
  kin_shape_circle c = (kin_shape_circle)shape;
  shape->bb.top    = c->world_center.y + c->radius;
  shape->bb.bottom = c->world_center.y - c->radius;
  shape->bb.right  = c->world_center.x + c->radius;
  shape->bb.left   = c->world_center.x - c->radius;
}

void shape_polygon_pose_update(kin_shape shape) {
  kin_shape_polygon p = (kin_shape_polygon)shape;
  for(unsigned int i = 0; i < p->num_vertices; ++i) {
    p->world_vertices[i] = NAMESPACE_KIN_INTERNAL.local_to_world(shape->body, NAMESPACE_KIN_INTERNAL.vec_add(shape->pos_off, NAMESPACE_KIN_INTERNAL.vec_rot(p->body_vertices[i], shape->ang_off)));
  }
}

void shape_polygon_bb_update(kin_shape shape) {
  kin_shape_polygon p = (kin_shape_polygon)shape;
  shape->bb.top    = p->world_vertices[0].y;
  shape->bb.bottom = p->world_vertices[0].y;
  shape->bb.right  = p->world_vertices[0].x;
  shape->bb.left   = p->world_vertices[0].x;
  for(unsigned int i = 1; i < p->num_vertices; i++) {
    shape->bb.top = fmaxf(shape->bb.top, p->world_vertices[i].y);
    shape->bb.bottom = fminf(shape->bb.bottom, p->world_vertices[i].y);
    shape->bb.right = fmaxf(shape->bb.right, p->world_vertices[i].x);
    shape->bb.left = fminf(shape->bb.left, p->world_vertices[i].x);
  }
}

void shape_segment_pose_update(kin_shape shape) {
  kin_shape_segment g = (kin_shape_segment)shape;
  g->start_point = NAMESPACE_KIN_INTERNAL.local_to_world(shape->body, NAMESPACE_KIN_INTERNAL.vec_add(shape->pos_off, NAMESPACE_KIN_INTERNAL.vec_rot(g->body_start_point, shape->ang_off)));
  g->end_point = NAMESPACE_KIN_INTERNAL.local_to_world(shape->body, NAMESPACE_KIN_INTERNAL.vec_add(shape->pos_off, NAMESPACE_KIN_INTERNAL.vec_rot(g->world_end_point, shape->ang_off)));
}

void shape_segment_bb_update(kin_shape shape) {
  kin_shape_segment g = (kin_shape_segment)shape;
  shape->bb.top    = fmaxf(g->start_point.y, g->end_point.y);
  shape->bb.bottom = fminf(g->start_point.y, g->end_point.y);
  shape->bb.right  = fmaxf(g->start_point.x, g->end_point.x);
  shape->bb.left   = fminf(g->start_point.x, g->end_point.x);
}

void shape_print(kin_shape shape) {
  switch(shape->type) {
    case KIN_SHAPE_CIRCLE:
      shape_circle_print((kin_shape_circle)shape);
      break;
    case KIN_SHAPE_POLYGON:
      shape_polygon_print((kin_shape_polygon)shape);
      break;
    case KIN_SHAPE_SEGMENT:
      shape_segment_print((kin_shape_segment)shape);
      break;
    default:
      printf("unknown shape type %u\n", shape->type);
      break;
  }
}
