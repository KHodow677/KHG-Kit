#pragma once

#include "khg_kin/namespace.h"

int kin_collide_bb(kin_shape s1, kin_shape s2);
int kin_collide_shapes(kin_collision_data *data, kin_shape s1, kin_shape s2);
