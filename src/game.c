#include "game.h"
#include "khg_gfx/internal.h"
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
  "uniform sampler2D u_textures[32];\n"
  "uniform vec2 u_screen_size;\n"
  "in vec2 v_min_coord;\n"
  "in vec2 v_max_coord;\n"
  "float rounded_box_sdf(vec2 center_pos, vec2 size, float radius) {\n"
  "    return length(max(abs(center_pos)-size+radius,0.0))-radius;\n"
  "}\n"
  "void main() {\n"
  "     if(u_screen_size.y - gl_FragCoord.y < v_min_coord.y && v_min_coord.y != -1) {\n"
  "         discard;\n"
  "     }\n"
  "     if(u_screen_size.y - gl_FragCoord.y > v_max_coord.y && v_max_coord.y != -1) {\n"
  "         discard;\n"
  "     }\n"
  "     if ((gl_FragCoord.x < v_min_coord.x && v_min_coord.x != -1) || (gl_FragCoord.x > v_max_coord.x && v_max_coord.x != -1)) {\n"
  "         discard;\n" 
  "     }\n"
  "     vec2 size = v_scale;\n"
  "     vec4 opaque_color, display_color;\n"
  "     if(v_tex_index == -1) {\n"
  "       opaque_color = v_color;\n"
  "     } else {\n"
  "       opaque_color = texture(u_textures[int(v_tex_index)], v_texcoord) * v_color;\n"
  "     }\n"
  "     if(opaque_color.a > 0.0f) {\n"
  "       opaque_color.rgb = vec3(0.0);\n"
  "     }\n"
  "     if(v_corner_radius != 0.0f) {"
  "       display_color = opaque_color;\n"
  "       vec2 location = vec2(v_pos_px.x, -v_pos_px.y);\n"
  "       location.y += u_screen_size.y - size.y;\n"
  "       float edge_softness = 1.0f;\n"
  "       float radius = v_corner_radius * 2.0f;\n"
  "       float distance = rounded_box_sdf(gl_FragCoord.xy - location - (size/2.0f), size / 2.0f, radius);\n"
  "       float smoothed_alpha = 1.0f-smoothstep(0.0f, edge_softness * 2.0f,distance);\n"
  "       vec3 fill_color;\n"
  "       if(v_border_width != 0.0f) {\n"
  "           vec2 location_border = vec2(location.x + v_border_width, location.y + v_border_width);\n"
  "           vec2 size_border = vec2(size.x - v_border_width * 2, size.y - v_border_width * 2);\n"
  "           float distance_border = rounded_box_sdf(gl_FragCoord.xy - location_border - (size_border / 2.0f), size_border / 2.0f, radius);\n"
  "           if(distance_border <= 0.0f) {\n"
  "               fill_color = display_color.xyz;\n"
  "           } else {\n"
  "               fill_color = v_border_color.xyz;\n"
  "           }\n"
  "       } else {\n"
  "           fill_color = display_color.xyz;\n"
  "       }\n"
  "       if(v_border_width != 0.0f)\n" 
  "         o_color =  mix(vec4(0.0f, 0.0f, 0.0f, 0.0f), vec4(fill_color, smoothed_alpha), smoothed_alpha);\n"
  "       else\n" 
  "         o_color = mix(vec4(0.0f, 0.0f, 0.0f, 0.0f), vec4(fill_color, display_color.a), smoothed_alpha);\n"
  "     } else {\n"
  "       vec4 fill_color = opaque_color;\n"
  "       if(v_border_width != 0.0f) {\n"
  "           vec2 location = vec2(v_pos_px.x, -v_pos_px.y);\n"
  "           location.y += u_screen_size.y - size.y;\n"
  "           vec2 location_border = vec2(location.x + v_border_width, location.y + v_border_width);\n"
  "           vec2 size_border = vec2(v_scale.x - v_border_width * 2, v_scale.y - v_border_width * 2);\n"
  "           float distance_border = rounded_box_sdf(gl_FragCoord.xy - location_border - (size_border / 2.0f), size_border / 2.0f, v_corner_radius);\n"
  "           if(distance_border > 0.0f) {\n"
  "               fill_color = v_border_color;\n"
  "}\n"
  "       }\n"
  "       o_color = fill_color;\n"
  " }\n"
  "}\n";

static gfx_shader alt_shader;

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
  alt_shader = gfx_internal_shader_prg_create(vert_src, frag_src);
  int res = gfx_loop_manager(window, false);
  return res;
}

bool gfx_loop(float delta) {
  float gray_color = 35.0f / 255.0f;
  glClearColor(gray_color, gray_color, gray_color, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  gfx_begin();
  state.render.shader = alt_shader;
  gfx_image(state.tex_arrow_down);
  return true;
}

