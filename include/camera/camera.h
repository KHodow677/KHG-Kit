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

phy_vect screen_to_world(float screen_x, float screen_y);
phy_vect world_to_screen(float world_x, float world_y);
phy_vect world_to_screen_perc(float world_x, float world_y);

