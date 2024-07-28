#pragma once

#include "khg_gfx/elements.h"
#include "khg_gfx/events.h"
#include "khg_gfx/texture.h"
#include "cglm/types-struct.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "libclipboard/libclipboard.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <limits.h>
#include <wchar.h>

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

#define MAX(a, b) a > b ? a : b
#define MIN(a, b) a < b ? a : b

typedef struct {
  uint32_t id;
} gfx_shader;

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
} gfx_vertex;

typedef struct {
  bool keys[MAX_KEYS];
  bool keys_changed[MAX_KEYS];
} gfx_keyboard;

typedef struct {
  bool buttons_current[MAX_MOUSE_BUTTONS];
  bool buttons_last[MAX_MOUSE_BUTTONS];

  double xpos, ypos, xpos_last, ypos_last, xpos_delta, ypos_delta;
  bool first_mouse_press; 
  double xscroll_delta, yscroll_delta;
} gfx_mouse;

typedef struct {
  bool is_dragging;
  vec2s start_cursor_pos;
  float start_scroll;
} gfx_drag_state;

typedef struct {
  gfx_keyboard keyboard;
  gfx_mouse mouse;
  KEY_CALLBACK_t key_cbs[MAX_KEY_CALLBACKS];
  MOUSE_BUTTON_CALLBACK_t mouse_button_cbs[MAX_MOUSE_BTTUON_CALLBACKS];
  SCROLL_CALLBACK_t scroll_cbs[MAX_SCROLL_CALLBACKS];
  CURSOR_CALLBACK_t cursor_pos_cbs[MAX_CURSOR_POS_CALLBACKS];
  uint32_t key_cb_count, mouse_button_cb_count, scroll_cb_count, cursor_pos_cb_count;
} gfx_input_state;

typedef struct {
  gfx_shader shader;
  uint32_t vao, vbo, ibo;
  uint32_t vert_count;
  gfx_vertex *verts;
  vec4s vert_pos[4];
  gfx_texture textures[MAX_TEX_COUNT_BATCH];
  uint32_t tex_index, tex_count,index_count;
} gfx_render_state;

typedef struct {
  gfx_element_props *data;
  uint32_t count, cap;
} gfx_props_stack;

typedef struct {
  bool init;
  uint32_t dsp_w, dsp_h;
  void *window_handle;
  gfx_render_state render;
  gfx_input_state input;
  gfx_theme theme;
  gfx_div current_div, prev_div;
  int32_t current_line_height, prev_line_height;
  vec2s pos_ptr, prev_pos_ptr; 
  gfx_font *font_stack, *prev_font_stack;
  gfx_element_props div_props, prev_props_stack;
  gfx_color image_color_stack;
  int64_t element_id_stack;
  gfx_props_stack props_stack;
  gfx_key_event key_ev;
  gfx_mouse_button_event mb_ev;
  gfx_cursor_pos_event cp_ev;
  gfx_scroll_event scr_ev;
  gfx_char_event ch_ev;
  vec2s cull_start, cull_end;
  gfx_texture tex_arrow_down, tex_tick;
  bool text_wrap, line_overflow, div_hoverable, input_grabbed;
  uint64_t active_element_id;
  float *scroll_velocity_ptr;
  float *scroll_ptr;
  gfx_div selected_div, selected_div_tmp, scrollbar_div, grabbed_div;
  uint32_t drawcalls;
  bool entered_div;
  bool div_velocity_accelerating;
  float last_time, delta_time;
  clipboard_c *clipboard;
  bool renderer_render; 
  gfx_drag_state drag_state;
} gfx_state;

typedef enum {
  INPUT_INT = 0, 
  INPUT_FLOAT, 
  INPUT_TEXT
} gfx_input_field_type;

extern gfx_state state;

