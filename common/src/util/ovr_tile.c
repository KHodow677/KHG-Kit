#define NAMESPACE_KIN_USE
#define NAMESPACE_TASKING_USE

#include "khg_gfx/texture.h"
#include "khg_kin/namespace.h"
#include "khg_utl/algorithm.h"
#include "khg_utl/array.h"
#include "khg_utl/easing.h"
#include "khg_utl/random.h"
#include "khg_utl/vector.h"
#include "tasking/namespace.h"
#include "util/camera/camera.h"
#include "util/ovr_tile.h"
#include "util/frame.h"
#include "util/letterbox.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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

static float OVR_TILE_ELEMENT_MIN_DURATION = 3.0f;
static float OVR_TILE_ELEMENT_MAX_DURATION = 5.0f;
static float OVR_TILE_ELEMENT_MAX_ANGLE = 0.174533f;

static int compare_ovr_tile_elements(const void *a, const void *b) {
  const ovr_tile_element *e1 = a;
  const ovr_tile_element *e2 = b;
  if (e1->stable != e2->stable) {
    return e2->stable - e1->stable; 
  }
  float arg1 = ovr_tile_rendering_pos(e1->parent_tile->pos, e1->pos, NAMESPACE_TASKING()->get_texture_data(e1->element_tex_id_loc).height * OVR_TILE_SCALE, 0.0f).y;
  float arg2 = ovr_tile_rendering_pos(e2->parent_tile->pos, e2->pos, NAMESPACE_TASKING()->get_texture_data(e2->element_tex_id_loc).height * OVR_TILE_SCALE, 0.0f).y;
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

kin_vec ovr_tile_rendering_pos(const kin_vec coords, const kin_vec offset, const float tex_height, const float tex_angle) {
  const kin_vec offset_from_mid = NAMESPACE_KIN()->vec_sub(offset, (kin_vec){ OVR_TILE_RAW_POSITION, OVR_TILE_RAW_POSITION });
  const kin_vec scaled_offset = NAMESPACE_KIN()->vec_scale(offset_from_mid, OVR_TILE_SCALE);
  const float hor_dist_comp = OVR_TILE_SIZE * sqrt(3.0f) / 2.0f;
  const float ver_dist_comp = OVR_TILE_SIZE / 2.0f;
  const kin_vec pos = { hor_dist_comp * coords.x, ver_dist_comp * coords.y };
  if (NAMESPACE_KIN()->vec_length(scaled_offset) != 0.0f) {
    return NAMESPACE_KIN()->vec_add(pos, NAMESPACE_KIN()->vec_add(scaled_offset, (kin_vec){ (tex_height / 2.0f) * sinf(tex_angle), -(tex_height / 2.0f) * cosf(tex_angle) }));
  }
  return pos;
}

kin_vec ovr_tile_pos_to_world_pos(const kin_vec coords) {
  const float hor_dist_comp = OVR_TILE_SIZE * sqrt(3.0f) / 2.0f;
  const float ver_dist_comp = OVR_TILE_SIZE / 2.0f;
  return (kin_vec){ hor_dist_comp * coords.x, ver_dist_comp * coords.y };
}

void render_ovr_tile_item(const unsigned int tex_id_loc, const kin_vec coords, const kin_vec offset) {
  gfx_texture tex_ref = NAMESPACE_TASKING()->get_texture_data(tex_id_loc);
  tex_ref.width *= OVR_TILE_SCALE;
  tex_ref.height *= OVR_TILE_SCALE;
  kin_vec pos = ovr_tile_rendering_pos(coords, offset, tex_ref.height, 0.0f);
  kin_vec cam_pos = { CAMERA.position.x, CAMERA.position.y };
  gfx_texture tex = { tex_ref.id, tex_ref.width, tex_ref.height, 0 };
  transform_letterbox_element_tex(LETTERBOX, &pos, &cam_pos, &tex);
  gfx_image_no_block(pos.x, pos.y, tex, cam_pos.x, cam_pos.y, CAMERA.zoom, true, false);
  OVR_TILE_ELEMENTS_RENDERED = false;
}

void render_ovr_tile_element_item(const ovr_tile_element *element) {
  gfx_texture tex_ref = NAMESPACE_TASKING()->get_texture_data(element->element_tex_id_loc);
  tex_ref.width *= OVR_TILE_SCALE;
  tex_ref.height *= OVR_TILE_SCALE;
  kin_vec pos = ovr_tile_rendering_pos(element->parent_tile->pos, element->pos, tex_ref.height, element->angle);
  kin_vec cam_pos = { CAMERA.position.x, CAMERA.position.y };
  gfx_texture tex = { tex_ref.id, tex_ref.width, tex_ref.height, element->angle };
  transform_letterbox_element_tex(LETTERBOX, &pos, &cam_pos, &tex);
  gfx_image_no_block(pos.x, pos.y, tex, cam_pos.x, cam_pos.y, CAMERA.zoom, true, element->flipped);
  OVR_TILE_ELEMENTS_RENDERED = true;
}

void update_ovr_tile_element_item(ovr_tile_element *element, const float dt) {
  if (element->stable) {
    return;
  }
  frame_tick(&element->frame, dt);
  if (frame_time_up(&element->frame)) {
    element->current_angle = element->target_angle;
    element->target_angle = element->target_angle > 0.0f ? utl_random_uniform(-OVR_TILE_ELEMENT_MAX_ANGLE, 0.0f) : utl_random_uniform(0.0f, OVR_TILE_ELEMENT_MAX_ANGLE);
    element->frame.duration = utl_random_uniform(OVR_TILE_ELEMENT_MIN_DURATION, OVR_TILE_ELEMENT_MAX_DURATION);
    frame_reset(&element->frame);
  }
  const float normalized_ang_diff = fmodf(element->target_angle - element->current_angle + M_PI, 2 * M_PI) - M_PI;
  element->angle = element->current_angle + normalized_ang_diff * utl_easing_linear_interpolation(frame_perc(&element->frame));
}

void add_ovr_tile_elements(ovr_tile_info *parent_tile) {
  if (!OVR_TILE_ELEMENTS) {
    OVR_TILE_ELEMENTS = utl_vector_create(sizeof(ovr_tile_element));
  }
  const ovr_tile tile = NAMESPACE_TASKING()->get_tile_data(parent_tile->tile_id);
  utl_vector_reserve(OVR_TILE_ELEMENTS, OVR_TILE_ELEMENTS->capacity_size + tile.num_elements);
  for (unsigned int i = 0; i < tile.num_elements; i++) {
    ovr_tile_element element = *(ovr_tile_element *)utl_array_at(tile.elements, i);
    if (!element.stable) {
      element.frame.timer = utl_random_uniform(0.0f, OVR_TILE_ELEMENT_MIN_DURATION);
      element.frame.duration = utl_random_uniform(OVR_TILE_ELEMENT_MIN_DURATION, OVR_TILE_ELEMENT_MAX_DURATION);
      element.angle = utl_random_uniform(-OVR_TILE_ELEMENT_MAX_ANGLE, OVR_TILE_ELEMENT_MAX_ANGLE);
      element.target_angle = utl_random_uniform(-OVR_TILE_ELEMENT_MAX_ANGLE, OVR_TILE_ELEMENT_MAX_ANGLE);
    }
    element.parent_tile = parent_tile;
    utl_vector_push_back(OVR_TILE_ELEMENTS, &element);
  }
  ovr_tile_element *data = utl_vector_data(OVR_TILE_ELEMENTS);
  const unsigned int data_size = utl_vector_size(OVR_TILE_ELEMENTS);
  utl_algorithm_stable_sort(data, data_size, sizeof(ovr_tile_element), compare_ovr_tile_elements);
}

void remove_ovr_tile_elements(ovr_tile_info *parent_tile) {
  const ovr_tile tile = NAMESPACE_TASKING()->get_tile_data(parent_tile->tile_id);
  for (int i = utl_vector_size(OVR_TILE_ELEMENTS) - 1; i >= 0; i--) {
    if (((ovr_tile_element *)utl_vector_at(OVR_TILE_ELEMENTS, i))->parent_tile == parent_tile) {
      utl_vector_erase(OVR_TILE_ELEMENTS, i, 1);
    }
  }
  if (utl_vector_size(OVR_TILE_ELEMENTS) == 0) {
    utl_vector_deallocate(OVR_TILE_ELEMENTS);
  }
}

void render_ovr_tile(const ovr_tile_info *tile, unsigned int *layer, const float dt) {
  const ovr_tile tile_ref = NAMESPACE_TASKING()->get_tile_data(tile->tile_id);
  if (!layer) {
    return;
  }
  switch (*layer) {
    case GROUND_LAYER:
      render_ovr_tile_item(tile_ref.ground_tex_id_loc, tile->pos, (kin_vec){ OVR_TILE_RAW_POSITION, OVR_TILE_RAW_POSITION });
      break;
    case ELEMENT_LAYER:
      if (OVR_TILE_ELEMENTS_RENDERED) {
        break;
      }
      if (OVR_TILE_ELEMENTS) {
        for (ovr_tile_element *it = (ovr_tile_element *)utl_vector_begin(OVR_TILE_ELEMENTS); it != (ovr_tile_element *)utl_vector_end(OVR_TILE_ELEMENTS); it++) { 
          update_ovr_tile_element_item(it, dt);
          render_ovr_tile_element_item(it);
        }
      }
      break;
    case BORDER_LAYER:
      render_ovr_tile_item(tile_ref.border_tex_id_loc, tile->pos, (kin_vec){ OVR_TILE_RAW_POSITION, OVR_TILE_RAW_POSITION });
      break;
    default:
      break;
  }
}

