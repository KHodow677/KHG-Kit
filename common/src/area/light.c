#include "area/light.h"
#include "game.h"
#include "glad/glad.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/internal.h"
#include "khg_utl/file_reader.h"
#include "letterbox.h"
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
  utl_file_reader *reader = utl_file_reader_open("res/assets/shaders/vertex/primary.vert", UTL_READ_TEXT);
  char light_vert_src[(int)pow(2, 18)];
  utl_file_reader_read(light_vert_src, sizeof(char), sizeof(light_vert_src) - 1, reader);
  utl_file_reader_close(reader);
  reader = utl_file_reader_open("res/assets/shaders/fragment/light.frag", UTL_READ_TEXT);
  char light_frag_src[(int)pow(2, 18)];
  utl_file_reader_read(light_frag_src, sizeof(char), sizeof(light_frag_src) - 1, reader);
  utl_file_reader_close(reader);
  LIGHTING_SHADER = gfx_internal_shader_prg_create(light_vert_src, light_frag_src);
  reader = utl_file_reader_open("res/assets/shaders/vertex/framebuffer.vert", UTL_READ_TEXT);
  char framebuffer_vert_src[(int)pow(2, 14)];
  utl_file_reader_read(framebuffer_vert_src, sizeof(char), sizeof(framebuffer_vert_src) - 1, reader);
  utl_file_reader_close(reader);
  reader = utl_file_reader_open("res/assets/shaders/fragment/framebuffer.frag", UTL_READ_TEXT);
  char framebuffer_frag_src[(int)pow(2, 14)];
  utl_file_reader_read(framebuffer_frag_src, sizeof(char), sizeof(framebuffer_frag_src) - 1, reader);
  utl_file_reader_close(reader);
  FRAMEBUFFER_SHADER = gfx_internal_shader_prg_create(framebuffer_vert_src, framebuffer_frag_src);
}

void clear_lights() {
  LIGHT_COUNT = 0;
}

void add_light(vec2s pos_perc, float radius) {
  const int i = LIGHT_COUNT++;
  LIGHTS[i] = (light){ pos_perc, radius};
}

void render_lights() {
  const float scale = LETTERBOX.size.x / INITIAL_WIDTH;
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
    light_radii[i] = LIGHTS[i].intensity * scale;
  }
  glUniform1fv(glGetUniformLocation(state.render.shader.id, "u_light_intensities"), LIGHT_COUNT, light_radii);
  LIGHTING_OVERLAY.width = LETTERBOX.size.x + 1;
  LIGHTING_OVERLAY.height = LETTERBOX.size.y + 1;
  gfx_image_no_block(gfx_get_display_width() * 0.5f, gfx_get_display_height() * 0.5f, LIGHTING_OVERLAY, 0, 0, 1, true, false);
}

