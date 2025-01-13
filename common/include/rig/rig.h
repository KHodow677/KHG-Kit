#pragma once

#include "khg_phy/body.h"
#include "khg_phy/core/phy_vector.h"
#include "khg_phy/shape.h"
#include "khg_utl/array.h"

struct bone;
typedef struct bone {
  bool updated;
  phy_rigid_body *bone_body;
  phy_shape *bone_shape;
  phy_vector2 bone_offset;
  float bone_angle_offset;
  unsigned int bone_tex_id;
  unsigned int layer;
  struct bone *parent;
} bone;

typedef struct rig {
  bool enabled;
  unsigned int num_bones;
  utl_array *bones;
  unsigned int root_id;
  utl_array *animation_states;
  utl_array *current_frame_bones;
  unsigned int current_state_id;
  unsigned int current_frame_id;
} rig;

typedef struct bone_info {
  unsigned int bone_tex;
  unsigned int bone_num;
  phy_vector2 bone_offset;
  unsigned int bone_parent_num;
} bone_info;

typedef struct rig_builder {
  bool valid;
  unsigned int num_bones;
  unsigned int root_tex;
  unsigned int init_layer;
  unsigned int num_anim;
} rig_builder;

bone create_bone(const phy_vector2 bone_offset, const float bone_angle_offset, const unsigned int tex_id, const unsigned int layer, bone *parent);
void add_bone(rig *r, const phy_vector2 bone_offset, const float bone_angle_offset, const unsigned int tex_id, const unsigned int layer, bone *parent);

void create_rig(rig *r, const rig_builder *rb);
void free_rig(const rig *r);

void update_rig(const rig *r, const phy_rigid_body *body, const float frame_percentage, utl_array *target, const bool flipped);
void render_rig(const rig *r, const float parallax_value, const bool flipped);

