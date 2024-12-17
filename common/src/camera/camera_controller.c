#include "camera/camera_controller.h"
#include "game.h"
#include "camera/camera.h"
#include "utility/math_utl.h"
#include "khg_gfx/elements.h"
#include "khg_phy/core/phy_vector.h"
#include <math.h>

static float camera_get_value_to_clamp_x(const camera *cam, const float val) {
  const float window_center_x = gfx_get_current_div().aabb.size.x / 2.0f;
  return val - (window_center_x / (window_center_x * cam->zoom)) * window_center_x + window_center_x;
}

static float camera_get_value_to_clamp_y(const camera *cam, const float val) {
  const float window_center_y = gfx_get_current_div().aabb.size.y / 2.0f;
  return val - (window_center_y / (window_center_y * cam->zoom)) * window_center_y + window_center_y;
}

static void camera_move_to_position(camera *cam, const float delta) {
  const float pos_diff = phy_vector2_dist(cam->position, cam->target);
  const float speed = -fminf(fabsf(pos_diff) * CAM_POSITION_EASING, CAM_MAX_VELOCITY) * delta;
  const float target_ang = normalize_angle(atan2f(cam->position.y - cam->target.y, cam->position.x - cam->target.x));
  cam->position = phy_vector2_add(cam->position, phy_vector2_new(speed * cosf(target_ang), speed * sinf(target_ang)));
}

static void camera_zoom_to_value(camera *cam, const float delta) {
  const float zoom_diff = fabsf(cam->zoom - cam->target_zoom);
  const float speed = fminf(fabsf(zoom_diff) * CAM_ZOOM_EASING, CAM_MAX_ZOOM_VELOCITY) * delta;
  cam->zoom = cam->zoom > cam->target_zoom ? cam->zoom - speed : cam->zoom + speed;
}

static void clamp_camera_x(camera *cam, const float range_x_min, const float range_x_max) {
  cam->position.x = fmaxf(cam->position.x, -range_x_min);
  cam->position.x = fminf(cam->position.x, range_x_max - INITIAL_WIDTH);
  cam->target.x = fmaxf(cam->target.x, -range_x_min);
  cam->target.x = fminf(cam->target.x, range_x_max - INITIAL_WIDTH);
}

void move_camera(camera *cam, const float delta) {
  /*if (KEYBOARD_STATE.a_key_is_down) {*/
  /*  cam->target.x -= 1000 * delta;*/
  /*}*/
  /*if (KEYBOARD_STATE.d_key_is_down) {*/
  /*  cam->target.x += 1000 * delta;*/
  /*}*/
  camera_move_to_position(cam, delta);
  camera_zoom_to_value(cam, delta);
  const float range_x_min = camera_get_value_to_clamp_x(cam, 0.0f);
  const float range_x_max = camera_get_value_to_clamp_x(cam, 2560.0f);
  clamp_camera_x(cam, range_x_min, range_x_max);
}

