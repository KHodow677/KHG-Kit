#pragma once

#include "khg_phy/phy_types.h"

typedef struct camera {
  phy_vect position;
  float zoom;
} camera;

void camera_setup(camera *cam);

