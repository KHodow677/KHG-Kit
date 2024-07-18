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

extern ui_state state;

extern uint32_t shader_create(GLenum type, const char *src);
extern ui_shader shader_prg_create(const char *vert_src, const char *frag_src);
extern void shader_set_mat(ui_shader prg, const char *name, mat4 mat); 
extern void set_projection_matrix();
extern void renderer_init();
extern void renderer_flush();
extern void renderer_begin();

extern ui_text_props text_render_simple(vec2s pos, const char *text, ui_font font, ui_color font_color, bool no_render);
extern ui_text_props text_render_simple_wide(vec2s pos, const wchar_t *text, ui_font font, ui_color font_color, bool no_render);
extern ui_clickable_item_state button_ex(const char *file, int32_t line, vec2s pos, vec2s size, ui_element_props props, ui_color color, float border_width, bool click_color, bool hover_color, vec2s hitbox_override);
extern ui_clickable_item_state button(const char *file, int32_t line, vec2s pos, vec2s size, ui_element_props props, ui_color color, float border_width, bool click_color, bool hover_color);
extern ui_clickable_item_state div_container(vec2s pos, vec2s size, ui_element_props props, ui_color color, float border_width, bool click_color, bool hover_color);
extern void next_line_on_overflow(vec2s size, float xoffset);
extern bool item_should_cull(ui_aabb item);
extern void draw_scrollbar_on(ui_div *div);

extern void input_field(ui_input_field *input, ui_input_field_type type, const char *file, int32_t line);
ui_font load_font(const char *filepath, uint32_t pixelsize, uint32_t tex_width, uint32_t tex_height, uint32_t line_gap_add);
extern ui_font get_current_font(); 
extern ui_clickable_item_state button_element_loc(void *text, const char *file, int32_t line, bool wide);
extern ui_clickable_item_state button_fixed_element_loc(void *text, float width, float height, const char *file, int32_t line, bool wide);
extern ui_clickable_item_state checkbox_element_loc(void *text, bool *val, ui_color tick_color, ui_color tex_color, const char *file, int32_t line, bool wide);
extern void dropdown_menu_item_loc(void **items, void *placeholder, uint32_t item_count, float width, float height, int32_t *selected_index, bool *opened, const char *file, int32_t line, bool wide);
extern int32_t menu_item_list_item_loc(void **items, uint32_t item_count, int32_t selected_index, ui_menu_item_callback per_cb, bool vertical, const char *file, int32_t line, bool wide);

extern int32_t get_max_char_height_font(ui_font font);
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

extern void update_input();
extern void clear_events();

extern uint64_t djb2_hash(uint64_t hash, const void *buf, size_t size);

extern void props_stack_create(ui_props_stack *stack); 
extern void props_stack_resize(ui_props_stack *stack, uint32_t newcap); 
extern void props_stack_push(ui_props_stack *stack, ui_element_props props); 
extern ui_element_props props_stack_pop(ui_props_stack *stack); 
extern ui_element_props props_stack_peak(ui_props_stack *stack); 
extern bool props_stack_empty(ui_props_stack *stack);

extern ui_element_props get_props_for(ui_element_props props);
