#pragma once

#include "khg_kin/namespace.h"

kin_shape_circle kin_shape_circle_create(float r);
kin_shape_polygon kin_shape_polygon_create(unsigned int n, kin_vec *v);
kin_shape_segment kin_shape_segment_create(kin_vec s, kin_vec e);
void kin_shape_deallocate(kin_shape *s);

void kin_shape_circle_pose_update(kin_shape s);
void kin_shape_circle_bb_update(kin_shape s);
void kin_shape_polygon_pose_update(kin_shape s);
void kin_shape_polygon_bb_update(kin_shape s);
void kin_shape_segment_pose_update(kin_shape s);
void kin_shape_segment_bb_update(kin_shape s);

void kin_shape_print(kin_shape s);
