#pragma once

#include "khg_phy/core/phy_vector.h"
#include "khg_utl/vector.h"

typedef struct ovr_unit {
  unsigned int unit_id;
  phy_vector2 tile_coord;
  utl_vector *source_coords;
  utl_vector *sink_coords;
  bool flipped;
} ovr_unit;

