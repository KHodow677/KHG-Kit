#pragma once

#include "khg_phy/phy_types.h"

typedef struct camera {
  phy_vect position;
  phy_vect target;
  float zoom;
  float target_zoom;
} camera;

extern camera CAMERA;
extern float INITIAL_ZOOM;

void camera_setup(camera *cam);

