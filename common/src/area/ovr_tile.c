#include "area/ovr_tile.h"
#include "camera/camera.h"
#include "khg_gfx/texture.h"
#include "khg_phy/core/phy_vector.h"
#include "khg_utl/algorithm.h"
#include "khg_utl/array.h"
#include "khg_utl/vector.h"
#include "letterbox.h"
#include "resources/ovr_tile_loader.h"
#include "resources/texture_loader.h"
#include <math.h>

#define OVR_TILE_RAW_POSITION 420 
#define GROUND_LAYER 0
#define ELEMENT_LAYER 1
#define BORDER_LAYER 2
#define GROUND_TEX_SIZE 628

utl_vector *OVR_TILE_OPTIONS = NULL;

static float OVR_TILE_SCALE = 1.0f;
static float OVR_TILE_SIZE = 0.0f;

static utl_vector *OVR_TILE_ELEMENTS = NULL;
static bool OVR_TILE_ELEMENTS_RENDERED = false;

static int compare_ovr_tile_elements(const void *a, const void *b) {
  const ovr_tile_element *e1 = a;
  const ovr_tile_element *e2 = b;
  float arg1 = ovr_tile_rendering_pos(e1->parent_tile->pos, e1->pos, get_or_add_texture(e1->element_tex_id).height * OVR_TILE_SCALE).y;
  float arg2 = ovr_tile_rendering_pos(e2->parent_tile->pos, e2->pos, get_or_add_texture(e2->element_tex_id).height * OVR_TILE_SCALE).y;
  if (fabsf(arg1 - arg2) < 1e-6f) {
    return 0;
  }
  return (arg1 > arg2) - (arg1 < arg2);
}

void set_ovr_tile_scale(const float tile_scale) {
  OVR_TILE_SCALE = tile_scale;
  OVR_TILE_SIZE = GROUND_TEX_SIZE * OVR_TILE_SCALE;
}

const float get_ovr_tile_size() {
  return OVR_TILE_SIZE;
}

phy_vector2 ovr_tile_rendering_pos(const phy_vector2 coords, const phy_vector2 offset, const float tex_height) {
  const phy_vector2 offset_from_mid = phy_vector2_sub(offset, phy_vector2_new(OVR_TILE_RAW_POSITION, OVR_TILE_RAW_POSITION));
  const phy_vector2 scaled_offset = phy_vector2_mul(offset_from_mid, OVR_TILE_SCALE);
  const float hor_dist_comp = OVR_TILE_SIZE * sqrt(3.0f) / 2.0f;
  const float ver_dist_comp = OVR_TILE_SIZE / 2.0f;
  const phy_vector2 pos = phy_vector2_new(hor_dist_comp * coords.x, ver_dist_comp * coords.y);
  if (phy_vector2_len(scaled_offset) != 0.0f) {
    return phy_vector2_add(pos, phy_vector2_add(scaled_offset, phy_vector2_new(0.0f, -(tex_height / 2.0f))));
  }
  return pos;
}

phy_vector2 ovr_tile_pos_to_world_pos(const phy_vector2 coords) {
  const float hor_dist_comp = OVR_TILE_SIZE * sqrt(3.0f) / 2.0f;
  const float ver_dist_comp = OVR_TILE_SIZE / 2.0f;
  return phy_vector2_new(hor_dist_comp * coords.x, ver_dist_comp * coords.y);
}

void render_ovr_tile_item(const unsigned int tex_id, const phy_vector2 coords, const phy_vector2 offset, const bool flipped) {
  gfx_texture tex_ref = get_or_add_texture(tex_id);
  tex_ref.width *= OVR_TILE_SCALE;
  tex_ref.height *= OVR_TILE_SCALE;
  phy_vector2 pos = ovr_tile_rendering_pos(coords, offset, tex_ref.height);
  phy_vector2 cam_pos = phy_vector2_new(CAMERA.position.x, CAMERA.position.y);
  gfx_texture tex = { tex_ref.id, tex_ref.width, tex_ref.height, 0 };
  transform_letterbox_element_tex(LETTERBOX, &pos, &cam_pos, &tex);
  gfx_image_no_block(pos.x, pos.y, tex, cam_pos.x, cam_pos.y, CAMERA.zoom, true, flipped);
  OVR_TILE_ELEMENTS_RENDERED = false;
}

