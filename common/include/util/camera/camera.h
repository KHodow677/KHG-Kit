#pragma once

#include "khg_kin/namespace.h"

typedef struct camera {
  kin_vec position;
  kin_vec target;
  float zoom;
  float target_zoom;
} camera;

extern camera CAMERA;
extern float INITIAL_ZOOM;

void camera_setup(camera *cam);

const kin_vec screen_to_world(const float screen_x, const float screen_y);
const kin_vec world_to_screen(const float world_x, const float world_y);
const kin_vec world_to_screen_perc(const float world_x, const float world_y);

