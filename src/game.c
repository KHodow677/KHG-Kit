#include "game.h"
#include "khg_gfx/internal.h"
#include "khg_gfx/texture.h"
#include "khg_gfx/ui.h"
#include "khg_gfx/elements.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

static const char* vert_src =
  "#version 450 core\n"
  "layout (location = 0) in vec2 a_pos;\n"
  "layout (location = 1) in vec4 a_border_color;\n"
  "layout (location = 2) in float a_border_width;\n"
  "layout (location = 3) in vec4 a_color;\n"
  "layout (location = 4) in vec2 a_texcoord;\n"
  "layout (location = 5) in float a_tex_index;\n"
  "layout (location = 6) in vec2 a_scale;\n"
  "layout (location = 7) in vec2 a_pos_px;\n"
  "layout (location = 8) in float a_corner_radius;\n"
  "layout (location = 10) in vec2 a_min_coord;\n"
  "layout (location = 11) in vec2 a_max_coord;\n"
  "uniform mat4 u_proj;\n"
  "out vec4 v_border_color;\n"
  "out float v_border_width;\n"
  "out vec4 v_color;\n"
  "out vec2 v_texcoord;\n"
  "out float v_tex_index;\n"
  "flat out vec2 v_scale;\n"
  "flat out vec2 v_pos_px;\n"
  "flat out float v_is_gradient;\n"
  "out float v_corner_radius;\n"
  "out vec2 v_min_coord;\n"
  "out vec2 v_max_coord;\n"
  "void main() {\n"
  "  v_color = a_color;\n"
  "  v_texcoord = a_texcoord;\n"
  "  v_tex_index = a_tex_index;\n"
  "  v_border_color = a_border_color;\n"
  "  v_border_width = a_border_width;\n"
  "  v_scale = a_scale;\n"
  "  v_pos_px = a_pos_px;\n"
  "  v_corner_radius = a_corner_radius;\n"
  "  v_min_coord = a_min_coord;\n"
  "  v_max_coord = a_max_coord;\n"
  "  gl_Position = u_proj * vec4(a_pos.x, a_pos.y, 0.0f, 1.0);\n"
  "}\n";

static const char *frag_src = 
  "#version 450 core\n"
  "out vec4 o_color;\n"
  "in vec4 v_color;\n"
  "in float v_tex_index;\n"
  "in vec4 v_border_color;\n"
  "in float v_border_width;\n"
  "in vec2 v_texcoord;\n"
  "flat in vec2 v_scale;\n"
  "flat in vec2 v_pos_px;\n"
  "in float v_corner_radius;\n"
  "uniform sampler2D u_textures[32];\n"
  "uniform vec2 u_screen_size;\n"
  "in vec2 v_min_coord;\n"
  "in vec2 v_max_coord;\n"
  "const int num_lights = 2;\n"
  "uniform vec3 u_light_colors[num_lights];\n"
  "uniform vec2 u_light_pos_percs[num_lights];\n"
  "uniform float u_light_intensities[num_lights];\n"
  "uniform float u_light_radii[num_lights];\n"
  "float rounded_box_sdf(vec2 center_pos, vec2 size, float radius) {\n"
  "  return length(max(abs(center_pos)-size+radius,0.0))-radius;\n"
  "}\n"
  "void main() {\n"
  "  if (u_screen_size.y - gl_FragCoord.y < v_min_coord.y && v_min_coord.y != -1) {\n"
  "    discard;\n"
  "  }\n"
  "  if (u_screen_size.y - gl_FragCoord.y > v_max_coord.y && v_max_coord.y != -1) {\n"
  "    discard;\n"
  "  }\n"
  "  if ((gl_FragCoord.x < v_min_coord.x && v_min_coord.x != -1) || (gl_FragCoord.x > v_max_coord.x && v_max_coord.x != -1)) {\n"
  "    discard;\n" 
  "  }\n"
  "  vec2 size = v_scale;\n"
  "  vec4 opaque_color, display_color;\n"
  "  if (v_tex_index == -1) {\n"
  "    opaque_color = v_color;\n"
  "  }\n" 
  "  else {\n"
  "    opaque_color = texture(u_textures[int(v_tex_index)], v_texcoord) * v_color;\n"
  "  }\n"
  "  if (v_corner_radius != 0.0f) {"
  "    display_color = opaque_color;\n"
  "    vec2 location = vec2(v_pos_px.x, -v_pos_px.y);\n"
  "    location.y += u_screen_size.y - size.y;\n"
  "    float edge_softness = 1.0f;\n"
  "    float radius = v_corner_radius * 2.0f;\n"
  "    float distance = rounded_box_sdf(gl_FragCoord.xy - location - (size/2.0f), size / 2.0f, radius);\n"
  "    float smoothed_alpha = 1.0f-smoothstep(0.0f, edge_softness * 2.0f,distance);\n"
  "    vec3 fill_color;\n"
  "    if (v_border_width != 0.0f) {\n"
  "      vec2 location_border = vec2(location.x + v_border_width, location.y + v_border_width);\n"
  "      vec2 size_border = vec2(size.x - v_border_width * 2, size.y - v_border_width * 2);\n"
  "      float distance_border = rounded_box_sdf(gl_FragCoord.xy - location_border - (size_border / 2.0f), size_border / 2.0f, radius);\n"
  "      if (distance_border <= 0.0f) {\n"
  "        fill_color = display_color.xyz;\n"
  "      }\n"
  "      else {\n"
  "        fill_color = v_border_color.xyz;\n"
  "      }\n"
  "    }\n" 
  "    else {\n"
  "      fill_color = display_color.xyz;\n"
  "    }\n"
  "    if (v_border_width != 0.0f)\n" 
  "      o_color =  mix(vec4(0.0f, 0.0f, 0.0f, 0.0f), vec4(fill_color, smoothed_alpha), smoothed_alpha);\n"
  "    else\n" 
  "      o_color = mix(vec4(0.0f, 0.0f, 0.0f, 0.0f), vec4(fill_color, display_color.a), smoothed_alpha);\n"
  "  }\n"
  "  else {\n"
  "    vec4 fill_color = opaque_color;\n"
  "    if (v_border_width != 0.0f) {\n"
  "      vec2 location = vec2(v_pos_px.x, -v_pos_px.y);\n"
  "      location.y += u_screen_size.y - size.y;\n"
  "      vec2 location_border = vec2(location.x + v_border_width, location.y + v_border_width);\n"
  "      vec2 size_border = vec2(v_scale.x - v_border_width * 2, v_scale.y - v_border_width * 2);\n"
  "      float distance_border = rounded_box_sdf(gl_FragCoord.xy - location_border - (size_border / 2.0f), size_border / 2.0f, v_corner_radius);\n"
  "      if (distance_border > 0.0f) {\n"
  "        fill_color = v_border_color;\n"
  "      }\n"
  "    }\n"
  "    o_color = fill_color;\n"
  "  }\n"
  "  for (int i = 0; i < num_lights; i++) {\n"
  "    vec3 u_light_color = u_light_colors[i];\n"
  "    vec2 u_light_pos_perc = u_light_pos_percs[i];\n"
  "    float u_light_intensity = u_light_intensities[i];\n"
  "    float u_light_radius = u_light_radii[i];\n"
  "    vec2 light_pos = vec2(v_pos_px.x + v_scale.x * u_light_pos_perc.x, u_screen_size.y - (v_pos_px.y + v_scale.y * u_light_pos_perc.y));\n"
  "    float dist_to_light = distance(gl_FragCoord.xy, light_pos);\n"
  "    float attenuation = clamp(1.0 - (dist_to_light / u_light_radius), 0.0, 1.0);\n"
  "    float transparency = 1.0 - attenuation; // More transparent as it gets closer to the light\n"
  "    vec3 final_color = mix(vec3(0.0), o_color.rgb, u_light_intensity * attenuation);\n"
  "    o_color = vec4(final_color, o_color.a * transparency);\n"
  "  }\n"
  "}\n";

