#pragma once

#include "khg_phy/phy_types.h"

extern float INITIAL_ZOOM;

typedef struct camera {
  bool move_enabled;
  phy_vect position;
  phy_vect target;
  float zoom;
  float target_zoom;
} camera;

void camera_setup(camera *cam, bool real_game, float x, float y);

