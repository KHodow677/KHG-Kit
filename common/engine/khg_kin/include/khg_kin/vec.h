#pragma once

#include "khg_kin/namespace.h"

kin_vec kin_vec_create(float x, float y);
void kin_vec_copy(kin_vec *v1, kin_vec v2);

float kin_vec_sq_length(kin_vec v);
float kin_vec_length(kin_vec v);
kin_vec kin_vec_perp(kin_vec v);
kin_vec kin_vec_add(kin_vec v1, kin_vec v2);
kin_vec kin_vec_sub(kin_vec v1, kin_vec v2);

float kin_vec_dot(kin_vec v1, kin_vec v2);
float kin_vec_cross(kin_vec v1, kin_vec v2);

kin_vec kin_vec_scale(kin_vec v, float f);
float kin_vec_dist(kin_vec v1, kin_vec v2);
kin_vec kin_vec_rot(kin_vec v, kin_vec r);
