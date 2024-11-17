#pragma once

#include "khg_phy/body.h"
#include "khg_phy/core/phy_vector.h"
#include "khg_phy/shape.h"
#include "khg_utl/array.h"

struct bone;
typedef struct bone {
  phy_rigid_body *bone_body;
  phy_shape *bone_shape;
  phy_vector2 bone_pos_offset;
  phy_vector2 bone_offset;
  int bone_tex_id;
  int layer;
  struct bone *parent;
} bone;

typedef struct bone_joint_pair {
  phy_vector2 bone_pos;
  phy_vector2 bone_offset;
} bone_joint_pair;

typedef struct rig {
  bool enabled;
  size_t num_bones;
  utl_array *bones;
} rig;

typedef struct rig_builder {
  const size_t num_bones;
  const int root_tex;
  const size_t init_layer;
} rig_builder;

bone create_bone(bone_joint_pair joint_info, const int tex_id, const int layer, bone *parent);
void add_bone(rig *r, const bone_joint_pair joint_info, const int tex_id, const int layer, bone *parent);

void create_rig(rig *r, const size_t num_bones, const phy_rigid_body *bone_body, const int root_tex, const size_t init_layer);
void free_rig(const rig *r);
void render_rig(const rig *r, const float parallax_value, const bool flipped);

