#include "khg_ui/ui.h"
#include "khg_ui/elements.h"
#include "khg_ui/events.h"
#include "khg_ui/texture.h"
#include "khg_utils/error_func.h"
#include "cglm/mat4.h"
#include "cglm/types-struct.h"
#include "glad/glad.h"
#include "stb_image/stb_image.h"
#include "stb_image_resize/stb_image_resize2.h"
#include "stb_truetype/stb_truetype.h"
#include "GLFW/glfw3.h"
#include "libclipboard/libclipboard.h"
#include <locale.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <wchar.h>
#include <wctype.h>

#ifdef _WIN32
#define HOMEDIR "USERPROFILE"
#else
#define HOMEDIR (char*)"HOME"
#endif

#define UI_STACK_INIT_CAP 4

#define MAX_KEYS GLFW_KEY_LAST
#define MAX_MOUSE_BUTTONS GLFW_MOUSE_BUTTON_LAST
#define KEY_CALLBACK_t GLFWkeyfun
#define MOUSE_BUTTON_CALLBACK_t GLFWmousebuttonfun
#define SCROLL_CALLBACK_t GLFWscrollfun
#define CURSOR_CALLBACK_t GLFWcursorposfun

#define MAX_RENDER_BATCH 10000
#define MAX_TEX_COUNT_BATCH 32
#define MAX_KEY_CALLBACKS 4
#define MAX_MOUSE_BTTUON_CALLBACKS 4
#define MAX_SCROLL_CALLBACKS 4
#define MAX_CURSOR_POS_CALLBACKS 4

#define DJB2_INIT 5381

typedef struct {
  uint32_t id;
} ui_shader;

typedef struct {
  vec2 pos;
  vec4 border_color;
  float border_width;
  vec4 color;
  vec2 texcoord;
  float tex_index;
  vec2 scale;
  vec2 pos_px;
  float corner_radius;
  vec2 min_coord, max_coord;
} ui_vertex;

typedef struct {
  bool keys[MAX_KEYS];
  bool keys_changed[MAX_KEYS];
} ui_keyboard;

typedef struct {
  bool buttons_current[MAX_MOUSE_BUTTONS];
  bool buttons_last[MAX_MOUSE_BUTTONS];

  double xpos, ypos, xpos_last, ypos_last, xpos_delta, ypos_delta;
  bool first_mouse_press; 
  double xscroll_delta, yscroll_delta;
} ui_mouse;

typedef struct {
  bool is_dragging;
  vec2s start_cursor_pos;
  float start_scroll;
} ui_drag_state;

typedef struct {
  ui_keyboard keyboard;
  ui_mouse mouse;
  KEY_CALLBACK_t key_cbs[MAX_KEY_CALLBACKS];
  MOUSE_BUTTON_CALLBACK_t mouse_button_cbs[MAX_MOUSE_BTTUON_CALLBACKS];
  SCROLL_CALLBACK_t scroll_cbs[MAX_SCROLL_CALLBACKS];
  CURSOR_CALLBACK_t cursor_pos_cbs[MAX_CURSOR_POS_CALLBACKS];
  uint32_t key_cb_count, mouse_button_cb_count, scroll_cb_count, cursor_pos_cb_count;
} ui_input_state;

typedef struct {
  ui_shader shader;
  uint32_t vao, vbo, ibo;
  uint32_t vert_count;
  ui_vertex *verts;
  vec4s vert_pos[4];
  ui_texture textures[MAX_TEX_COUNT_BATCH];
  uint32_t tex_index, tex_count,index_count;
} ui_render_state;

typedef struct {
  ui_element_props *data;
  uint32_t count, cap;
} ui_props_stack;

