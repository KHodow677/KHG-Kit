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

void transform_letterbox_element(gfx_aabb letterbox, phy_vect *pos, phy_vect *cam_pos, gfx_texture *tex) {
  float scale_x = letterbox.size.x / INITIAL_WIDTH;
  float scale_y = letterbox.size.y / INITIAL_HEIGHT;
  tex->width *= scale_x;
  tex->height *= scale_y;
  pos->x = (pos->x) * scale_x + letterbox.pos.x;
  pos->y = (pos->y) * scale_y + letterbox.pos.y;
  cam_pos->x = (cam_pos->x) * scale_x;
  cam_pos->y = (cam_pos->y) * scale_y;
}

void get_letterbox() {
  float current_aspect_ratio = (float)gfx_get_display_width() / (float)gfx_get_display_height();
  float ratio_ratio = current_aspect_ratio / TARGET_ASPECT_RATIO;
  LETTERBOX.pos.x = get_letterbox_x(current_aspect_ratio, ratio_ratio);
  LETTERBOX.pos.y = get_letterbox_y(current_aspect_ratio, ratio_ratio);
  LETTERBOX.size.x = get_letterbox_width(current_aspect_ratio, ratio_ratio);
  LETTERBOX.size.y = get_letterbox_height(current_aspect_ratio, ratio_ratio);
}

