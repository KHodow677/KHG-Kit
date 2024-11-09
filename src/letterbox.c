#include "letterbox.h"
#include "game.h"
#include "khg_gfx/elements.h"

static const float TARGET_ASPECT_RATIO = 16.0f / 9.0f;
gfx_aabb LETTERBOX = { 0 };

static const float get_letterbox_x(const float current_aspect_ratio, const float ratio_ratio) {
  return current_aspect_ratio > TARGET_ASPECT_RATIO ? (gfx_get_display_width() - gfx_get_display_width() / ratio_ratio) / 2.0f : 0;
}

static const float get_letterbox_y(const float current_aspect_ratio, const float ratio_ratio) {
  return current_aspect_ratio < TARGET_ASPECT_RATIO ? (gfx_get_display_height() - gfx_get_display_height() * ratio_ratio) / 2.0f : 0;
}

static const float get_letterbox_width(const float current_aspect_ratio, const float ratio_ratio) {
  return current_aspect_ratio > TARGET_ASPECT_RATIO ? gfx_get_display_width() / ratio_ratio : gfx_get_display_width();
}

static const float get_letterbox_height(const float current_aspect_ratio, const float ratio_ratio) {
  return current_aspect_ratio < TARGET_ASPECT_RATIO ? gfx_get_display_height() * ratio_ratio : gfx_get_display_height();
}

void transform_letterbox_element(const gfx_aabb letterbox, nvVector2 *pos, nvVector2 *cam_pos, gfx_texture *tex) {
  const float scale_x = letterbox.size.x / INITIAL_WIDTH;
  const float scale_y = letterbox.size.y / INITIAL_HEIGHT;
  tex->width *= scale_x;
  tex->height *= scale_y;
  pos->x = (pos->x) * scale_x + letterbox.pos.x;
  pos->y = (pos->y) * scale_y + letterbox.pos.y;
  cam_pos->x = (cam_pos->x) * scale_x;
  cam_pos->y = (cam_pos->y) * scale_y;
}

void get_letterbox() {
  const float current_aspect_ratio = (float)gfx_get_display_width() / (float)gfx_get_display_height();
  const float ratio_ratio = current_aspect_ratio / TARGET_ASPECT_RATIO;
  LETTERBOX.pos.x = get_letterbox_x(current_aspect_ratio, ratio_ratio);
  LETTERBOX.pos.y = get_letterbox_y(current_aspect_ratio, ratio_ratio);
  LETTERBOX.size.x = get_letterbox_width(current_aspect_ratio, ratio_ratio);
  LETTERBOX.size.y = get_letterbox_height(current_aspect_ratio, ratio_ratio);
}

