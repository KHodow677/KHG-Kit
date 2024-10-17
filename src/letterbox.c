#include "letterbox.h"
#include "game.h"
#include "khg_gfx/elements.h"
#include "khg_phy/phy_types.h"

static float TARGET_ASPECT_RATIO = 16.0f / 9.0f;
gfx_aabb LETTERBOX = { 0 };

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

static void set_letterbox_camera(gfx_aabb letterbox) {
  /*CAMERA.position = phy_v(letterbox.pos.x, letterbox.pos.y);*/
  /*CAMERA.target = phy_v(letterbox.pos.x, letterbox.pos.y);*/
  /*CAMERA.zoom = fminf(gfx_get_display_width() / letterbox.size.x, gfx_get_display_height() / letterbox.size.y);*/
  /*CAMERA.target_zoom = fminf(gfx_get_display_width() / letterbox.size.x, gfx_get_display_height() / letterbox.size.y);*/
}

void transform_letterbox_element(gfx_aabb letterbox, phy_vect *pos, gfx_texture *tex, float offset_x, float offset_y) {
  float scale = letterbox.size.x / INITIAL_WIDTH;
  float window_center_x = gfx_get_display_width() / 2.0f;
  float window_center_y = gfx_get_display_height() / 2.0f;
  tex->width *= scale;
  tex->height *= scale;
  // pos->x = (pos->x - window_center_x) * scale + window_center_x + letterbox.pos.x;
  // pos->y = (pos->y - window_center_y) * scale + window_center_y + letterbox.pos.y;
}

void get_letterbox() {
  float current_aspect_ratio = (float)gfx_get_display_width() / (float)gfx_get_display_height();
  float ratio_ratio = current_aspect_ratio / TARGET_ASPECT_RATIO;
  LETTERBOX.pos.x = get_letterbox_x(current_aspect_ratio, ratio_ratio);
  LETTERBOX.pos.y = get_letterbox_y(current_aspect_ratio, ratio_ratio);
  LETTERBOX.size.x = get_letterbox_width(current_aspect_ratio, ratio_ratio);
  LETTERBOX.size.y = get_letterbox_height(current_aspect_ratio, ratio_ratio);
  set_letterbox_camera(LETTERBOX);
}
