#include "lighting/light.h"
#include "khg_gfx/internal.h"
#include "glad/glad.h"
#include "lighting/lighting_shader.h"

gfx_texture LIGHTING_OVERLAY = { 0 };
gfx_shader LIGHTING_SHADER = { 0 };
int LIGHT_COUNT = 0;
light LIGHTS[1024] = { 0 };

void setup_lights_texture() {
  LIGHTING_OVERLAY = gfx_load_texture_asset("square", "png");
}

void setup_lights_shader() {
  LIGHTING_SHADER = gfx_internal_shader_prg_create(lighting_vert_src, lighting_frag_src);
}

void clear_lights() {
  LIGHT_COUNT = 0;
}

void add_light(vec2s pos_perc, float intensity, float radius) {
  int i = LIGHT_COUNT++;
  LIGHTS[i] = (light){ pos_perc, intensity, radius};
}

void render_lights() {
  glUniform1i(glGetUniformLocation(state.render.shader.id, "u_num_lights"), LIGHT_COUNT);
  float light_position_percs[LIGHT_COUNT * 2];
  for (int i = 0; i < LIGHT_COUNT; i++) {
    light_position_percs[i * 2] = LIGHTS[i].pos_perc.x;
    light_position_percs[i * 2 + 1] = LIGHTS[i].pos_perc.y;
  }
  glUniform2fv(glGetUniformLocation(state.render.shader.id, "u_light_pos_percs"), LIGHT_COUNT, light_position_percs);
  float light_intensities[LIGHT_COUNT];
  for (int i = 0; i < LIGHT_COUNT; i++) {
    light_intensities[i] = LIGHTS[i].intensity;
  }
  glUniform1fv(glGetUniformLocation(state.render.shader.id, "u_light_intensities"), LIGHT_COUNT, light_intensities);
  float light_radii[LIGHT_COUNT];
  for (int i = 0; i < LIGHT_COUNT; i++) {
    light_radii[i] = LIGHTS[i].radius;
  }
  glUniform1fv(glGetUniformLocation(state.render.shader.id, "u_light_radii"), LIGHT_COUNT, light_radii);
  LIGHTING_OVERLAY.width = gfx_get_display_width();
  LIGHTING_OVERLAY.height = gfx_get_display_height();
  gfx_image_no_block(gfx_get_display_width() * 0.5f, gfx_get_display_height() * 0.5f, LIGHTING_OVERLAY, 0, 0, 0, 0, 1, true);
}