typedef struct {
  bool init;
  uint32_t dsp_w, dsp_h;
  void *window_handle;
  ui_render_state render;
  ui_input_state input;
  ui_theme theme;
  ui_div current_div, prev_div;
  int32_t current_line_height, prev_line_height;
  vec2s pos_ptr, prev_pos_ptr; 
  ui_font *font_stack, *prev_font_stack;
  ui_element_props div_props, prev_props_stack;
  ui_color image_color_stack;
  int64_t element_id_stack;
  ui_props_stack props_stack;
  ui_key_event key_ev;
  ui_mouse_button_event mb_ev;
  ui_cursor_pos_event cp_ev;
  ui_scroll_event scr_ev;
  ui_char_event ch_ev;
  vec2s cull_start, cull_end;
  ui_texture tex_arrow_down, tex_tick;
  bool text_wrap, line_overflow, div_hoverable, input_grabbed;
  uint64_t active_element_id;
  float *scroll_velocity_ptr;
  float *scroll_ptr;
  ui_div selected_div, selected_div_tmp, scrollbar_div, grabbed_div;
  uint32_t drawcalls;
  bool entered_div;
  bool div_velocity_accelerating;
  float last_time, delta_time;
  clipboard_c *clipboard;
  bool renderer_render; 
  ui_drag_state drag_state;
} ui_state;

typedef enum {
  INPUT_INT = 0, 
  INPUT_FLOAT, 
  INPUT_TEXT
} ui_input_field_type;

static ui_state state;

static uint32_t shader_create(GLenum type, const char *src);
static ui_shader shader_prg_create(const char *vert_src, const char *frag_src);
static void shader_set_mat(ui_shader prg, const char *name, mat4 mat); 
static void set_projection_matrix();
static void renderer_init();
static void renderer_flush();
static void renderer_begin();

static ui_text_props text_render_simple(vec2s pos, const char *text, ui_font font, ui_color font_color, bool no_render);
static ui_text_props text_render_simple_wide(vec2s pos, const wchar_t *text, ui_font font, ui_color font_color, bool no_render);
static ui_clickable_item_state button_ex(const char *file, int32_t line, vec2s pos, vec2s size, ui_element_props props, ui_color color, float border_width, bool click_color, bool hover_color, vec2s hitbox_override);
static ui_clickable_item_state button(const char *file, int32_t line, vec2s pos, vec2s size, ui_element_props props, ui_color color, float border_width, bool click_color, bool hover_color);
static ui_clickable_item_state div_container(vec2s pos, vec2s size, ui_element_props props, ui_color color, float border_width, bool click_color, bool hover_color);
static void next_line_on_overflow(vec2s size, float xoffset);
static bool item_should_cull(ui_aabb item);
static void draw_scrollbar_on(ui_div *div);

static void input_field(ui_input_field *input, ui_input_field_type type, const char *file, int32_t line);
ui_font load_font(const char *filepath, uint32_t pixelsize, uint32_t tex_width, uint32_t tex_height, uint32_t line_gap_add);
static ui_font get_current_font(); 
static ui_clickable_item_state button_element_loc(void *text, const char *file, int32_t line, bool wide);
static ui_clickable_item_state button_fixed_element_loc(void *text, float width, float height, const char *file, int32_t line, bool wide);
static ui_clickable_item_state checkbox_element_loc(void *text, bool *val, ui_color tick_color, ui_color tex_color, const char *file, int32_t line, bool wide);
static void dropdown_menu_item_loc(void **items, void *placeholder, uint32_t item_count, float width, float height, int32_t *selected_index, bool *opened, const char *file, int32_t line, bool wide);
static int32_t menu_item_list_item_loc(void **items, uint32_t item_count, int32_t selected_index, ui_menu_item_callback per_cb, bool vertical, const char *file, int32_t line, bool wide);

static int32_t get_max_char_height_font(ui_font font);
static void remove_i_str(char *str, int32_t index);
static void remove_substr_str(char *str, int start_index, int end_index);
static void insert_i_str(char *str, char ch, int32_t index);
static void insert_str_str(char *source, const char *insert, int32_t index);
static void substr_str(const char *str, int start_index, int end_index, char *substring);
static int map_vals(int value, int from_min, int from_max, int to_min, int to_max);

static void glfw_key_callback(GLFWwindow *window, int32_t key, int scancode, int action, int mods);
static void glfw_mouse_button_callback(GLFWwindow *window, int32_t button, int action, int mods); 
static void glfw_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
static void glfw_cursor_callback(GLFWwindow *window, double xpos, double ypos);
static void glfw_char_callback(GLFWwindow *window, uint32_t charcode);

static void update_input();
static void clear_events();

