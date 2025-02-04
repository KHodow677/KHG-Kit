#pragma once

#include "khg_kin/namespace.h"

kin_vec vec_create(float x, float y);
void vec_copy(kin_vec *vec1, kin_vec vec2);

float vec_sq_length(kin_vec vec);
float vec_length(kin_vec vec);
kin_vec vec_perp(kin_vec vec);
kin_vec vec_add(kin_vec vec1, kin_vec vec2);
kin_vec vec_sub(kin_vec vec1, kin_vec vec2);

float vec_dot(kin_vec vec1, kin_vec vec2);
float vec_cross(kin_vec vec1, kin_vec vec2);

kin_vec vec_scale(kin_vec vec, float scale);
float vec_dist(kin_vec vec1, kin_vec vec2);
kin_vec vec_rot(kin_vec vec, kin_vec ang);
