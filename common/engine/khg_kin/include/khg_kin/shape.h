#pragma once

#include "khg_kin/namespace.h"

kin_shape_circle shape_circle_create(float radius);
kin_shape_polygon shape_polygon_create(unsigned int num_vertices, kin_vec *vertices);
kin_shape_segment shape_segment_create(kin_vec start, kin_vec end);
void shape_deallocate(kin_shape *shape);

void shape_circle_pose_update(kin_shape shape);
void shape_circle_bb_update(kin_shape shape);
void shape_polygon_pose_update(kin_shape shape);
void shape_polygon_bb_update(kin_shape shape);
void shape_segment_pose_update(kin_shape shape);
void shape_segment_bb_update(kin_shape shape);

void shape_print(kin_shape shape);
