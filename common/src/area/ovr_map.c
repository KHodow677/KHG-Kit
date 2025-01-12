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

phy_vector2 get_ovr_map_pos(ovr_map *map, const phy_vector2 map_pos, const unsigned int tile_size, const phy_vector2 coords) {
  const phy_vector2 half_screen = phy_vector2_new(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
  phy_vector2 pos = phy_vector2_add(phy_vector2_mul(phy_vector2_sub(map_pos, half_screen), map->tile_scale), half_screen);
  float hor_dist_comp = tile_size * sqrt(3.0f) / 2.0f;
  float ver_dist_comp = tile_size / 2.0f;
  phy_vector2 raw = phy_vector2_new(hor_dist_comp * coords.x, ver_dist_comp * coords.y);
  return phy_vector2_add(pos, raw);
}

unsigned int get_ovr_tile_size(ovr_map *map, const unsigned int tex_id) {
  gfx_texture tex_ref = get_or_add_texture(tex_id);
  tex_ref.height *= map->tile_scale;
  return tex_ref.height;
}

void render_item(ovr_map *map, const ovr_tile tile, const unsigned int tex_id, const unsigned int tex_size, const phy_vector2 map_pos, const phy_vector2 coords, const float parallax_value) {
  gfx_texture tex_ref = get_or_add_texture(tex_id);
  tex_ref.width *= map->tile_scale;
  tex_ref.height *= map->tile_scale;
  phy_vector2 pos = get_ovr_map_pos(map, map_pos, tex_size, coords);
  phy_vector2 cam_pos = phy_vector2_new(CAMERA.position.x, CAMERA.position.y);
  gfx_texture tex = { tex_ref.id, tex_ref.width, tex_ref.height, 0 };
  transform_letterbox_element_tex(LETTERBOX, &pos, &cam_pos, &tex);
  gfx_image_no_block(pos.x, pos.y, tex, cam_pos.x * parallax_value, cam_pos.y * parallax_value, CAMERA.zoom, true, false);
}

void render_ovr_map(ovr_map *map, const phy_vector2 map_pos, const float parallax_value) {
  for (ovr_tile_info *at = utl_vector_begin(map->tiles); at != (ovr_tile_info *)utl_vector_end(map->tiles); at++) {
    const ovr_tile tile = get_or_add_ovr_tile(at->id);
    const unsigned int tile_size = get_ovr_tile_size(map, tile.ground_tex_id);
    render_item(map, tile, tile.ground_tex_id, tile_size, map_pos, at->pos, parallax_value);
    render_item(map, tile, tile.border_tex_id, tile_size, map_pos, at->pos, parallax_value);
  }
}

