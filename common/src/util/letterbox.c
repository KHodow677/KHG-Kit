#include "game.h"
#include "khg_gfx/elements.h"
#include "khg_kin/namespace.h"
#include "util/letterbox.h"

static const float TARGET_ASPECT_RATIO = 16.0f / 9.0f;
static const float RESOLUTION_SCALE = 1.0f;
gfx_aabb LETTERBOX = { 0 };

static const float get_letterbox_x(const float current_aspect_ratio, const float ratio_ratio) {
  const float scaled_width = gfx_get_display_width() / RESOLUTION_SCALE;
  return current_aspect_ratio > TARGET_ASPECT_RATIO ? (scaled_width - scaled_width / ratio_ratio) / 2.0f : 0;
}

static const float get_letterbox_y(const float current_aspect_ratio, const float ratio_ratio) {
  const float scaled_height = gfx_get_display_height() / RESOLUTION_SCALE;
  const float height = gfx_get_display_height();
  return current_aspect_ratio < TARGET_ASPECT_RATIO ? (scaled_height - scaled_height * ratio_ratio) / 2.0f + (height - scaled_height) : height - scaled_height;
}

static const float get_letterbox_width(const float current_aspect_ratio, const float ratio_ratio) {
  const float scaled_width = gfx_get_display_width() / RESOLUTION_SCALE;
  return current_aspect_ratio > TARGET_ASPECT_RATIO ? scaled_width / ratio_ratio : scaled_width;
}

static const float get_letterbox_height(const float current_aspect_ratio, const float ratio_ratio) {
  const float scaled_height = gfx_get_display_height() / RESOLUTION_SCALE;
  return current_aspect_ratio < TARGET_ASPECT_RATIO ? scaled_height * ratio_ratio : scaled_height;
}

void transform_letterbox_element_tex(const gfx_aabb letterbox, kin_vec *pos, kin_vec *cam_pos, gfx_texture *tex) {
  const float scale_x = letterbox.size.x / SCREEN_WIDTH * SCREEN_SCALE;
  const float scale_y = letterbox.size.y / SCREEN_HEIGHT * SCREEN_SCALE;
  tex->width *= scale_x;
  tex->height *= scale_y;
  pos->x = (pos->x * scale_x) + letterbox.pos.x;
  pos->y = (pos->y * scale_y) + letterbox.pos.y;
  cam_pos->x *= scale_x;
  cam_pos->y *= scale_y;
}

void transform_letterbox_element_aabb(const gfx_aabb letterbox, kin_vec *pos, kin_vec *cam_pos, gfx_aabb *aabb) {
  const float scale_x = letterbox.size.x / SCREEN_WIDTH * SCREEN_SCALE;
  const float scale_y = letterbox.size.y / SCREEN_HEIGHT * SCREEN_SCALE;
  aabb->size.x *= scale_x;
  aabb->size.y *= scale_y;
  pos->x = (pos->x * scale_x) + letterbox.pos.x;
  pos->y = (pos->y * scale_y) + letterbox.pos.y;
  cam_pos->x *= scale_x;
  cam_pos->y *= scale_y;
}

void get_letterbox() {
  const float current_aspect_ratio = (float)(gfx_get_display_width()) / (float)(gfx_get_display_height());
  const float ratio_ratio = current_aspect_ratio / TARGET_ASPECT_RATIO;
  LETTERBOX.pos.x = get_letterbox_x(current_aspect_ratio, ratio_ratio);
  LETTERBOX.pos.y = get_letterbox_y(current_aspect_ratio, ratio_ratio);
  LETTERBOX.size.x = get_letterbox_width(current_aspect_ratio, ratio_ratio);
  LETTERBOX.size.y = get_letterbox_height(current_aspect_ratio, ratio_ratio);
}

