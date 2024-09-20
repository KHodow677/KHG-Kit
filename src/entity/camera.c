#include "entity/camera.h"
#include "khg_phy/vect.h"

void camera_setup(camera *cam) {
  cam->position = phy_v(0.0f, 0.0f);
  cam->zoom = 1.0f;
  cam->speed = (cam->zoom * 1.0f) * 1000.0f;
}
