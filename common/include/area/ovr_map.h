#pragma once

#include "khg_utl/vector.h"
#include <stdbool.h>

typedef struct ovr_map {
  bool enabled;
  utl_vector *tiles;
} ovr_map;

void render_tiles(utl_vector *tiles, int parallax_value);

