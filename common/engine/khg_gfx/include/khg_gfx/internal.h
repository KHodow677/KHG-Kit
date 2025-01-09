#pragma once

#include "cglm/types-struct.h"
#include "GLFW/glfw3.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/events.h"
#include "khg_gfx/texture.h"
#include "libclipboard/libclipboard.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <limits.h>

#define GFX_UI_STACK_INIT_CAP 4

#define GFX_MAX_KEYS GLFW_KEY_LAST
#define GFX_MAX_MOUSE_BUTTONS GLFW_MOUSE_BUTTON_LAST
#define GFX_KEY_CALLBACK GLFWkeyfun
#define GFX_MOUSE_BUTTON_CALLBACK GLFWmousebuttonfun
#define GFX_SCROLL_CALLBACK GLFWscrollfun
#define GFX_CURSOR_CALLBACK GLFWcursorposfun

#define GFX_MAX_RENDER_BATCH 10000
#define GFX_MAX_TEX_COUNT_BATCH 32
#define GFX_MAX_KEY_CALLBACKS 4
#define GFX_MAX_MOUSE_BTTUON_CALLBACKS 4
#define GFX_MAX_SCROLL_CALLBACKS 4
#define GFX_MAX_CURSOR_POS_CALLBACKS 4

#define GFX_DJB2_INIT 5381

#define GFX_MAX(a, b) a > b ? a : b
#define GFX_MIN(a, b) a < b ? a : b

typedef struct {
  unsigned int id;
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
  bool keys[GFX_MAX_KEYS];
  bool keys_changed[GFX_MAX_KEYS];
} gfx_keyboard;

typedef struct {
  bool buttons_current[GFX_MAX_MOUSE_BUTTONS];
  bool buttons_last[GFX_MAX_MOUSE_BUTTONS];

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
  GFX_KEY_CALLBACK key_cbs[GFX_MAX_KEY_CALLBACKS];
  GFX_MOUSE_BUTTON_CALLBACK mouse_button_cbs[GFX_MAX_MOUSE_BTTUON_CALLBACKS];
  GFX_SCROLL_CALLBACK scroll_cbs[GFX_MAX_SCROLL_CALLBACKS];
  GFX_CURSOR_CALLBACK cursor_pos_cbs[GFX_MAX_CURSOR_POS_CALLBACKS];
  unsigned int key_cb_count, mouse_button_cb_count, scroll_cb_count, cursor_pos_cb_count;
} gfx_input_state;

typedef struct {
  gfx_shader shader;
  unsigned int vao, vbo, ibo;
  unsigned int vert_count;
  gfx_vertex *verts;
  vec4s vert_pos[4];
  gfx_texture textures[GFX_MAX_TEX_COUNT_BATCH];
  unsigned int tex_index, tex_count,index_count;
} gfx_render_state;

typedef struct {
  gfx_element_props *data;
  unsigned int count, cap;
} gfx_props_stack;

typedef struct {
  bool init;
  unsigned int dsp_w, dsp_h;
  void *window_handle;
  gfx_render_state render;
  gfx_input_state input;
  gfx_theme theme;
  gfx_div current_div, prev_div;
  int current_line_height, prev_line_height;
  vec2s pos_ptr, prev_pos_ptr; 
  gfx_font *font_stack, *prev_font_stack;
  gfx_element_props div_props, prev_props_stack;
  gfx_color image_color_stack;
  long long element_id_stack;
  gfx_props_stack props_stack;
  gfx_key_event key_ev;
  gfx_mouse_button_event mb_ev;
  gfx_cursor_pos_event cp_ev;
  gfx_scroll_event scr_ev;
  gfx_char_event ch_ev;
  vec2s cull_start, cull_end;
  gfx_texture tex_arrow_down, tex_tick;
  bool text_wrap, line_overflow, div_hoverable, input_grabbed;
  unsigned long long active_element_id;
  float *scroll_velocity_ptr;
  float *scroll_ptr;
  gfx_div selected_div, selected_div_tmp, scrollbar_div, grabbed_div;
  unsigned int drawcalls;
  bool entered_div;
  bool div_velocity_accelerating;
  float last_time, delta_time;
  clipboard_c *clipboard;
  bool renderer_render; 
  gfx_drag_state drag_state;
} gfx_state;

typedef enum {
  GFX_INPUT_INT, 
  GFX_INPUT_FLOAT, 
  GFX_INPUT_TEXT
} gfx_input_field_type;

