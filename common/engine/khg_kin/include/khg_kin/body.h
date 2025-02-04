#pragma once

#include "khg_kin/namespace.h"

kin_body kin_body_create(void);
void kin_body_deallocate(kin_body *b);

void kin_body_step(kin_body b, float dt);

void kin_body_reset(kin_body b);
void kin_body_force_set(kin_body b, kin_vec f);
void kin_body_torque_set(kin_body b, float t);

void kin_body_force_add(kin_body b, kin_vec f);
void kin_body_torque_add(kin_body b, float t);

void kin_body_shape_add(kin_body b, kin_shape s, kin_vec p_off, kin_vec a_off);
void kin_body_shape_remove(kin_body b, kin_shape s);

kin_vec kin_world_to_local(kin_body b, kin_vec v);
kin_vec kin_local_to_world(kin_body b, kin_vec v);

void kin_body_print(kin_body b);
