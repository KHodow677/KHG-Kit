#pragma once

#include "khgmath/vec4.h"
#include "khgutils/bool.h"

typedef struct {
  int x_count;
	int y_count;
} texture_atlas;

vec4 compute_texture_atlas(int x_count, int y_count, int x, int y, bool flip);
vec4 compute_texture_atlas_with_padding(int map_x_size, int map_y_size, int x_count, int y_count, int x, int y, bool flip);
vec4 get_texture_atlas(texture_atlas *ta, int x, int y, bool flip);
