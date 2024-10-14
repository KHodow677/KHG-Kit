#include "letterbox.h"
#include "khg_gfx/elements.h"

static float target_aspect_ratio = 16.0f / 9.0f;

static float get_letterbox_x() {
  float current_aspect_ratio = (float)gfx_get_display_width() / (float)gfx_get_display_height();
  float ratio_ratio = current_aspect_ratio / target_aspect_ratio;
  return current_aspect_ratio > target_aspect_ratio ? (gfx_get_display_width() - gfx_get_display_width() / ratio_ratio) / 2.0f : 0;
}

static float get_letterbox_y() {
  float current_aspect_ratio = (float)gfx_get_display_width() / (float)gfx_get_display_height();
  float ratio_ratio = current_aspect_ratio / target_aspect_ratio;
  return current_aspect_ratio < target_aspect_ratio ? (gfx_get_display_height() - gfx_get_display_height() * ratio_ratio) / 2.0f : 0;
}

static float get_letterbox_width() {
  float current_aspect_ratio = (float)gfx_get_display_width() / (float)gfx_get_display_height();
  float ratio_ratio = current_aspect_ratio / target_aspect_ratio;
  return current_aspect_ratio > target_aspect_ratio ? gfx_get_display_width() / ratio_ratio : gfx_get_display_width();
}

static float get_letterbox_height() {
  float current_aspect_ratio = (float)gfx_get_display_width() / (float)gfx_get_display_height();
  float ratio_ratio = current_aspect_ratio / target_aspect_ratio;
  return current_aspect_ratio < target_aspect_ratio ? gfx_get_display_height() * ratio_ratio : gfx_get_display_height();
}

gfx_aabb get_letterbox(void) {
  gfx_aabb letterbox;
  letterbox.pos.x = get_letterbox_x();
  letterbox.pos.y = get_letterbox_y();
  letterbox.size.x = get_letterbox_width();
  letterbox.size.y = get_letterbox_height();
  return letterbox;
}
