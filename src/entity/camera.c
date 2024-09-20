#include "entity/camera.h"
#include "khg_phy/vect.h"

float INITIAL_ZOOM = 1.0f;

void camera_setup(camera *cam) {
  cam->position = phy_v(0.0f, 0.0f);
  cam->target = phy_v(0.0f, 0.0f);
  cam->zoom = INITIAL_ZOOM;
  cam->target_zoom = INITIAL_ZOOM;
}
