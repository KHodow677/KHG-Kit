#include "controllers/elements/camera_controller.h"
#include "controllers/input/key_controllers.h"
#include "GLFW/glfw3.h"

void move_camera(camera *cam, float delta) {
  if (handle_key_button_is_down(GLFW_KEY_A)) {
    cam->position.x -= 1000 * delta;
  }
  if (handle_key_button_is_down(GLFW_KEY_D)) {
    cam->position.x += 1000 * delta;
  }
  if (handle_key_button_is_down(GLFW_KEY_W)) {
    cam->position.y -= 1000 * delta;
  }
  if (handle_key_button_is_down(GLFW_KEY_S)) {
    cam->position.y += 1000 * delta;
  }
}