static gfx_shader primary_shader;
static gfx_shader alternate_shader;
static gfx_texture tex;
static gfx_texture square;
static gfx_font font;
static uint32_t original_font_size;

void log_sys_info() {
  printf("OS: %s\n", OS_NAME);
  const GLubyte *vendor = glGetString(GL_VENDOR);
  const GLubyte *version = glGetString(GL_VERSION);
  if (vendor != NULL && version != NULL) {
    printf("Vendor: %s\n", vendor);
    printf("OpenGL Version: %s\n", version);
  }
}

static void update_font() {
  uint32_t min_change = fminf((uint32_t)(gfx_get_display_width() / 1280.0f * original_font_size), (uint32_t)(gfx_get_display_height() / 720.0f * original_font_size));
  if (font.font_size != min_change) {
    gfx_free_font(&font);
    font = gfx_load_font_asset("rubik", "ttf", min_change);
  }
}

int game_run() {
  if (!glfwInit()) {
    return -1;
  }
  GLFWwindow *window = glfwCreateWindow(1280, 720, "Game", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  gfx_init_glfw(1280, 720, window);
  primary_shader = state.render.shader;
  alternate_shader = gfx_internal_shader_prg_create(vert_src, frag_src);
  tex = gfx_load_texture_asset("creature_spawner", "png");
  square = gfx_load_texture_asset("square", "png");
  font = gfx_load_font_asset("rubik", "ttf", 24);
  original_font_size = font.font_size;
  int res = gfx_loop_manager(window, false);
  return res;
}

bool gfx_loop(float delta) {
  float gray_color = 35.0f / 255.0f;
  glClearColor(gray_color, gray_color, gray_color, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  gfx_begin();
  gfx_internal_renderer_set_shader(primary_shader);
  update_font();
  gfx_push_font(&font);
  gfx_text("Test the Font");
  gfx_pop_font();
  gfx_image_no_block(400, 400, tex, 0, 0, 0, 0, 1, true);
  state.current_div.scrollable = false;
  return true;
}

bool gfx_loop_post(float delta) {
  gfx_begin();
  gfx_internal_renderer_set_shader(alternate_shader);
  int numLights = 2;
  float lightColors[] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
  glUniform3fv(glGetUniformLocation(state.render.shader.id, "u_light_colors"), numLights, lightColors);
  float lightPositionPercs[] = { 0.5f, 0.7f, 0.5, 0.0f };
  glUniform2fv(glGetUniformLocation(state.render.shader.id, "u_light_pos_percs"), numLights, lightPositionPercs);
  float lightIntensities[] = { 1.0f, 1.0f };
  glUniform1fv(glGetUniformLocation(state.render.shader.id, "u_light_intensities"), numLights, lightIntensities);
  float lightRadii[] = { 300.0f, 300.0f };
  glUniform1fv(glGetUniformLocation(state.render.shader.id, "u_light_radii"), numLights, lightRadii);
  square.width = gfx_get_display_width();
  square.height = gfx_get_display_height();
  gfx_image_no_block(gfx_get_display_width() * 0.5f, gfx_get_display_height() * 0.5f, square, 0, 0, 0, 0, 1, true);
  state.current_div.scrollable = false;
  return true;
};

bool gfx_loop_ui(float delta) {
  gfx_begin();
  gfx_internal_renderer_set_shader(primary_shader);
  state.current_div.scrollable = false;
  return true;
};
