#include "khg_kin/body.h"
#include "khg_kin/shape.h"
#include "khg_kin/vec.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void bkShapeGenericPrint(kin_shape s) {
  printf("p_off=<%f,%f> a_off=<%f,%f> ", s->pos_off.x, s->pos_off.y, s->ang_off.x, s->ang_off.y);
}

static void bkShapeCircleprint(kin_shape_circle s) {
  printf("circle ");
  bkShapeGenericPrint((kin_shape)s);
  printf("r=%f\n", s->radius);
}

static void bkShapePolygonprint(kin_shape_polygon s) {
  printf("polygon ");
  bkShapeGenericPrint((kin_shape)s);
  printf("n=%u v=", s->num_vertices);
  for(unsigned int i = 0; i < s->num_vertices; ++i) {
    printf("<%f,%f>", s->world_vertices[i].x, s->world_vertices[i].y);
  }
  printf("\n");
}

static void bkShapeSegmentPrint(kin_shape_segment s) {
  printf("segment ");
  bkShapeGenericPrint((kin_shape)s);
  printf("s=<%f,%f> e=<%f,%f>\n", s->start_point.x, s->start_point.y, s->end_point.x, s->end_point.y);
}

kin_shape_circle kin_shape_circle_create(float r) {
  kin_shape_circle c = calloc(1, sizeof(struct kin_shape_circle_raw));
  c->generic.type = KIN_SHAPE_CIRCLE;
  c->generic.pose_update = kin_shape_circle_pose_update;
  c->generic.bb_update = kin_shape_circle_bb_update;
  c->radius = r;
  return c;
}

kin_shape_polygon kin_shape_polygon_create(unsigned int n, kin_vec *v) {
  kin_shape_polygon p = calloc(1, sizeof(struct kin_shape_polygon_raw));
  p->generic.type = KIN_SHAPE_POLYGON;
  p->generic.pose_update = kin_shape_polygon_pose_update;
  p->generic.bb_update = kin_shape_polygon_bb_update;
  p->num_vertices = n;
  p->world_vertices = calloc(n, sizeof(kin_vec));
  p->body_vertices = calloc(n, sizeof(kin_vec));
  memcpy(p->body_vertices, v, n * sizeof(kin_vec));
  return p;
}

kin_shape_segment kin_shape_segment_create(kin_vec s, kin_vec e) {
  kin_shape_segment g = (kin_shape_segment)calloc(1, sizeof(struct kin_shape_segment_raw));
  g->generic.type = KIN_SHAPE_SEGMENT;
  g->generic.pose_update = kin_shape_segment_pose_update;
  g->generic.bb_update = kin_shape_segment_bb_update;
  kin_vec_copy(&g->body_start_point, s);
  kin_vec_copy(&g->world_end_point, e);
  return g;
}

void kin_shape_deallocate(kin_shape* s) {
  kin_shape next = (*s)->next;
  free(*s);
  *s = next;
}

void kin_shape_circle_pose_update(kin_shape s) {
  kin_shape_circle c = (kin_shape_circle)s;
  kin_vec_copy(&c->world_center, kin_local_to_world(s->body, s->pos_off));
}

void kin_shape_circle_bb_update(kin_shape s) {
  kin_shape_circle c = (kin_shape_circle)s;
  s->bb.top    = c->world_center.y + c->radius;
  s->bb.bottom = c->world_center.y - c->radius;
  s->bb.right  = c->world_center.x + c->radius;
  s->bb.left   = c->world_center.x - c->radius;
}

void kin_shape_polygon_pose_update(kin_shape s) {
  kin_shape_polygon p = (kin_shape_polygon)s;
  for(unsigned int i = 0; i < p->num_vertices; ++i) {
    p->world_vertices[i] = kin_local_to_world(s->body, kin_vec_add(s->pos_off, kin_vec_rot(p->body_vertices[i], s->ang_off)));
  }
}

void kin_shape_polygon_bb_update(kin_shape s) {
  kin_shape_polygon p = (kin_shape_polygon)s;
  s->bb.top    = p->world_vertices[0].y;
  s->bb.bottom = p->world_vertices[0].y;
  s->bb.right  = p->world_vertices[0].x;
  s->bb.left   = p->world_vertices[0].x;
  for(unsigned int i = 1; i < p->num_vertices; i++) {
    s->bb.top = fmaxf(s->bb.top, p->world_vertices[i].y);
    s->bb.bottom = fminf(s->bb.bottom, p->world_vertices[i].y);
    s->bb.right = fmaxf(s->bb.right, p->world_vertices[i].x);
    s->bb.left = fminf(s->bb.left, p->world_vertices[i].x);
  }
}

void kin_shape_segment_pose_update(kin_shape s) {
  kin_shape_segment g = (kin_shape_segment)s;
  g->start_point = kin_local_to_world(s->body, kin_vec_add(s->pos_off, kin_vec_rot(g->body_start_point, s->ang_off)));
  g->end_point = kin_local_to_world(s->body, kin_vec_add(s->pos_off, kin_vec_rot(g->world_end_point, s->ang_off)));
}

void kin_shape_segment_bb_update(kin_shape s) {
  kin_shape_segment g = (kin_shape_segment)s;
  s->bb.top    = fmaxf(g->start_point.y, g->end_point.y);
  s->bb.bottom = fminf(g->start_point.y, g->end_point.y);
  s->bb.right  = fmaxf(g->start_point.x, g->end_point.x);
  s->bb.left   = fminf(g->start_point.x, g->end_point.x);
}

void kin_shape_print(kin_shape s) {
  switch(s->type) {
    case KIN_SHAPE_CIRCLE:
      bkShapeCircleprint((kin_shape_circle)s);
      break;
    case KIN_SHAPE_POLYGON:
      bkShapePolygonprint((kin_shape_polygon)s);
      break;
    case KIN_SHAPE_SEGMENT:
      bkShapeSegmentPrint((kin_shape_segment)s);
      break;
    default:
      printf("unknown shape type %u\n", s->type);
      break;
  }
}
