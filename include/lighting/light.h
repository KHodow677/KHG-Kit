#pragma once

#include "khg_gfx/internal.h"
#include "khg_gfx/texture.h"

extern gfx_texture LIGHTING_OVERLAY;
extern gfx_shader LIGHTING_SHADER;

void setup_lights_texture(void);
void setup_lights_shader(void);
void render_lights(void);