static uint64_t djb2_hash(uint64_t hash, const void *buf, size_t size);

static void props_stack_create(ui_props_stack *stack); 
static void props_stack_resize(ui_props_stack *stack, uint32_t newcap); 
static void props_stack_push(ui_props_stack *stack, ui_element_props props); 
static ui_element_props props_stack_pop(ui_props_stack *stack); 
static ui_element_props props_stack_peak(ui_props_stack *stack); 
static bool props_stack_empty(ui_props_stack *stack);

static ui_element_props get_props_for(ui_element_props props);

uint32_t shader_create(GLenum type, const char *src) {
  uint32_t shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
  int32_t compiled; 
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if(!compiled) {
    error_func("Failed to compile shader", user_defined_data);
    glDeleteShader(shader);
  }
  return shader;
}

ui_shader shader_prg_create(const char *vert_src, const char *frag_src) {
  uint32_t vertex_shader = shader_create(GL_VERTEX_SHADER, vert_src);
  uint32_t fragment_shader = shader_create(GL_FRAGMENT_SHADER, frag_src);
  ui_shader prg;
  prg.id = glCreateProgram();
  glAttachShader(prg.id, vertex_shader);
  glAttachShader(prg.id, fragment_shader);
  glLinkProgram(prg.id);
  int32_t linked;
  glGetProgramiv(prg.id, GL_LINK_STATUS, &linked);
  if(!linked) {
    error_func("Failed to link shader program", user_defined_data);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteProgram(prg.id);
    return prg;
  }
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  return prg;
}

void shader_set_mat(ui_shader prg, const char *name, mat4 mat) {
  glUniformMatrix4fv(glGetUniformLocation(prg.id, name), 1, GL_FALSE, mat[0]);
}

void set_projection_matrix() { 
  float left = 0.0f;
  float right = state.dsp_w;
  float bottom = state.dsp_h;
  float top = 0.0f;
  float near = 0.1f;
  float far = 100.0f;
  mat4 orthoMatrix = GLM_MAT4_IDENTITY_INIT;
  orthoMatrix[0][0] = 2.0f / (right - left);
  orthoMatrix[1][1] = 2.0f / (top - bottom);
  orthoMatrix[2][2] = -1;
  orthoMatrix[3][0] = -(right + left) / (right - left);
  orthoMatrix[3][1] = -(top + bottom) / (top - bottom);
  shader_set_mat(state.render.shader, "u_proj", orthoMatrix);
}

void renderer_init() {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.render.vert_count = 0;
  state.render.verts = (ui_vertex *)malloc(sizeof(ui_vertex) * MAX_RENDER_BATCH * 4);
  glCreateVertexArrays(1, &state.render.vao);
  glBindVertexArray(state.render.vao);
  glCreateBuffers(1, &state.render.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, state.render.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(ui_vertex) * MAX_RENDER_BATCH * 4, NULL, GL_DYNAMIC_DRAW);
  uint32_t *indices = (uint32_t *)malloc(sizeof(uint32_t) * MAX_RENDER_BATCH * 6);
  uint32_t offset = 0;
  for (uint32_t i = 0; i < MAX_RENDER_BATCH * 6; i += 6) {
    indices[i + 0] = offset + 0;
    indices[i + 1] = offset + 1;
    indices[i + 2] = offset + 2;
    indices[i + 3] = offset + 2;
    indices[i + 4] = offset + 3;
    indices[i + 5] = offset + 0;
    offset += 4;
  }
  glCreateBuffers(1, &state.render.ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, state.render.ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_RENDER_BATCH * 6 * sizeof(uint32_t), indices, GL_STATIC_DRAW);
  free(indices); 
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), NULL);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (void*)(intptr_t)offsetof(ui_vertex, border_color));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (void*)(intptr_t)offsetof(ui_vertex, border_width));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (void*)(intptr_t)offsetof(ui_vertex, color));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (void*)(intptr_t *)offsetof(ui_vertex, texcoord));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (void*)(intptr_t *)offsetof(ui_vertex, tex_index));
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (void*)(intptr_t *)offsetof(ui_vertex, scale));
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (void*)(intptr_t *)offsetof(ui_vertex, pos_px));
  glEnableVertexAttribArray(7);
  glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (void*)(intptr_t *)offsetof(ui_vertex, corner_radius));
  glEnableVertexAttribArray(8);
  glVertexAttribPointer(10, 2, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (void*)(intptr_t *)offsetof(ui_vertex, min_coord));
  glEnableVertexAttribArray(10);
  glVertexAttribPointer(11, 2, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (void*)(intptr_t *)offsetof(ui_vertex, max_coord));
  glEnableVertexAttribArray(11);
  const char *vert_src =
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

  const char *frag_src = "#version 450 core\n"
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
  state.render.shader = shader_prg_create(vert_src, frag_src);
  state.render.vert_pos[0] = (vec4s){ -0.5f, -0.5f, 0.0f, 1.0f };
  state.render.vert_pos[1] = (vec4s){ 0.5f, -0.5f, 0.0f, 1.0f };
  state.render.vert_pos[2] = (vec4s){ 0.5f, 0.5f, 0.0f, 1.0f };
  state.render.vert_pos[3] = (vec4s){ -0.5f, 0.5f, 0.0f, 1.0f };
  int32_t tex_slots[MAX_TEX_COUNT_BATCH];
  for(uint32_t i = 0; i < MAX_TEX_COUNT_BATCH; i++) {
    tex_slots[i] = i;
  }
  glUseProgram(state.render.shader.id);
  set_projection_matrix();
  glUniform1iv(glGetUniformLocation(state.render.shader.id, "u_textures"), MAX_TEX_COUNT_BATCH, tex_slots);
}

