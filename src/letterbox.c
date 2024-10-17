#include "letterbox.h"
#include "camera/camera.h"
#include "khg_gfx/elements.h"

static float TARGET_ASPECT_RATIO = 16.0f / 9.0f;

static float get_letterbox_x(float current_aspect_ratio, float ratio_ratio) {
  return current_aspect_ratio > TARGET_ASPECT_RATIO ? (gfx_get_display_width() - gfx_get_display_width() / ratio_ratio) / 2.0f : 0;
}

static float get_letterbox_y(float current_aspect_ratio, float ratio_ratio) {
  return current_aspect_ratio < TARGET_ASPECT_RATIO ? (gfx_get_display_height() - gfx_get_display_height() * ratio_ratio) / 2.0f : 0;
}

static float get_letterbox_width(float current_aspect_ratio, float ratio_ratio) {
  return current_aspect_ratio > TARGET_ASPECT_RATIO ? gfx_get_display_width() / ratio_ratio : gfx_get_display_width();
}

static float get_letterbox_height(float current_aspect_ratio, float ratio_ratio) {
  return current_aspect_ratio < TARGET_ASPECT_RATIO ? gfx_get_display_height() * ratio_ratio : gfx_get_display_height();
}

static void set_letterbox_zoom(float ratio_ratio) {
  CAMERA.zoom = INITIAL_ZOOM * ratio_ratio;
  CAMERA.target_zoom = INITIAL_ZOOM * ratio_ratio;
}

gfx_aabb get_letterbox(void) {
  gfx_aabb letterbox;
  float current_aspect_ratio = (float)gfx_get_display_width() / (float)gfx_get_display_height();
  float ratio_ratio = current_aspect_ratio / TARGET_ASPECT_RATIO;
  set_letterbox_zoom(ratio_ratio);
  letterbox.pos.x = get_letterbox_x(current_aspect_ratio, ratio_ratio);
  letterbox.pos.y = get_letterbox_y(current_aspect_ratio, ratio_ratio);
  letterbox.size.x = get_letterbox_width(current_aspect_ratio, ratio_ratio);
  letterbox.size.y = get_letterbox_height(current_aspect_ratio, ratio_ratio);
  return letterbox;
}

