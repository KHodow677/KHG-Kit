#pragma once

#include "rig/rig.h"
#include <stdbool.h>

typedef struct element_frame {
  unsigned int bone_tex;
  phy_vector2 bone_offset;
  float bone_angle_offset;
} element_frame;

typedef struct element_rig element_rig;
struct element_rig {
  bool enabled;
  bone single_bone;
  element_rig *child_rig;
  unsigned int current_frame_id;
};

