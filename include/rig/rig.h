#pragma once

#include "khg_phy/body.h"
#include "khg_phy/core/phy_vector.h"
#include "khg_phy/shape.h"
#include "khg_utl/array.h"

struct bone;
typedef struct bone {
  phy_rigid_body *bone_body;
  phy_shape *bone_shape;
  phy_vector2 bone_offset;
  int bone_tex_id;
  int layer;
  struct bone *parent;
} bone;

typedef struct bone_joint_info {
  phy_vector2 bone_pos;
} bone_joint_info;

typedef struct rig {
  bool enabled;
  size_t num_bones;
  utl_array *bones;
  phy_vector2 root_offset;
} rig;

typedef struct rig_builder {
  const size_t num_bones;
  const int root_tex;
  const size_t init_layer;
  const phy_vector2 root_offset;
} rig_builder;

bone create_bone(const phy_vector2 bone_offset, const int tex_id, const int layer, bone *parent);
void add_bone(rig *r, const phy_vector2 bone_offset, const int tex_id, const int layer, bone *parent);

void create_rig(rig *r, const size_t num_bones, const phy_rigid_body *bone_body, const phy_vector2 root_offset, const int root_tex, const size_t init_layer);
void create_rig_from_file(rig *r, const char *filepath);
void free_rig(const rig *r);
void render_rig(const rig *r, const float parallax_value, const bool flipped);

