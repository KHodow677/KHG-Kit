#include "area/ovr_tile.h"
#include "camera/camera.h"
#include "khg_gfx/texture.h"
#include "khg_phy/core/phy_vector.h"
#include "letterbox.h"
#include "resources/ovr_tile_loader.h"
#include "resources/texture_loader.h"
#include <math.h>

#define OVR_TILE_RAW_POSITION 420 
#define GROUND_LAYER 0
#define ELEMENT_LAYER 1
#define BORDER_LAYER 2

static float OVR_TILE_SCALE = 1.0f;
static float OVR_TILE_SIZE = 0.0f;

void set_ovr_tile_scale(const float tile_scale) {
  OVR_TILE_SCALE = tile_scale;
}

void set_ovr_tile_size(const unsigned int tile_tex_id) {
  gfx_texture tex_ref = get_or_add_texture(tile_tex_id);
  OVR_TILE_SIZE = tex_ref.height * OVR_TILE_SCALE;
}

phy_vector2 get_ovr_tile_rendering_pos(const phy_vector2 coords, const phy_vector2 offset, const float tex_height) {
  float hor_dist_comp = OVR_TILE_SIZE * sqrt(3.0f) / 2.0f;
  float ver_dist_comp = OVR_TILE_SIZE / 2.0f;
  phy_vector2 pos = phy_vector2_new(hor_dist_comp * coords.x, ver_dist_comp * coords.y);
  if (phy_vector2_len(offset) != 0.0f) {
    return phy_vector2_add(pos, phy_vector2_add(offset, phy_vector2_new(0.0f, -(tex_height / 2.0f))));
  }
  return pos;
}

phy_vector2 ovr_tile_pos_to_world_pos(const phy_vector2 coords) {
  float hor_dist_comp = OVR_TILE_SIZE * sqrt(3.0f) / 2.0f;
  float ver_dist_comp = OVR_TILE_SIZE / 2.0f;
  return phy_vector2_new(hor_dist_comp * coords.x, ver_dist_comp * coords.y);
}

void render_ovr_tile_item(const ovr_tile tile, const unsigned int tex_id, const phy_vector2 coords, const phy_vector2 offset, const bool flipped) {
  gfx_texture tex_ref = get_or_add_texture(tex_id);
  tex_ref.width *= OVR_TILE_SCALE;
  tex_ref.height *= OVR_TILE_SCALE;
  const phy_vector2 offset_from_mid = phy_vector2_sub(offset, phy_vector2_new(OVR_TILE_RAW_POSITION, OVR_TILE_RAW_POSITION));
  phy_vector2 pos = get_ovr_tile_rendering_pos(coords, phy_vector2_mul(offset_from_mid, OVR_TILE_SCALE), tex_ref.height);
  phy_vector2 cam_pos = phy_vector2_new(CAMERA.position.x, CAMERA.position.y);
  gfx_texture tex = { tex_ref.id, tex_ref.width, tex_ref.height, 0 };
  transform_letterbox_element_tex(LETTERBOX, &pos, &cam_pos, &tex);
  gfx_image_no_block(pos.x, pos.y, tex, cam_pos.x, cam_pos.y, CAMERA.zoom, true, flipped);
}

void render_ovr_tile(const unsigned int tile_id, const phy_vector2 tile_pos, const unsigned int layer) {
  const ovr_tile tile = get_or_add_ovr_tile(tile_id);
  switch (layer) {
    case GROUND_LAYER:
      render_ovr_tile_item(tile, tile.ground_tex_id, tile_pos, phy_vector2_new(OVR_TILE_RAW_POSITION, OVR_TILE_RAW_POSITION), false);
      break;
    case ELEMENT_LAYER:
      for (unsigned int i = 0; i < tile.num_elements; i++) {
        ovr_tile_element element = tile.elements[i];
        render_ovr_tile_item(tile, element.element_tex_id, tile_pos, element.pos, element.flipped);
      }
      break;
    case BORDER_LAYER:
      render_ovr_tile_item(tile, tile.border_tex_id, tile_pos, phy_vector2_new(OVR_TILE_RAW_POSITION, OVR_TILE_RAW_POSITION), false);
      break;
  }
}

