#pragma once

#include "rig/rig.h"

void generate_animation_frame(rig *r, const char *dir_path, const char *rig_section, const int anim_num, const int num_frames, const int frame_num);
void generate_animation_from_path(rig *r, const char *dir_path, const char *section, const int anim_num, const int num_frames);