void renderer_begin() {
  state.render.vert_count = 0;
  state.render.index_count = 0;
  state.render.tex_index = 0;
  state.render.tex_count = 0;
  state.drawcalls = 0;
}

void renderer_flush() {
  if(state.render.vert_count <= 0) {
    return;
  }
  glUseProgram(state.render.shader.id);
  glBindBuffer(GL_ARRAY_BUFFER, state.render.vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ui_vertex) * state.render.vert_count, state.render.verts);
  for(uint32_t i = 0; i < state.render.tex_count; i++) {
    glBindTextureUnit(i, state.render.textures[i].id);
    state.drawcalls++;
  }
  vec2s renderSize = (vec2s){(float)state.dsp_w, (float)state.dsp_h};
  glUniform2fv(glGetUniformLocation(state.render.shader.id, "u_screen_size"), 1, (float*)renderSize.raw);
  glBindVertexArray(state.render.vao);
  glDrawElements(GL_TRIANGLES, state.render.index_count, GL_UNSIGNED_INT, NULL);
}

ui_text_props text_render_simple(vec2s pos, const char *text, ui_font font, ui_color font_color, bool no_render) {
  return ui_text_render(pos, text, font, font_color, -1, (vec2s){-1, -1}, no_render, false, -1, -1);
}

ui_text_props text_render_simple_wide(vec2s pos, const wchar_t *text, ui_font font, ui_color font_color, bool no_render) {
  return ui_text_render_wchar(pos, text, font, font_color, -1, (vec2s){-1, -1}, no_render, false, -1, -1);
}

ui_clickable_item_state button(const char *file, int32_t line, vec2s pos, vec2s size, ui_element_props props, ui_color color, float border_width,  bool click_color, bool hover_color) {
  return button_ex(file, line, pos, size, props, color, border_width, click_color, hover_color, (vec2s){-1, -1}); 
}

