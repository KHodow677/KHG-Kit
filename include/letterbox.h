#pragma once

#include "khg_gfx/elements.h"
#include "khg_phy/vector.h"

extern gfx_aabb LETTERBOX;

void transform_letterbox_element(const gfx_aabb letterbox, nvVector2 *pos, nvVector2 *cam_pos, gfx_texture *tex);

void get_letterbox(void);

