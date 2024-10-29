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

phy_vect screen_to_world(const float screen_x, const float screen_y);
phy_vect world_to_screen(const float world_x, const float world_y);
phy_vect world_to_screen_perc(const float world_x, const float world_y);

