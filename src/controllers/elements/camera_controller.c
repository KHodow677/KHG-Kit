#include "controllers/elements/camera_controller.h"
#include "controllers/input/key_controllers.h"
#include "data_utl/kinematic_utl.h"
#include "GLFW/glfw3.h"
#include "entity/camera.h"
#include "game_manager.h"
#include "khg_gfx/elements.h"
#include "khg_phy/vect.h"
#include <math.h>

static float CAM_POSITION_TOLERANCE = 1.0f;
static float CAM_ZOOM_TOLERANCE = 0.0001f;
static float CAM_POSITION_EASING = 3.0f;
static float CAM_ZOOM_EASING = 30.0f;
static float CAM_MAX_VELOCITY = 1000.0f;
static float CAM_MAX_ZOOM_VELOCITY = 0.5f;

static void camera_move_to_position(camera *cam, float delta) {
  float pos_diff = phy_v_dist(cam->position, cam->target);
  if (fabsf(pos_diff) < CAM_POSITION_TOLERANCE) {
    cam->position = cam->target;
    return;
  }
  float speed = -fminf(fabsf(pos_diff) * CAM_POSITION_EASING, CAM_MAX_VELOCITY) * delta;
  float target_ang = normalize_angle(atan2f(cam->position.y - cam->target.y, cam->position.x - cam->target.x));
  cam->position = phy_v_add(cam->position, phy_v(speed * cosf(target_ang), speed * sinf(target_ang)));
}

static void camera_zoom_to_value(camera *cam, float delta) {
  float zoom_diff = fabsf(cam->zoom - cam->target_zoom);
  if (fabsf(zoom_diff) < CAM_ZOOM_TOLERANCE) {
    cam->zoom = cam->target_zoom;
    return;
  }
  float speed = fminf(fabsf(zoom_diff) * CAM_ZOOM_EASING, CAM_MAX_ZOOM_VELOCITY) * delta;
  cam->zoom = cam->zoom > cam->target_zoom ? cam->zoom - speed : cam->zoom + speed;
}

static void clamp_camera_x(camera *cam, float range_x) {
  cam->position.x = fmaxf(cam->position.x, -range_x);
  cam->position.x = fminf(cam->position.x, range_x - gfx_get_display_width());
  cam->target.x = fmaxf(cam->target.x, -range_x);
  cam->target.x = fminf(cam->target.x, range_x - gfx_get_display_width());
}

static void clamp_camera_y(camera *cam, float range_y) {
  cam->position.y = fmaxf(cam->position.y, -range_y);
  cam->position.y = fminf(cam->position.y, range_y - gfx_get_display_height());
  cam->target.y = fmaxf(cam->target.y, -range_y);
  cam->target.y = fminf(cam->target.y, range_y - gfx_get_display_height());
}

static void clamp_camera(camera *cam, float range_x, float range_y) {
  clamp_camera_x(cam, range_x);
  clamp_camera_y(cam, range_y);
}

void move_camera(camera *cam, float delta) {
  if (handle_key_button_is_down(GLFW_KEY_A)) {
    cam->target.x -= 1000 * delta;
  }
  if (handle_key_button_is_down(GLFW_KEY_D)) {
    cam->target.x += 1000 * delta;
  }
  if (handle_key_button_is_down(GLFW_KEY_W)) {
    cam->target.y -= 1000 * delta;
  }
  if (handle_key_button_is_down(GLFW_KEY_S)) {
    cam->target.y += 1000 * delta;
  }
  if (handle_key_button_went_down(GLFW_KEY_Q)) {
    cam->target_zoom = fminf(cam->target_zoom + 0.25f * INITIAL_ZOOM, 2.0f * INITIAL_ZOOM);
  }
  if (handle_key_button_went_down(GLFW_KEY_E)) {
    cam->target_zoom = fmaxf(cam->target_zoom - 0.25f * INITIAL_ZOOM, 0.5f * INITIAL_ZOOM);
  }
  camera_move_to_position(cam, delta);
  camera_zoom_to_value(cam, delta);
  float window_center_x = gfx_get_display_width() / 2.0f;
  float window_center_y = gfx_get_display_height() / 2.0f;
  float range_x = (GAME_MAP_TILE_SIZE * GAME_MAP_SIZE * 0.5f) - (window_center_x / (window_center_x * cam->zoom)) * window_center_x + window_center_x;
  float range_y = (GAME_MAP_TILE_SIZE * GAME_MAP_SIZE * 0.5f) - (window_center_y / (window_center_y * cam->zoom)) * window_center_y + window_center_y;
  clamp_camera(cam, range_x, range_y);
}

