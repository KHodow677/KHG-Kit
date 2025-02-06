#pragma once

#include "khg_kin/namespace.h"

kin_body body_create(void);
void body_deallocate(kin_body *body);

void body_step(kin_body body, float dt);

void body_reset(kin_body body);
void body_force_set(kin_body body, kin_vec force);
void body_torque_set(kin_body body, float torque);

void body_force_add(kin_body body, kin_vec force);
void body_torque_add(kin_body body, float torque);

void body_shape_add(kin_body body, kin_shape shape, kin_vec pos_off, kin_vec ang_off);
void body_shape_remove(kin_body body, kin_shape shape);

void body_print(kin_body body);

kin_vec world_to_local(kin_body body, kin_vec vec);
kin_vec local_to_world(kin_body body, kin_vec vec);
