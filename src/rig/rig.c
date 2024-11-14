#include "rig/rig.h"
#include "camera/camera.h"
#include "khg_phy/shape.h"
#include "khg_phy/space.h"
#include "khg_utl/array.h"
#include "letterbox.h"
#include "physics/physics.h"
#include "resources/texture_loader.h"
#include "khg_gfx/texture.h"
#include "khg_phy/body.h"
#include <stdio.h>

bone create_bone(const phy_vector2 bone_pos, const int tex_id, const int layer, bone *parent) {
  phy_rigid_body_initializer bone_init = phy_rigid_body_initializer_default;
  bone_init.type = PHY_RIGID_BODY_TYPE_DYNAMIC;
  bone_init.position = bone_pos;
  bone_init.material = (phy_material){ .density=1.0, .restitution = 0.85, .friction = 0.0 };
  bone res = { phy_rigid_body_new(bone_init), phy_rect_shape_new(0.0f, 0.0f, phy_vector2_zero), phy_vector2_new(0.0f, 0.0f), tex_id, layer, parent };
  phy_rigid_body_add_shape(res.bone_body, res.bone_shape);
  phy_rigid_body_disable_collisions(res.bone_body);
  phy_space_add_rigidbody(SPACE, res.bone_body);
  return res;
}

void create_rig(rig *r, const size_t num_bones, const phy_rigid_body *bone_body, const int root_tex, const size_t init_layer) {
  r->enabled = true;
  r->num_bones = num_bones;
  r->bones = utl_array_create(sizeof(bone), num_bones);
  const bone root_bone = create_bone(phy_rigid_body_get_position(bone_body), root_tex, init_layer, NULL);
  utl_array_set(r->bones, init_layer, &root_bone);
}

void free_rig(const rig *r) {
  for (bone* b = utl_array_begin(r->bones); b != (bone *)utl_array_end(r->bones); b++) {
    phy_space_remove_rigidbody(SPACE, b->bone_body);
    phy_rigid_body_free(b->bone_body);
  }
  utl_array_deallocate(r->bones);
}

void render_rig(const rig *r, const float parallax_value, const bool flipped) {
  for (bone* b = utl_array_begin(r->bones); b != (bone *)utl_array_end(r->bones); b++) {
    phy_vector2 pos = phy_rigid_body_get_position(b->bone_body);
    phy_vector2 cam_pos = phy_vector2_new(CAMERA.position.x, CAMERA.position.y);
    const float angle = phy_rigid_body_get_angle(b->bone_body);
    const gfx_texture tex_ref = get_or_add_texture(b->bone_tex_id);
    gfx_texture tex = { tex_ref.id, tex_ref.width, tex_ref.height, tex_ref.angle };
    transform_letterbox_element(LETTERBOX, &pos, &cam_pos, &tex);
    gfx_image_no_block(pos.x, pos.y, tex, 0.0f, 0.0f, cam_pos.x * parallax_value, cam_pos.y * parallax_value, CAMERA.zoom, true, flipped);
  }
}

