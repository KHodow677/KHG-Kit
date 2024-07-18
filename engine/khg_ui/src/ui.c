#include "khg_ui/ui.h"
#include "khg_ui/events.h"
#include "glad/glad.h"
#include <time.h>
#include "stb_image/stb_image.h"
#include "stb_image_resize/stb_image_resize2.h"
#include "stb_truetype/stb_truetype.h"
#include <GLFW/glfw3.h>

#include "libclipboard/libclipboard.h"

#include <locale.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <wchar.h>
#include <wctype.h>

#ifdef _WIN32
#define HOMEDIR "USERPROFILE"
#else
#define HOMEDIR (char*)"HOME"
#endif

#include <GLFW/glfw3.h>

#define LF_STACK_INIT_CAP 4

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
  vec2 start_cursor_pos;
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
  vec4 vert_pos[4];
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
  vec2 pos_ptr, prev_pos_ptr; 
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
  vec2 cull_start, cull_end;
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
