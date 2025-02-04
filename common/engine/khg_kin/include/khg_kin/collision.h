#pragma once

#include "khg_kin/namespace.h"

int collide_bb(kin_shape shape1, kin_shape shape2);
int collide_shapes(kin_collision_data *data, kin_shape shape1, kin_shape shape2);
