#pragma once

#include "khg_gfx/elements.h"
#include "khg_phy/phy_types.h"

extern gfx_aabb LETTERBOX;

void transform_letterbox_element(gfx_aabb letterbox, phy_vect *pos, gfx_texture *tex, float offset_x, float offset_y);

void get_letterbox(void);

