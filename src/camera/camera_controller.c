#include "camera/camera_controller.h"
#include "camera/camera.h"
#include "io/key_controller.h"
#include "utility/math_utl.h"
#include "khg_gfx/elements.h"
#include "khg_phy/vect.h"
#include <math.h>

static float CAM_POSITION_EASING = 3.0f;
static float CAM_ZOOM_EASING = 15.0f;
static float CAM_MAX_VELOCITY = 1000.0f;
static float CAM_MAX_ZOOM_VELOCITY = 1.0f;

static void camera_move_to_position(camera *cam, const float delta) {
  float pos_diff = phy_v_dist(cam->position, cam->target);
  float speed = -fminf(fabsf(pos_diff) * CAM_POSITION_EASING, CAM_MAX_VELOCITY) * delta;
  float target_ang = normalize_angle(atan2f(cam->position.y - cam->target.y, cam->position.x - cam->target.x));
  cam->position = phy_v_add(cam->position, phy_v(speed * cosf(target_ang), speed * sinf(target_ang)));
}

static void camera_zoom_to_value(camera *cam, const float delta) {
  float zoom_diff = fabsf(cam->zoom - cam->target_zoom);
  float speed = fminf(fabsf(zoom_diff) * CAM_ZOOM_EASING, CAM_MAX_ZOOM_VELOCITY) * delta;
  cam->zoom = cam->zoom > cam->target_zoom ? cam->zoom - speed : cam->zoom + speed;
}

static void clamp_camera_x(camera *cam, const float range_x) {
  cam->position.x = fmaxf(cam->position.x, -range_x);
  cam->position.x = fminf(cam->position.x, range_x - gfx_get_current_div().aabb.size.x);
  cam->target.x = fmaxf(cam->target.x, -range_x);
  cam->target.x = fminf(cam->target.x, range_x - gfx_get_current_div().aabb.size.x);
}

static void clamp_camera_y(camera *cam, const float range_y) {
  cam->position.y = fmaxf(cam->position.y, -range_y);
  cam->position.y = fminf(cam->position.y, range_y - gfx_get_current_div().aabb.size.y);
  cam->target.y = fmaxf(cam->target.y, -range_y);
  cam->target.y = fminf(cam->target.y, range_y - gfx_get_current_div().aabb.size.y);
}

static void clamp_camera(camera *cam, const float range_x, const float range_y) {
  clamp_camera_x(cam, range_x);
  clamp_camera_y(cam, range_y);
}

void move_camera(camera *cam, const float delta) {
  if (KEYBOARD_STATE.a_key_is_down) {
    cam->target.x -= 1000 * delta;
  }
  if (KEYBOARD_STATE.d_key_is_down) {
    cam->target.x += 1000 * delta;
  }
  if (KEYBOARD_STATE.w_key_is_down) {
    cam->target.y -= 1000 * delta;
  }
  if (KEYBOARD_STATE.s_key_is_down) {
    cam->target.y += 1000 * delta;
  }
  camera_move_to_position(cam, delta);
  camera_zoom_to_value(cam, delta);
  float window_center_x = gfx_get_current_div().aabb.size.x / 2.0f;
  float window_center_y = gfx_get_current_div().aabb.size.y / 2.0f;
  float range_x = 4096.0f - (window_center_x / (window_center_x * cam->zoom)) * window_center_x + window_center_x;
  float range_y = 4096.0f - (window_center_y / (window_center_y * cam->zoom)) * window_center_y + window_center_y;
  clamp_camera(cam, range_x, range_y);
}

