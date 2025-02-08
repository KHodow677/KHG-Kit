#pragma once

#include "khg_gfx/internal.h"
#include "khg_gfx/texture.h"
#include "khg_kin/namespace.h"

#define MAX_LIGHTS 256

typedef struct {
  kin_vec pos_perc;
  float intensity;
} light;

extern gfx_texture LIGHTING_OVERLAY;
extern float LIGHTING_OVERLAY_COLOR[3];
extern gfx_shader PRIMARY_SHADER;
extern gfx_shader LIGHTING_SHADER;
extern gfx_shader FRAMEBUFFER_SHADER;
extern int LIGHT_COUNT;
extern light LIGHTS[MAX_LIGHTS];

void setup_lights_texture(void);
void setup_lights_shader(void);

void clear_lights();
void add_light(kin_vec pos_perc, float radius);

void render_lights(void);

