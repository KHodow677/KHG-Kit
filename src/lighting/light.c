#include "lighting/light.h"
#include "khg_gfx/internal.h"
#include "glad/glad.h"
#include "lighting/lighting_shader.h"

gfx_texture LIGHTING_OVERLAY = { 0 };
gfx_shader LIGHTING_SHADER = { 0 };

void setup_lights_texture() {
  LIGHTING_OVERLAY = gfx_load_texture_asset("square", "png");
}

void setup_lights_shader() {
  LIGHTING_SHADER = gfx_internal_shader_prg_create(lighting_vert_src, lighting_frag_src);
}

void render_lights() {
  int num_lights = 2;
  glUniform1i(glGetUniformLocation(state.render.shader.id, "u_num_lights"), num_lights);
  float light_colors[] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
  glUniform3fv(glGetUniformLocation(state.render.shader.id, "u_light_colors"), num_lights, light_colors);
  float light_position_percs[] = { 0.5f, 0.7f, 0.5, 0.0f };
  glUniform2fv(glGetUniformLocation(state.render.shader.id, "u_light_pos_percs"), num_lights, light_position_percs);
  float light_intensities[] = { 1.0f, 1.0f };
  glUniform1fv(glGetUniformLocation(state.render.shader.id, "u_light_intensities"), num_lights, light_intensities);
  float light_radii[] = { 300.0f, 300.0f };
  glUniform1fv(glGetUniformLocation(state.render.shader.id, "u_light_radii"), num_lights, light_radii);
  LIGHTING_OVERLAY.width = gfx_get_display_width();
  LIGHTING_OVERLAY.height = gfx_get_display_height();
  gfx_image_no_block(gfx_get_display_width() * 0.5f, gfx_get_display_height() * 0.5f, LIGHTING_OVERLAY, 0, 0, 0, 0, 1, true);
}
