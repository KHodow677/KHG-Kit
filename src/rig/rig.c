#include "rig/rig.h"
#include "camera/camera.h"
#include "khg_phy/constraints/constraint.h"
#include "khg_phy/constraints/distance_constraint.h"
#include "khg_phy/constraints/hinge_constraint.h"
#include "khg_phy/core/phy_vector.h"
#include "khg_phy/shape.h"
#include "khg_phy/space.h"
#include "khg_utl/array.h"
#include "letterbox.h"
#include "physics/physics.h"
#include "resources/texture_loader.h"
#include "khg_gfx/texture.h"
#include "khg_phy/body.h"
#include <stdio.h>

bone create_bone(const phy_vector2 bone_pos, const phy_vector2 bone_offset, const int tex_id, const int layer, bone *parent) {
  phy_rigid_body_initializer bone_init = phy_rigid_body_initializer_default;
  bone_init.type = PHY_RIGID_BODY_TYPE_DYNAMIC;
  bone_init.position = phy_vector2_add(bone_pos, bone_offset);
  bone_init.material = (phy_material){ .density=1.0, .restitution = 0.85, .friction = 0.0 };
  bone res = { phy_rigid_body_new(bone_init), phy_rect_shape_new(0.0f, 0.0f, phy_vector2_zero), NULL, phy_vector2_new(0.0f, 0.0f), tex_id, layer, parent };
  /*phy_rigid_body_disable_collisions(res.bone_body);*/
  phy_rigid_body_set_mass(res.bone_body, 1.0f);
  phy_rigid_body_add_shape(res.bone_body, res.bone_shape);
  phy_space_add_rigidbody(SPACE, res.bone_body);
  return res;
}

void add_bone(rig *r, const phy_vector2 bone_offset, const bone_joint_pair joint_info, const int tex_id, const int layer, bone *parent) {
  bone b = create_bone(phy_rigid_body_get_position(parent->bone_body), bone_offset, tex_id, layer, parent);
  phy_hinge_constraint_initializer cons_init = phy_hinge_constraint_initializer_default;
  cons_init.a = parent->bone_body;
  cons_init.b = b.bone_body;
  cons_init.anchor = phy_vector2_new(50.0 + 2.0, 15.0);
  cons_init.enable_limits = true;
  cons_init.lower_limit = -M_PI * 0.5;
  cons_init.upper_limit = M_PI * 0.5;
  phy_constraint *hinge_cons0 = phy_hinge_constraint_new(cons_init);
  phy_space_add_constraint(SPACE, hinge_cons0);
  utl_array_set(r->bones, layer, &b);
}

void create_rig(rig *r, const size_t num_bones, const phy_rigid_body *bone_body, const int root_tex, const size_t init_layer) {
  r->enabled = true;
  r->num_bones = num_bones;
  r->bones = utl_array_create(sizeof(bone), num_bones);
  bone root_bone = create_bone(phy_rigid_body_get_position(bone_body), phy_vector2_zero, root_tex, init_layer, NULL);
  utl_array_set(r->bones, init_layer, &root_bone);
}

void free_rig(const rig *r) {
  for (bone* b = utl_array_begin(r->bones); b != (bone *)utl_array_end(r->bones); b++) {
    if (b->bone_constraint) {
      phy_constraint_free(b->bone_constraint);
    }
    phy_space_remove_rigidbody(SPACE, b->bone_body);
    phy_rigid_body_free(b->bone_body);
  }
  utl_array_deallocate(r->bones);
}

void render_rig(const rig *r, const float parallax_value, const bool flipped) {
  for (bone* b = utl_array_begin(r->bones); b != (bone *)utl_array_end(r->bones); b++) {
    phy_vector2 pos = phy_rigid_body_get_position(b->bone_body);
    phy_vector2 cam_pos = phy_vector2_new(CAMERA.position.x, CAMERA.position.y);
    printf("%f, %f\n", pos.x, pos.y);
    const float angle = phy_rigid_body_get_angle(b->bone_body);
    const gfx_texture tex_ref = get_or_add_texture(b->bone_tex_id);
    gfx_texture tex = { tex_ref.id, tex_ref.width, tex_ref.height, angle };
    transform_letterbox_element(LETTERBOX, &pos, &cam_pos, &tex);
    gfx_image_no_block(pos.x, pos.y, tex, 0.0f, 0.0f, cam_pos.x * parallax_value, cam_pos.y * parallax_value, CAMERA.zoom, true, flipped);
  }
}

