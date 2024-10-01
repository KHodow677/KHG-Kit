#include "game.h"
#include "khg_gfx/internal.h"
#include "khg_gfx/texture.h"
#include "khg_gfx/ui.h"
#include "khg_gfx/elements.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
  "v_color = a_color;\n"
  "v_texcoord = a_texcoord;\n"
  "v_tex_index = a_tex_index;\n"
  "v_border_color = a_border_color;\n"
  "v_border_width = a_border_width;\n"
  "v_scale = a_scale;\n"
  "v_pos_px = a_pos_px;\n"
  "v_corner_radius = a_corner_radius;\n"
  "v_min_coord = a_min_coord;\n"
  "v_max_coord = a_max_coord;\n"
  "gl_Position = u_proj * vec4(a_pos.x, a_pos.y, 0.0f, 1.0);\n"
  "}\n";

static const char* frag_src = 
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
  "in vec2 v_frag_pos;\n"
  "uniform sampler2D u_textures[32];\n"
  "uniform vec2 u_screen_size;\n"
  "in vec2 v_min_coord;\n"
  "in vec2 v_max_coord;\n"
  "uniform vec3 u_light_color = vec3(1.0, 1.0, 1.0);\n"
  "uniform vec2 u_light_pos;\n"
  "uniform float u_light_intensity = 1.0;\n"
  "uniform float u_light_radius = 300.0;\n"
  "float rounded_box_sdf(vec2 center_pos, vec2 size, float radius) {\n"
  "  return length(max(abs(center_pos)-size+radius,0.0))-radius;\n"
  "}\n"
  "void main() {\n"
  "  // Light positioning and distance to light\n"
  "  vec2 light_pos = vec2(v_pos_px.x + v_scale.x / 2.0, u_screen_size.y - (v_pos_px.y + v_scale.y / 2.0));\n"
  "  float dist_to_light = distance(gl_FragCoord.xy, light_pos);\n"
  
  "  // Calculate attenuation based on distance\n"
  "  float attenuation = clamp(1.0 - (dist_to_light / u_light_radius), 0.0, 1.0);\n"
  
  "  // Calculate color and transparency for the light\n"
  "  vec4 base_color = (v_tex_index == -1) ? v_color : texture(u_textures[int(v_tex_index)], v_texcoord) * v_color;\n"
  
  "  // Set transparency based on the distance to the light (more transparent near the light center)\n"
  "  float transparency = 1.0 - attenuation; // More transparent as it gets closer to the light\n"
  
  "  // Final color blending\n"
  "  vec3 final_color = mix(vec3(0.0), base_color.rgb, u_light_intensity * attenuation);\n"
  
  "  // Handle corner radius if necessary\n"
  "  if (v_corner_radius != 0.0f) {\n"
  "    vec2 location = vec2(v_pos_px.x, -v_pos_px.y);\n"
  "    location.y += u_screen_size.y - v_scale.y;\n"
  "    float edge_softness = 1.0f;\n"
  "    float radius = v_corner_radius * 2.0f;\n"
  "    float distance = rounded_box_sdf(gl_FragCoord.xy - location - (v_scale / 2.0f), v_scale / 2.0f, radius);\n"
  "    float smoothed_alpha = 1.0f - smoothstep(0.0f, edge_softness * 2.0f, distance);\n"
  "    o_color = vec4(final_color, smoothed_alpha * transparency);\n"
  "  } else {\n"
  "    o_color = vec4(final_color, base_color.a * transparency);\n"
  "  }\n"
  "}\n";

static gfx_shader primary_shader;
static gfx_shader alternate_shader;
static gfx_texture square;

void log_sys_info() {
  printf("OS: %s\n", OS_NAME);
  const GLubyte *vendor = glGetString(GL_VENDOR);
  const GLubyte *version = glGetString(GL_VERSION);
  if (vendor != NULL && version != NULL) {
    printf("Vendor: %s\n", vendor);
    printf("OpenGL Version: %s\n", version);
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
  square = gfx_load_texture_asset("square", "png");
  int res = gfx_loop_manager(window, false);
  return res;
}

bool gfx_loop(float delta) {
  float gray_color = 35.0f / 255.0f;
  glClearColor(gray_color, gray_color, gray_color, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  gfx_begin();
  return true;
}

bool gfx_loop_post(float delta) {
  gfx_begin();
  state.render.shader = alternate_shader;
  gfx_image_no_block(400, 400, square, 0, 0, 0, 0, 1, true);
  return true;
};

