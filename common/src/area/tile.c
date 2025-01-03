#include "area/tile.h"
#include "camera/camera.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/texture.h"
#include "khg_utl/array.h"
#include "letterbox.h"
#include "resources/texture_loader.h"

void render_tiles(utl_array *tiles, int parallax_value) {
  for (area_tile *at = utl_array_begin(tiles); at != (area_tile *)utl_array_end(tiles); at++) {
    phy_vector2 pos = at->pos;
    phy_vector2 cam_pos = phy_vector2_new(CAMERA.position.x, CAMERA.position.y);
    const gfx_texture tex_ref = get_or_add_texture(at->tex_id);
    gfx_texture tex = { tex_ref.id, tex_ref.width + 1, tex_ref.height + 1, 0 };
    transform_letterbox_element_tex(LETTERBOX, &pos, &cam_pos, &tex);
    gfx_image_no_block(pos.x, pos.y, tex, cam_pos.x * parallax_value, cam_pos.y * parallax_value, CAMERA.zoom, true, false);
  }
}

