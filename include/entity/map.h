#pragma once

#include "khg_utl/vector.h"

void render_map(utl_vector *map);

void add_map_collision_segments(utl_vector *map, utl_vector **segments);
void free_map_collision_segments(utl_vector **segments);

