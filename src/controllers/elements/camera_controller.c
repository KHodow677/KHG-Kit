#include "controllers/elements/camera_controller.h"
#include "controllers/input/key_controllers.h"
#include "GLFW/glfw3.h"
#include "game_manager.h"
#include "khg_gfx/elements.h"
#include <math.h>

static void clamp_camera_x(camera *cam, float range_x) {
  cam->position.x = fmaxf(cam->position.x, -range_x);
  cam->position.x = fminf(cam->position.x, range_x - gfx_get_display_width());
}

static void clamp_camera_y(camera *cam, float range_y) {
  cam->position.y = fmaxf(cam->position.y, -range_y);
  cam->position.y = fminf(cam->position.y, range_y - gfx_get_display_height());
}

static void clamp_camera(camera *cam, float range_x, float range_y) {
  clamp_camera_x(cam, range_x);
  clamp_camera_y(cam, range_y);
}

void move_camera(camera *cam, float delta) {
  float window_center_x = gfx_get_display_width() / 2.0f;
  float window_center_y = gfx_get_display_height() / 2.0f;
  float range_x = (GAME_MAP_TILE_SIZE * GAME_MAP_SIZE * 0.5f) - (window_center_x / (window_center_x * cam->zoom)) * window_center_x + window_center_x;
  float range_y = (GAME_MAP_TILE_SIZE * GAME_MAP_SIZE * 0.5f) - (window_center_y / (window_center_y * cam->zoom)) * window_center_y + window_center_y;
  if (handle_key_button_is_down(GLFW_KEY_A)) {
    cam->position.x -= 1000 * delta;
    clamp_camera_x(cam, range_x);
  }
  if (handle_key_button_is_down(GLFW_KEY_D)) {
    cam->position.x += 1000 * delta;
    clamp_camera_x(cam, range_x);
  }
  if (handle_key_button_is_down(GLFW_KEY_W)) {
    cam->position.y -= 1000 * delta;
    clamp_camera_y(cam, range_y);
  }
  if (handle_key_button_is_down(GLFW_KEY_S)) {
    cam->position.y += 1000 * delta;
    clamp_camera_y(cam, range_y);
  }
  if (handle_key_button_is_down(GLFW_KEY_Q)) {
    cam->zoom = fminf(cam->zoom + delta, 4.0f);
    clamp_camera(cam, range_x, range_y);
  }
  if (handle_key_button_is_down(GLFW_KEY_E)) {
    cam->zoom = fmaxf(cam->zoom - delta, 0.25f);
    clamp_camera(cam, range_x, range_y);
  }
}
