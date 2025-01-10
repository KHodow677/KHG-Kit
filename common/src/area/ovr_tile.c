#include "area/ovr_tile.h"
#include "camera/camera.h"
#include "khg_gfx/texture.h"
#include "letterbox.h"
#include "resources/texture_loader.h"

void render_tiles(utl_vector *tiles, int parallax_value) {
  for (ovr_tile *at = utl_vector_begin(tiles); at != (ovr_tile *)utl_vector_end(tiles); at++) {
    phy_vector2 pos = at->pos;
    phy_vector2 cam_pos = phy_vector2_new(CAMERA.position.x, CAMERA.position.y);
    const gfx_texture ground_tex_ref = get_or_add_texture(at->ground_tex_id);
    gfx_texture ground_tex = { ground_tex_ref.id, ground_tex_ref.width, ground_tex_ref.height, 0 };
    transform_letterbox_element_tex(LETTERBOX, &pos, &cam_pos, &ground_tex);
    gfx_image_no_block(pos.x, pos.y, ground_tex, cam_pos.x * parallax_value, cam_pos.y * parallax_value, CAMERA.zoom, true, false);
  }
}

