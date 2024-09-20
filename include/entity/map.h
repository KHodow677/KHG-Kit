#pragma once

#include "khg_utl/vector.h"

typedef enum {
  SEGMENT_TOP_LEFT,
  SEGMENT_TOP,
  SEGMENT_TOP_RIGHT,
  SEGMENT_LEFT,
  SEGMENT_NONE,
  SEGMENT_RIGHT,
  SEGMENT_BOTTOM_LEFT,
  SEGMENT_BOTTOM,
  SEGMENT_BOTTOM_RIGHT,
} collision_segment_direction;

void render_map(utl_vector *map);

void add_map_collision_segments(utl_vector *map, utl_vector **segments);
void free_map_collision_segments(utl_vector **segments);