extern gfx_state GFX_STATE;

unsigned int gfx_internal_shader_create(unsigned int type, const char *src);
gfx_shader gfx_internal_shader_prg_create(const char *vert_src, const char *frag_src);
void gfx_internal_shader_set_mat(gfx_shader prg, const char *name, mat4 mat); 
void gfx_internal_set_projection_matrix(void);
void gfx_internal_renderer_init(void);
void gfx_internal_renderer_set_shader(gfx_shader shader);
void gfx_internal_renderer_flush(void);
void gfx_internal_renderer_begin(void);

gfx_text_props gfx_internal_text_render_simple(vec2s pos, const char *text, gfx_font font, gfx_color font_color, bool no_render);
gfx_clickable_item_state gfx_internal_button_ex(const char *file, int line, vec2s pos, vec2s size, gfx_element_props props, gfx_color color, float border_width, bool click_color, bool hover_color, vec2s hitbox_override);
gfx_clickable_item_state gfx_internal_button(const char *file, int line, vec2s pos, vec2s size, gfx_element_props props, gfx_color color, float border_width, bool click_color, bool hover_color);
gfx_clickable_item_state gfx_internal_div_container(vec2s pos, vec2s size, gfx_element_props props, gfx_color color, float border_width, bool click_color, bool hover_color);
void gfx_internal_next_line_on_overflow(vec2s size, float xoffset);
bool gfx_internal_item_should_cull(gfx_aabb item, bool cullable);
void gfx_internal_draw_scrollbar_on(gfx_div *div);

void gfx_internal_input_field(gfx_input_field *input, gfx_input_field_type type, const char *file, int line);
gfx_font gfx_internal_get_current_font(void); 
gfx_clickable_item_state gfx_internal_button_element_loc(void *text, const char *file, int line);
gfx_clickable_item_state gfx_internal_button_fixed_element_loc(void *text, float width, float height, const char *file, int line);
gfx_clickable_item_state gfx_internal_checkbox_element_loc(void *text, bool *val, gfx_color tick_color, gfx_color tex_color, const char *file, int line);
void gfx_internal_dropdown_menu_item_loc(void **items, void *placeholder, unsigned int item_count, float width, float height, int *selected_index, bool *opened, const char *file, int line);
int gfx_internal_menu_item_list_item_loc(void **items, unsigned int item_count, int selected_index, gfx_menu_item_callback per_cb, bool vertical, const char *file, int line);

void gfx_internal_remove_i_str(char *str, unsigned int index);
void gfx_internal_remove_substr_str(char *str, unsigned int start_index, unsigned int end_index);
void gfx_internal_insert_i_str(char *str, char ch, unsigned int index);
void gfx_internal_insert_str_str(char *source, const char *insert, unsigned int index);
void gfx_internal_substr_str(const char *str, unsigned int start_index, unsigned int end_index, char *substring);
int gfx_internal_map_vals(int value, int from_min, int from_max, int to_min, int to_max);

void gfx_internal_glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void gfx_internal_glfw_mouse_button_callback(GLFWwindow *window, int button, int action, int mods); 
void gfx_internal_glfw_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void gfx_internal_glfw_cursor_callback(GLFWwindow *window, double xpos, double ypos);
void gfx_internal_glfw_char_callback(GLFWwindow *window, unsigned int charcode);
void gfx_internal_glfw_window_size_callback(GLFWwindow *window, int width, int height);

extern void gfx_internal_update_input(void);
extern void gfx_internal_clear_events(void);

extern unsigned long long gfx_internal_djb2_hash(unsigned long long hash, const void *buf, unsigned int size);

extern void gfx_internal_props_stack_create(gfx_props_stack *stack); 
extern void gfx_internal_props_stack_resize(gfx_props_stack *stack, unsigned int newcap); 
extern void gfx_internal_props_stack_push(gfx_props_stack *stack, gfx_element_props props); 
extern gfx_element_props gfx_internal_props_stack_pop(gfx_props_stack *stack); 
extern gfx_element_props gfx_internal_props_stack_peak(gfx_props_stack *stack); 
extern bool gfx_internal_props_stack_empty(gfx_props_stack *stack);
extern void gfx_internal_props_stack_clear(gfx_props_stack *stack);

extern gfx_element_props gfx_internal_get_props_for(gfx_element_props props);

