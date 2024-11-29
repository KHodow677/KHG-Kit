#pragma once

#include "rig/rig.h"
#include "khg_phy/core/phy_vector.h"

typedef struct frame_info {
  int bone_tex;
  phy_vector2 bone_offset;
  float bone_angle_offset;
} bone_frame_info;

const bone_frame_info *get_bone_frame_info(const rig *r, const int state_id, const int frame_id);
const int last_frame_num(rig *r, const int state_id);

void generate_animation_frame(rig *r, const char *dir_path, const char *rig_section, const int anim_num, const int num_frames, const int frame_num);
void generate_animation_from_path(rig *r, const char *dir_path, const char *section, const int anim_num, const int num_frames);
void set_state_and_frame(rig *r, const int state_id, const int frame_id);