extern uint32_t shader_create(GLenum type, const char *src);
extern gfx_shader shader_prg_create(const char *vert_src, const char *frag_src);
extern void shader_set_mat(gfx_shader prg, const char *name, mat4 mat); 
extern void set_projection_matrix(void);
extern void renderer_init(void);
extern void renderer_flush(void);
extern void renderer_begin(void);

extern gfx_text_props text_render_simple(vec2s pos, const char *text, gfx_font font, gfx_color font_color, bool no_render);
extern gfx_text_props text_render_simple_wide(vec2s pos, const wchar_t *text, gfx_font font, gfx_color font_color, bool no_render);
extern gfx_clickable_item_state button_ex(const char *file, int32_t line, vec2s pos, vec2s size, gfx_element_props props, gfx_color color, float border_width, bool click_color, bool hover_color, vec2s hitbox_override);
extern gfx_clickable_item_state button(const char *file, int32_t line, vec2s pos, vec2s size, gfx_element_props props, gfx_color color, float border_width, bool click_color, bool hover_color);
extern gfx_clickable_item_state div_container(vec2s pos, vec2s size, gfx_element_props props, gfx_color color, float border_width, bool click_color, bool hover_color);
extern void next_line_on_overflow(vec2s size, float xoffset);
extern bool item_should_cull(gfx_aabb item);
extern void draw_scrollbar_on(gfx_div *div);

extern void input_field(gfx_input_field *input, gfx_input_field_type type, const char *file, int32_t line);
gfx_font load_font(const char *filepath, uint32_t pixelsize, uint32_t tex_width, uint32_t tex_height, uint32_t line_gap_add);
extern gfx_font get_current_font(void); 
extern gfx_clickable_item_state button_element_loc(void *text, const char *file, int32_t line, bool wide);
extern gfx_clickable_item_state button_fixed_element_loc(void *text, float width, float height, const char *file, int32_t line, bool wide);
extern gfx_clickable_item_state checkbox_element_loc(void *text, bool *val, gfx_color tick_color, gfx_color tex_color, const char *file, int32_t line, bool wide);
extern void dropdown_menu_item_loc(void **items, void *placeholder, uint32_t item_count, float width, float height, int32_t *selected_index, bool *opened, const char *file, int32_t line, bool wide);
extern int32_t menu_item_list_item_loc(void **items, uint32_t item_count, int32_t selected_index, gfx_menu_item_callback per_cb, bool vertical, const char *file, int32_t line, bool wide);

extern int32_t get_max_char_height_font(gfx_font font);
extern void remove_i_str(char *str, int32_t index);
extern void remove_substr_str(char *str, int start_index, int end_index);
extern void insert_i_str(char *str, char ch, int32_t index);
extern void insert_str_str(char *source, const char *insert, int32_t index);
extern void substr_str(const char *str, int start_index, int end_index, char *substring);
extern int map_vals(int value, int from_min, int from_max, int to_min, int to_max);

extern void glfw_key_callback(GLFWwindow *window, int32_t key, int scancode, int action, int mods);
extern void glfw_mouse_button_callback(GLFWwindow *window, int32_t button, int action, int mods); 
extern void glfw_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
extern void glfw_cursor_callback(GLFWwindow *window, double xpos, double ypos);
extern void glfw_char_callback(GLFWwindow *window, uint32_t charcode);
extern void glfw_window_size_callback(GLFWwindow *window, int width, int height);

extern void update_input(void);
extern void clear_events(void);

extern uint64_t djb2_hash(uint64_t hash, const void *buf, size_t size);

extern void props_stack_create(gfx_props_stack *stack); 
extern void props_stack_resize(gfx_props_stack *stack, uint32_t newcap); 
extern void props_stack_push(gfx_props_stack *stack, gfx_element_props props); 
extern gfx_element_props props_stack_pop(gfx_props_stack *stack); 
extern gfx_element_props props_stack_peak(gfx_props_stack *stack); 
extern bool props_stack_empty(gfx_props_stack *stack);

extern gfx_element_props get_props_for(gfx_element_props props);

