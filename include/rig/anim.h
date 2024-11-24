#pragma once

#include "khg_phy/core/phy_vector.h"

typedef struct frame_info {
  int bone_tex;
  phy_vector2 bone_offset;
} frame_info;

bool generate_animation_frame(const char *dir_path, const int anim_num, const int frame_num);
void generate_animation_from_path(const char *dir_path, const int anim_num);

