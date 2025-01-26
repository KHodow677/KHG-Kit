#pragma once

#include "rig/rig.h"
#include "khg_phy/core/phy_vector.h"

typedef struct bone_frame_info {
  unsigned int bone_tex;
  phy_vector2 bone_offset;
  float bone_angle_offset;
} bone_frame_info;

const bone_frame_info *get_bone_frame_info(const rig *r, const unsigned int state_id, const unsigned int frame_id);
const unsigned int last_frame_num(rig *r, const unsigned int state_id);

void update_rig_with_interpolated_frame(bone *b, const bone_frame_info *current, const bone_frame_info *target, const float frame_percentage, const phy_rigid_body *root_body, const bool flipped);

void set_state_and_frame(rig *r, const unsigned int state_id, const unsigned int frame_id);
utl_array *get_frame(const rig *r, const unsigned int state_id, const unsigned int frame_id);

