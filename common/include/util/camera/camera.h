#pragma once

#include "khg_phy/core/phy_vector.h"

typedef struct camera {
  phy_vector2 position;
  phy_vector2 target;
  float zoom;
  float target_zoom;
} camera;

extern camera CAMERA;
extern float INITIAL_ZOOM;

void camera_setup(camera *cam);

const phy_vector2 screen_to_world(const float screen_x, const float screen_y);
const phy_vector2 world_to_screen(const float world_x, const float world_y);
const phy_vector2 world_to_screen_perc(const float world_x, const float world_y);

