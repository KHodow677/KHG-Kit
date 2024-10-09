#pragma once

#include "khg_gfx/internal.h"
#include "khg_gfx/texture.h"
#include "cglm/types-struct.h"

typedef struct {
  vec2s pos_perc;
  float radius;
} light;

extern gfx_texture LIGHTING_OVERLAY;
extern gfx_shader LIGHTING_SHADER;
extern int LIGHT_COUNT;
extern light LIGHTS[1024];

void setup_lights_texture(void);
void setup_lights_shader(void);

void clear_lights();
void add_light(vec2s pos_perc, float radius);

void render_lights(void);
