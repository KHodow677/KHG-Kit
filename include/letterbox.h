#pragma once

#include "khg_gfx/elements.h"
#include "khg_phy/phy_types.h"

extern gfx_aabb LETTERBOX;

void transform_letterbox_element(gfx_aabb letterbox, phy_vect *pos, phy_vect *cam_pos, gfx_texture *tex);

void get_letterbox(void);

