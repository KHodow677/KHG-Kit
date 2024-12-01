#pragma once

#include "khg_gfx/elements.h"
#include "khg_phy/core/phy_vector.h"

extern gfx_aabb LETTERBOX;

void transform_letterbox_element(const gfx_aabb letterbox, phy_vector2 *pos, phy_vector2 *cam_pos, gfx_texture *tex);

void get_letterbox(void);