void render_ovr_tile_element_item(const ovr_tile_element *element, const phy_vector2 offset, const bool flipped) {
  gfx_texture tex_ref = get_or_add_texture(element->element_tex_id);
  tex_ref.width *= OVR_TILE_SCALE;
  tex_ref.height *= OVR_TILE_SCALE;
  phy_vector2 pos = ovr_tile_rendering_pos(element->parent_tile->pos, offset, tex_ref.height);
  phy_vector2 cam_pos = phy_vector2_new(CAMERA.position.x, CAMERA.position.y);
  gfx_texture tex = { tex_ref.id, tex_ref.width, tex_ref.height, 0 };
  transform_letterbox_element_tex(LETTERBOX, &pos, &cam_pos, &tex);
  gfx_image_no_block(pos.x, pos.y, tex, cam_pos.x, cam_pos.y, CAMERA.zoom, true, flipped);
  OVR_TILE_ELEMENTS_RENDERED = true;
}

void add_ovr_tile_elements(ovr_tile_info *parent_tile) {
  if (!OVR_TILE_ELEMENTS) {
    OVR_TILE_ELEMENTS = utl_vector_create(sizeof(ovr_tile_element));
  }
  const ovr_tile tile = get_or_add_ovr_tile(parent_tile->tile_id);
  utl_vector_reserve(OVR_TILE_ELEMENTS, OVR_TILE_ELEMENTS->capacity_size + tile.num_elements);
  for (unsigned int i = 0; i < tile.num_elements; i++) {
    ovr_tile_element element = *(ovr_tile_element *)utl_array_at(tile.elements, i);
    element.parent_tile = parent_tile;
    utl_vector_push_back(OVR_TILE_ELEMENTS, &element);
  }
  ovr_tile_element *data = utl_vector_data(OVR_TILE_ELEMENTS);
  const unsigned int data_size = utl_vector_size(OVR_TILE_ELEMENTS);
  utl_algorithm_stable_sort(data, data_size, sizeof(ovr_tile_element), compare_ovr_tile_elements);
}

void remove_ovr_tile_elements(ovr_tile_info *parent_tile) {
  const ovr_tile tile = get_or_add_ovr_tile(parent_tile->tile_id);
  for (int i = utl_vector_size(OVR_TILE_ELEMENTS) - 1; i >= 0; i--) {
    if (((ovr_tile_element *)utl_vector_at(OVR_TILE_ELEMENTS, i))->parent_tile == parent_tile) {
      utl_vector_erase(OVR_TILE_ELEMENTS, i, 1);
    }
  }
  if (utl_vector_size(OVR_TILE_ELEMENTS) == 0) {
    utl_vector_deallocate(OVR_TILE_ELEMENTS);
  }
}

void render_ovr_tile(const ovr_tile_info *tile, unsigned int *layer) {
  const ovr_tile tile_ref = get_or_add_ovr_tile(tile->tile_id);
  if (!layer) {
    return;
  }
  switch (*layer) {
    case GROUND_LAYER:
      render_ovr_tile_item(tile_ref.ground_tex_id, tile->pos, phy_vector2_new(OVR_TILE_RAW_POSITION, OVR_TILE_RAW_POSITION), false);
      break;
    case ELEMENT_LAYER:
      if (OVR_TILE_ELEMENTS_RENDERED) {
        break;
      }
      for (ovr_tile_element *it = (ovr_tile_element *)utl_vector_begin(OVR_TILE_ELEMENTS); it != (ovr_tile_element *)utl_vector_end(OVR_TILE_ELEMENTS); it++) { 
        if (it) {
          render_ovr_tile_element_item(it, it->pos, it->flipped);
        }
      }
      break;
    case BORDER_LAYER:
      render_ovr_tile_item(tile_ref.border_tex_id, tile->pos, phy_vector2_new(OVR_TILE_RAW_POSITION, OVR_TILE_RAW_POSITION), false);
      break;
    default:
      break;
  }
}

void set_ovr_tile_options() {
  OVR_TILE_OPTIONS = utl_vector_create(sizeof(unsigned int));
  utl_vector_reserve(OVR_TILE_OPTIONS, NUM_OVR_TILES);
  for (unsigned int i = 0; i < NUM_OVR_TILES; i++) {
    utl_vector_push_back(OVR_TILE_OPTIONS, &i);
  }
}

void clear_ovr_tile_options() {
  utl_vector_deallocate(OVR_TILE_OPTIONS);
}

