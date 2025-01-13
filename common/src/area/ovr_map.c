#include "area/ovr_map.h"
#include "camera/camera.h"
#include "game.h"
#include "khg_gfx/texture.h"
#include "khg_phy/core/phy_vector.h"
#include "letterbox.h"
#include "resources/ovr_tile_loader.h"
#include "resources/texture_loader.h"
#include <math.h>

void set_ovr_map_tile_scale(ovr_map *map, const float tile_scale) {
  map->tile_scale = tile_scale;
}

phy_vector2 get_ovr_map_pos(ovr_map *map, const phy_vector2 map_pos, const unsigned int tile_size, const phy_vector2 coords, const phy_vector2 offset, const float tex_height) {
  const phy_vector2 half_screen = phy_vector2_new(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
  phy_vector2 pos = phy_vector2_add(phy_vector2_mul(phy_vector2_sub(map_pos, half_screen), map->tile_scale), half_screen);
  float hor_dist_comp = tile_size * sqrt(3.0f) / 2.0f;
  float ver_dist_comp = tile_size / 2.0f;
  phy_vector2 raw = phy_vector2_new(hor_dist_comp * coords.x, ver_dist_comp * coords.y);
  if (phy_vector2_len(offset) != 0.0f) {
    return phy_vector2_add(phy_vector2_add(pos, raw), phy_vector2_add(offset, phy_vector2_new(0.0f, -(tex_height / 2.0f))));
  }
  return phy_vector2_add(pos, raw);
}

unsigned int get_ovr_tile_size(ovr_map *map, const unsigned int tex_id) {
  gfx_texture tex_ref = get_or_add_texture(tex_id);
  tex_ref.height *= map->tile_scale;
  return tex_ref.height;
}

void render_item(ovr_map *map, const ovr_tile tile, const unsigned int tex_id, const unsigned int tile_size, const phy_vector2 map_pos, const phy_vector2 coords, const phy_vector2 perc_offset, const bool flipped) {
  gfx_texture tex_ref = get_or_add_texture(tex_id);
  tex_ref.width *= map->tile_scale;
  tex_ref.height *= map->tile_scale;
  const phy_vector2 offset_from_mid = phy_vector2_sub(perc_offset, phy_vector2_new(0.5f, 0.5f));
  phy_vector2 pos = get_ovr_map_pos(map, map_pos, tile_size, coords, phy_vector2_mul(offset_from_mid, tile_size * 1.338f), tex_ref.height);
  phy_vector2 cam_pos = phy_vector2_new(CAMERA.position.x, CAMERA.position.y);
  gfx_texture tex = { tex_ref.id, tex_ref.width, tex_ref.height, 0 };
  transform_letterbox_element_tex(LETTERBOX, &pos, &cam_pos, &tex);
  gfx_image_no_block(pos.x, pos.y, tex, cam_pos.x, cam_pos.y, CAMERA.zoom, true, flipped);
}

void render_ovr_map(ovr_map *map, const phy_vector2 map_pos) {
  for (ovr_tile_info *at = utl_vector_begin(map->tiles); at != (ovr_tile_info *)utl_vector_end(map->tiles); at++) {
    const ovr_tile tile = get_or_add_ovr_tile(at->id);
    const unsigned int tile_size = get_ovr_tile_size(map, tile.ground_tex_id);
    render_item(map, tile, tile.ground_tex_id, tile_size, map_pos, at->pos, phy_vector2_new(0.5f, 0.5f), false);
    render_item(map, tile, tile.border_tex_id, tile_size, map_pos, at->pos, phy_vector2_new(0.5f, 0.5f), false);
    for (unsigned int i = 0; i < tile.num_elements; i++) {
      ovr_tile_element element = tile.elements[i];
      render_item(map, tile, element.element_tex_id, tile_size, map_pos, at->pos, element.pos, element.flipped);
    }
  }
}

