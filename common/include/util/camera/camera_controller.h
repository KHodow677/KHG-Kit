#pragma once

#include "util/camera/camera.h"

#define CAM_POSITION_EASING 3.0f
#define CAM_ZOOM_EASING 15.0f
#define CAM_MAX_VELOCITY 1000.0f
#define CAM_MAX_ZOOM_VELOCITY 1.0f

void move_camera(camera *cam, const float delta);

