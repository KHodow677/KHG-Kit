#pragma once

#include "khg_gfx/elements.h"
#include "khg_kin/namespace.h"

extern gfx_aabb LETTERBOX;

void transform_letterbox_element_tex(const gfx_aabb letterbox, kin_vec *pos, kin_vec *cam_pos, gfx_texture *tex);
void transform_letterbox_element_aabb(const gfx_aabb letterbox, kin_vec *pos, kin_vec *cam_pos, gfx_aabb *aabb);

void get_letterbox(void);
