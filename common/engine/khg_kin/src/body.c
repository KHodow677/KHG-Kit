#include "khg_kin/body.h"
#include "khg_kin/shape.h"
#include "khg_kin/vec.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

kin_body kin_body_create() {
  return calloc(1, sizeof(struct kin_body_raw));
}

void kin_body_deallocate(kin_body* b) {
  kin_body next = (*b)->next;
  free(*b);
  *b = next;
}

void kin_body_step(kin_body b, float dt) {
  b->lin_vel = kin_vec_add(b->lin_vel, kin_vec_scale(b->force, dt / b->mass));
  b->pos = kin_vec_add(b->pos, kin_vec_scale(b->lin_vel, dt));
  b->ang_vel += b->torque / b->inertia * dt;
  b->rot_scalar += b->ang_vel * dt;
  b->rot_vector = kin_vec_create(cos(b->rot_scalar), sin(b->rot_scalar));
  for(kin_shape s = b->shapes; s != 0; s = s->next) {
    s->pose_update(s);
    s->bb_update(s);
  }
}

void kin_body_reset(kin_body b) {
  b->force = kin_vec_create(0.0, 0.0);
  b->torque = 0.0;
}

void kin_body_force_set(kin_body b, kin_vec f) {
  b->force = f;
}

void kin_body_torque_set(kin_body b, float t) {
  b->torque = t;
}

void kin_body_force_add(kin_body b, kin_vec f) {
  b->force = kin_vec_add(b->force, f);
}

void kin_body_torque_add(kin_body b, float t) {
  b->torque += t;
}

void kin_body_shape_add(kin_body b, kin_shape s, kin_vec p_off, kin_vec a_off) {
  s->body = b;
  s->next = b->shapes;
  b->shapes = s;
  s->pos_off = p_off;
  s->ang_off = a_off;
}

void kin_body_shape_remove(kin_body b, kin_shape s) {
  kin_shape* cur = &(b->shapes);
  while((*cur) && (*cur != s)) cur = &((*cur)->next);
  kin_shape_deallocate(cur);
}

kin_vec kin_world_to_local(kin_body b, kin_vec v) {
  return kin_vec_create(b->rot_vector.x * (v.x - b->pos.x) + b->rot_vector.y * (v.y - b->pos.y), -b->rot_vector.y * (v.x - b->pos.x) + b->rot_vector.x * (v.y - b->pos.y));
}

kin_vec kin_local_to_world(kin_body b, kin_vec v) {
  return kin_vec_create(b->rot_vector.x * v.x - b->rot_vector.y * v.y + b->pos.x, b->rot_vector.y * v.x + b->rot_vector.x * v.y + b->pos.y);
}

void kin_body_print(kin_body b) {
  printf("body p=<%f,%f>, v=<%f,%f>, f=<%f,%f>, r=%f, w=%f, t=%f\n", b->pos.x, b->pos.y, b->lin_vel.x, b->lin_vel.y, b->force.x, b->force.y, b->rot_scalar, b->ang_vel, b->torque);
  for(kin_shape s = b->shapes; s; s = s->next) {
    kin_shape_print(s);
  }
  printf("\n");
}