ui_clickable_item_state button_ex(const char* file, int32_t line, vec2s pos, vec2s size, ui_element_props props, ui_color color, float border_width, bool click_color, bool hover_color, vec2s hitbox_override) {
  uint64_t id = DJB2_INIT;
  id = djb2_hash(id, file, strlen(file));
  id = djb2_hash(id, &line, sizeof(line));
  if(state.element_id_stack != -1) {
    id = djb2_hash(id, &state.element_id_stack, sizeof(state.element_id_stack));
  }
  if(item_should_cull((ui_aabb){.pos = pos, .size= size})) {
    return ui_clickable_idle;
  }
  ui_color hover_color_rgb = hover_color ? (props.hover_color.a == 0.0f ? ui_color_brightness(color, 1.2) : props.hover_color) : color; 
  ui_color held_color_rgb = click_color ? ui_color_brightness(color, 1.3) : color; 

  bool is_hovered = ui_hovered(pos, (vec2s){hitbox_override.x != -1 ? hitbox_override.x : size.x, hitbox_override.y != -1 ? hitbox_override.y : size.y});
  if(state.active_element_id == 0) {
    if(is_hovered && ui_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT)) {
      state.active_element_id = id;
    }
  } 
  else if(state.active_element_id == id) {
    if(is_hovered && ui_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)) {
      ui_rect_render(pos, size, hover_color_rgb, props.border_color, border_width, props.corner_radius);
      state.active_element_id = 0;
      return ui_clickable_clicked;
    }
  }
  if(is_hovered && ui_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)) {
    state.active_element_id = 0;
  }
  if(is_hovered && ui_mouse_button_is_down(GLFW_MOUSE_BUTTON_LEFT)) {
    ui_rect_render(pos, size, held_color_rgb, props.border_color, border_width, props.corner_radius);
    return ui_clickable_held;
  }
  if(is_hovered && (!ui_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT) && !ui_mouse_button_is_down(GLFW_MOUSE_BUTTON_LEFT))) {
    ui_rect_render(pos, size, hover_color ? hover_color_rgb : color, props.border_color, border_width, props.corner_radius);
    return ui_clickable_held;
  }
  ui_rect_render(pos, size, color, props.border_color, border_width, props.corner_radius);
  return ui_clickable_idle;
}

ui_clickable_item_state div_container(vec2s pos, vec2s size, ui_element_props props, ui_color color, float border_width, bool click_color, bool hover_color) {
  if(item_should_cull((ui_aabb){.pos = pos, .size = size})) {
    return ui_clickable_idle;
  }
  ui_color hover_color_rgb = hover_color ? (props.hover_color.a == 0.0f ? ui_color_brightness(color, 1.5) : props.hover_color) : color; 
  ui_color held_color_rgb = click_color ? ui_color_brightness(color, 1.8) : color; 

  bool is_hovered = ui_hovered(pos, size);
  if(is_hovered && ui_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)) {
    ui_rect_render(pos, size, hover_color_rgb, props.border_color, border_width, props.corner_radius);
    return ui_clickable_clicked;
  }
  if(is_hovered && ui_mouse_button_is_down(GLFW_MOUSE_BUTTON_LEFT)) {
    ui_rect_render(pos, size, held_color_rgb, props.border_color, border_width, props.corner_radius);
    return ui_clickable_held;
  }
  if(is_hovered && (!ui_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT) && !ui_mouse_button_is_down(GLFW_MOUSE_BUTTON_LEFT))) {
    ui_rect_render(pos, size, hover_color ? hover_color_rgb : color, props.border_color, border_width, props.corner_radius);
    return ui_clickable_hovered;
  }
  ui_rect_render(pos, size, color, props.border_color, border_width, props.corner_radius);
  return ui_clickable_idle;
}

void next_line_on_overflow(vec2s size, float xoffset) {
  if(!state.line_overflow) return;
  if(state.pos_ptr.x - state.current_div.aabb.pos.x + size.x > state.current_div.aabb.size.x) {
    state.pos_ptr.y += state.current_line_height;
    state.pos_ptr.x = state.current_div.aabb.pos.x + xoffset;
    state.current_line_height = 0;
  }
  if(size.y > state.current_line_height) {
    state.current_line_height = size.y;
  }
}

bool item_should_cull(ui_aabb item) {
  bool intersect = true;
  ui_aabb window =  (ui_aabb){.pos = (vec2s){0, 0}, .size = (vec2s){state.dsp_w, state.dsp_h}};
  if(item.size.x == -1 || item.size.y == -1) {
    item.size.x = state.dsp_w;
    item.size.y = get_current_font().font_size;
  }  
  if (item.pos.x + item.size.x <= window.pos.x || item.pos.x >= window.pos.x + window.size.x)
    intersect = false;

  if (item.pos.y + item.size.y <= window.pos.y || item.pos.y >= window.pos.y + window.size.y)
    intersect = false;

  return !intersect && state.current_div.id == state.scrollbar_div.id;

  return false;
}
