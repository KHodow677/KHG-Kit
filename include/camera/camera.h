#pragma once

#include "khg_phy/vector.h"

typedef struct camera {
  nvVector2 position;
  nvVector2 target;
  float zoom;
  float target_zoom;
} camera;

extern camera CAMERA;
extern float INITIAL_ZOOM;

void camera_setup(camera *cam);

const nvVector2 screen_to_world(const float screen_x, const float screen_y);
const nvVector2 world_to_screen(const float world_x, const float world_y);
const nvVector2 world_to_screen_perc(const float world_x, const float world_y);

