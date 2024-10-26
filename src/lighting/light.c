#include "lighting/light.h"
#include "game.h"
#include "letterbox.h"
#include "lighting/framebuffer_shader.h"
#include "lighting/light_shader.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/internal.h"
#include "glad/glad.h"
#include "resources/texture_loader.h"

gfx_texture LIGHTING_OVERLAY = { 0 };
float LIGHTING_OVERLAY_COLOR[3] = { 0.0f, 0.0f, 0.0f };
gfx_shader PRIMARY_SHADER = { 0 };
gfx_shader LIGHTING_SHADER = { 0 };
gfx_shader FRAMEBUFFER_SHADER = { 0 };
int LIGHT_COUNT = 0;
light LIGHTS[1024];

void setup_lights_texture() {
  LIGHTING_OVERLAY = get_or_add_texture(SQUARE);
}

void setup_lights_shader() {
  PRIMARY_SHADER = state.render.shader;
  LIGHTING_SHADER = gfx_internal_shader_prg_create(light_vert_src, light_frag_src);
  FRAMEBUFFER_SHADER = gfx_internal_shader_prg_create(framebuffer_vert_src, framebuffer_frag_src);
}

void clear_lights() {
  LIGHT_COUNT = 0;
}

void add_light(vec2s pos_perc, float radius) {
  int i = LIGHT_COUNT++;
  LIGHTS[i] = (light){ pos_perc, radius};
}

void render_lights() {
  float scale = LETTERBOX.size.x / INITIAL_WIDTH;
  glUniform3f(glGetUniformLocation(state.render.shader.id, "u_light_color"), LIGHTING_OVERLAY_COLOR[0], LIGHTING_OVERLAY_COLOR[1], LIGHTING_OVERLAY_COLOR[2]);
  glUniform1i(glGetUniformLocation(state.render.shader.id, "u_num_lights"), LIGHT_COUNT);
  float light_position_percs[LIGHT_COUNT * 2];
  for (int i = 0; i < LIGHT_COUNT; i++) {
    light_position_percs[i * 2] = LIGHTS[i].pos_perc.x;
    light_position_percs[i * 2 + 1] = LIGHTS[i].pos_perc.y;
  }
  glUniform2fv(glGetUniformLocation(state.render.shader.id, "u_light_pos_percs"), LIGHT_COUNT, light_position_percs);
  float light_radii[LIGHT_COUNT];
  for (int i = 0; i < LIGHT_COUNT; i++) {
    light_radii[i] = LIGHTS[i].radius * scale;
  }
  glUniform1fv(glGetUniformLocation(state.render.shader.id, "u_light_radii"), LIGHT_COUNT, light_radii);
  LIGHTING_OVERLAY.width = LETTERBOX.size.x + 1;
  LIGHTING_OVERLAY.height = LETTERBOX.size.y + 1;
  gfx_image_no_block(gfx_get_display_width() * 0.5f, gfx_get_display_height() * 0.5f, LIGHTING_OVERLAY, 0, 0, 0, 0, 1, true);
}

