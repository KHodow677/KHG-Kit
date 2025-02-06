#define NAMESPACE_KIN_IMPL

#include "khg_kin/namespace.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

kin_body body_create() {
  return calloc(1, sizeof(struct kin_body_raw));
}

void body_deallocate(kin_body *body) {
  kin_body next = (*body)->next;
  free(*body);
  *body = next;
}

void body_step(kin_body body, float dt) {
  body->lin_vel = NAMESPACE_KIN_INTERNAL.vec_add(body->lin_vel, NAMESPACE_KIN_INTERNAL.vec_scale(body->force, dt / body->mass));
  body->pos = NAMESPACE_KIN_INTERNAL.vec_add(body->pos, NAMESPACE_KIN_INTERNAL.vec_scale(body->lin_vel, dt));
  body->ang_vel += body->torque / body->inertia * dt;
  body->rot_scalar += body->ang_vel * dt;
  body->rot_vector = NAMESPACE_KIN_INTERNAL.vec_create(cos(body->rot_scalar), sin(body->rot_scalar));
  for(kin_shape s = body->shapes; s != 0; s = s->next) {
    s->pose_update(s);
    s->bb_update(s);
  }
}

void body_reset(kin_body body) {
  body->force = NAMESPACE_KIN_INTERNAL.vec_create(0.0, 0.0);
  body->torque = 0.0;
}

void body_force_set(kin_body body, kin_vec force) {
  body->force = force;
}

void body_torque_set(kin_body body, float torque) {
  body->torque = torque;
}

void body_force_add(kin_body body, kin_vec force) {
  body->force = NAMESPACE_KIN_INTERNAL.vec_add(body->force, force);
}

void body_torque_add(kin_body body, float torque) {
  body->torque += torque;
}

void body_shape_add(kin_body body, kin_shape shape, kin_vec pos_off, kin_vec ang_off) {
  shape->body = body;
  shape->next = body->shapes;
  body->shapes = shape;
  shape->pos_off = pos_off;
  shape->ang_off = ang_off;
}

void body_shape_remove(kin_body body, kin_shape shape) {
  kin_shape* cur = &(body->shapes);
  while((*cur) && (*cur != shape)) cur = &((*cur)->next);
  NAMESPACE_KIN_INTERNAL.shape_deallocate(cur);
}

void body_print(kin_body body) {
  printf("body p=<%f,%f>, v=<%f,%f>, f=<%f,%f>, r=%f, w=%f, t=%f\n", body->pos.x, body->pos.y, body->lin_vel.x, body->lin_vel.y, body->force.x, body->force.y, body->rot_scalar, body->ang_vel, body->torque);
  for(kin_shape s = body->shapes; s; s = s->next) {
    NAMESPACE_KIN_INTERNAL.shape_print(s);
  }
  printf("\n");
}

kin_vec world_to_local(kin_body body, kin_vec vec) {
  return NAMESPACE_KIN_INTERNAL.vec_create(body->rot_vector.x * (vec.x - body->pos.x) + body->rot_vector.y * (vec.y - body->pos.y), -body->rot_vector.y * (vec.x - body->pos.x) + body->rot_vector.x * (vec.y - body->pos.y));
}

kin_vec local_to_world(kin_body body, kin_vec vec) {
  return NAMESPACE_KIN_INTERNAL.vec_create(body->rot_vector.x * vec.x - body->rot_vector.y * vec.y + body->pos.x, body->rot_vector.y * vec.x + body->rot_vector.x * vec.y + body->pos.y);
}
