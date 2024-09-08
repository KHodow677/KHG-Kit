#pragma once

#include "khg_gfx/texture.h"
#include "cglm/types-struct.h"
#include <wchar.h>

typedef struct {
  uint8_t r, g, b, a;
} gfx_color;

typedef struct {
  int32_t cursor_index, width, height, start_height;
  char *buf;
  uint32_t buf_size;
  char *placeholder;
  bool selected;
  uint32_t max_chars;
  int32_t selection_start, selection_end, mouse_selection_start, mouse_selection_end;
  int32_t selection_dir, mouse_dir;
  bool init;
  void (*char_callback)(char);
  void (*insert_override_callback)(void *);
  void (*key_callback)(void *);
  bool retain_height;
} gfx_input_field;

typedef struct {
  void *val;
  int32_t handle_pos;
  bool init;
  float min, max;
  bool held, selected;
  float width, height;
  uint32_t handle_size;
  gfx_color handle_color;
} gfx_slider;

typedef enum {
  gfx_clickable_released = -1,
  gfx_clickable_idle = 0,
  gfx_clickable_hovered = 1,
  gfx_clickable_clicked = 2,
  gfx_clickable_held = 3
} gfx_clickable_item_state;

typedef struct {
  gfx_color color, hover_color;
  gfx_color text_color, hover_text_color;
  gfx_color border_color;
  float padding;
  float margin_left;
  float margin_right;
  float margin_top;
  float margin_bottom;
  float border_width;
  float corner_radius;
} gfx_element_props;

typedef struct {
  vec2s pos, size;
} gfx_aabb;

typedef struct {
  int64_t id;
  gfx_aabb aabb;
  gfx_clickable_item_state interact_state;
  bool scrollable;
  vec2s total_area;
} gfx_div;

typedef struct {
  gfx_element_props button_props, div_props, text_props, image_props, input_field_props, checkbox_props, slider_props, scrollbar_props;
  gfx_font font;
  bool div_smooth_scroll;
  float div_scroll_acceleration, div_scroll_max_velocity;
  float div_scroll_amount_px;
  float div_scroll_velocity_deceleration;
  float scrollbar_width;
} gfx_theme;

typedef void (*gfx_menu_item_callback)(uint32_t *);

gfx_theme gfx_default_theme(void);
gfx_theme gfx_get_theme(void);
void gfx_set_theme(gfx_theme theme);
void gfx_resize_display(uint32_t display_width, uint32_t display_height);

#define gfx_div_begin(pos, size, scrollable) {\
  static float scroll = 0.0f; \
  static float scroll_velocity = 0.0f; \
  gfx_div_begin_loc(pos, size, scrollable, &scroll, &scroll_velocity, __FILE__, __LINE__);\
}

#define gfx_div_begin_ex(pos, size, scrollable, scroll_ptr, scroll_velocity_ptr) gfx_div_begin_loc(pos, size, scrollable, scroll_ptr, scroll_velocity_ptr, __FILE__, __LINE__);
gfx_div *gfx_div_begin_loc(vec2s pos, vec2s size, bool scrollable, float *scroll, float *scroll_velocity, const char* file, int32_t line);
void gfx_div_end(void);

gfx_clickable_item_state gfx_item_loc(vec2s size,  const char *file, int32_t line);
#define gfx_item(size) gfx_item_loc(size, __FILE__, __LINE__)

#define gfx_button(text) gfx_button_loc(text, __FILE__, __LINE__)
gfx_clickable_item_state gfx_button_loc(const char *text, const char *file, int32_t line);
#define gfx_button_wide(text) gfx_button_wide_loc(text, __FILE__, __LINE__)
gfx_clickable_item_state gfx_button_wide_loc(const wchar_t *text, const char *file, int32_t line);

#define gfx_image_button(img) gfx_image_button_loc(img, __FILE__, __LINE__)
gfx_clickable_item_state gfx_image_button_loc(gfx_texture img, const char *file, int32_t line);
#define gfx_image_button_fixed(img, width, height) gfx_image_button_fixed_loc(img, width, height, __FILE__, __LINE__)
gfx_clickable_item_state gfx_image_button_fixed_loc(gfx_texture img, float width, float height, const char *file, int32_t line);

#define gfx_button_fixed(text, width, height) gfx_button_fixed_loc(text, width, height, __FILE__, __LINE__)
gfx_clickable_item_state gfx_button_fixed_loc(const char *text, float width, float height, const char *file, int32_t line);
#define gfx_button_fixed_wide(text, width, height) gfx_button_fixed_loc_wide(text, width, height, __FILE__, __LINE__)
gfx_clickable_item_state gfx_button_fixed_wide_loc(const wchar_t *text, float width, float height, const char *file, int32_t line);

#define gfx_slider_int(slider) gfx_slider_int_loc(slider, __FILE__, __LINE__)
gfx_clickable_item_state gfx_slider_int_loc(gfx_slider *slider, const char *file, int32_t line);

#define gfx_slider_int_inl_ex(slider_val, slider_min, slider_max, slider_width, slider_height, slider_handle_size, state) {\
  static gfx_slider slider = {\
    .val = slider_val,\
    .handle_pos = 0,\
    .min = slider_min,\
    .max = slider_max,\
    .width = slider_width,\
    .height = slider_height,\
    .handle_size = slider_handle_size\
  };\
  state = gfx_slider_int(&slider);\
}\

#define gfx_slider_int_inl(slider_val, slider_min, slider_max, state) gfx_slider_int_inl_ex(slider_val, slider_min, slider_max, gfx_get_current_div().aabb.size.x / 2.0f, 5, 0, state)

#define gfx_progress_bar_val(width, height, min, max, val) gfx_progress_bar_val_loc(width, height, min, max, val, __FILE__, __LINE__)
gfx_clickable_item_state gfx_progress_bar_val_loc(float width, float height, int32_t min, int32_t max, int32_t val, const char *file, int32_t line);
#define gfx_progress_bar_int(val, min, max, width, height) gfx_progress_bar_int_loc(val, min, max, width, height, __FILE__, __LINE__)
gfx_clickable_item_state gfx_progress_bar_int_loc(float val, float min, float max, float width, float height, const char *file, int32_t line);
#define gfx_progress_stripe_int(slider) gfx_progresss_stripe_int_loc(slider , __FILE__, __LINE__)
gfx_clickable_item_state gfx_progress_stripe_int_loc(gfx_slider *slider, const char *file, int32_t line);

#define gfx_checkbox(text, val, tick_color, tex_color) gfx_checkbox_loc(text, val, tick_color, tex_color, __FILE__, __LINE__)
gfx_clickable_item_state gfx_checkbox_loc(const char *text, bool *val, gfx_color tick_color, gfx_color tex_color, const char *file, int32_t line);
#define gfx_checkbox_wide(text, val, tick_color, tex_color) gfx_checkbox_wide_loc(text, val, tick_color, tex_color, __FILE__, __LINE__)
gfx_clickable_item_state _gfx_checkbox_wide_loc(const wchar_t *text, bool *val, gfx_color tick_color, gfx_color tex_color, const char *file, int32_t line);

#define gfx_menu_item_list(items, item_count, selected_index, per_cb, vertical) gfx_menu_item_list_loc(__FILE__, __LINE__, items, item_count, selected_index, per_cb, vertical)
int32_t gfx_menu_item_list_loc(const char **items, uint32_t item_count, int32_t selected_index, gfx_menu_item_callback per_cb, bool vertical, const char *file, int32_t line);
#define gfx_menu_item_list_wide(items, item_count, selected_index, per_cb, vertical) gfx_menu_item_list_loc_wide(__FILE__, __LINE__, items, item_count, selected_index, per_cb, vertical)
int32_t gfx_menu_item_list_loc_wide(const wchar_t **items, uint32_t item_count, int32_t selected_index, gfx_menu_item_callback per_cb, bool vertical, const char *file, int32_t line);

#define gfx_dropdown_menu(items, placeholder, item_count, width, height, selected_index, opened) gfx_dropdown_menu_loc(items, placeholder, item_count, width, height, selected_index, opened, __FILE__, __LINE__)
void gfx_dropdown_menu_loc(const char **items, const char *placeholder, uint32_t item_count, float width, float height, int32_t *selected_index, bool *opened, const char *file, int32_t line);
#define gfx_dropdown_menu_wide(items, placeholder, item_count, width, height, selected_index, opened) gfx_dropdown_menu_loc_wide(items, placeholder, item_count, width, height, selected_index, opened, __FILE__, __LINE__)
void gfx_dropdown_menu_loc_wide(const wchar_t **items, const wchar_t *placeholder, uint32_t item_count, float width, float height, int32_t *selected_index, bool *opened, const char *file, int32_t line);

#define gfx_input_text_inl_ex(buffer, buffer_size, input_width, placeholder_str) {\
  static gfx_input_field input = {\
    .cursor_index = 0,\
    .width = input_width,\
    .buf = buffer,\
    .buf_size = buffer_size,\
    .placeholder = (char *)placeholder_str,\
    .selected = false\
  };\
  gfx_input_text_loc(&input, __FILE__, __LINE__);\
}\

#define gfx_input_text_inl(buffer, buffer_size) gfx_input_text_inl_ex(buffer, buffer_size, (int32_t)(gfx_get_current_div().aabb.size.x / 2), "")
#define gfx_input_text(input) gfx_input_text_loc(input, __FILE__, __LINE__)
void gfx_input_text_loc(gfx_input_field *input, const char *file, int32_t line);
#define gfx_input_int(input) gfx_input_int_loc(input, __FILE__, __LINE__)
void gfx_input_int_loc(gfx_input_field *input, const char *file, int32_t line);
#define gfx_input_float(input) gfx_input_float_loc(input, __FILE__, __LINE__)
void gfx_input_float_loc(gfx_input_field *input, const char *file, int32_t line);
void gfx_input_insert_char_idx(gfx_input_field *input, char c, uint32_t idx);
void gfx_input_insert_str_idx(gfx_input_field *input, const char *insert, uint32_t len, uint32_t idx);

void gfx_input_field_unselect_all(gfx_input_field *input);
bool gfx_input_grabbed(void);

void gfx_div_grab(gfx_div div);
void gfx_div_ungrab(void);
bool gfx_div_grabbed(void);
gfx_div gfx_get_grabbed_div(void);

#define gfx_begin() gfx_begin_loc(__FILE__, __LINE__)
void gfx_begin_loc(const char* file, int32_t line);
void gfx_end(void);
void gfx_next_line(void);

vec2s gfx_text_dimension(const char *str);
vec2s gfx_text_dimension_ex(const char *str, float wrap_point);
vec2s gfx_text_dimension_wide(const wchar_t *str);
vec2s gfx_text_dimension_wide_ex(const wchar_t *str, float wrap_point);
vec2s gfx_button_dimension(const char *text);

float gfx_get_text_end(const char *str, float start_x);
void gfx_text(const char *text);
void gfx_text_wide(const wchar_t *text);
void gfx_set_text_wrap(bool wrap);

gfx_div gfx_get_current_div(void);
gfx_div gfx_get_selected_div(void);
gfx_div *gfx_get_current_div_ptr(void);
gfx_div *gfx_get_selected_div_ptr(void);

void gfx_set_ptr_x(float x);
void gfx_set_ptr_y(float y);
void gfx_set_ptr_x_absolute(float x);
void gfx_set_ptr_y_absolute(float y);
float gfx_get_ptr_x(void);
float gfx_get_ptr_y(void);
uint32_t gfx_get_display_width(void);
uint32_t gfx_get_display_height(void);

void gfx_push_font(gfx_font *font);
void gfx_pop_font(void);

gfx_text_props gfx_text_render(vec2s pos, const char *str, gfx_font font, gfx_color color, int32_t wrap_point, vec2s stop_point, bool no_render, bool render_solid, int32_t start_index, int32_t end_index);

void gfx_rect_render(vec2s pos, vec2s size, gfx_color color, gfx_color border_color, float border_width, float corner_radius, float rotation_angle);
void gfx_image_render(vec2s pos, gfx_color color, gfx_texture tex, gfx_color border_color, float border_width, float corner_radius, float rotation_angle, float cam_x, float cam_y, float cam_zoom);

bool gfx_point_intersects_aabb(vec2s p, gfx_aabb aabb);
bool gfx_aabb_intersects_aabb(gfx_aabb a, gfx_aabb b);

void gfx_push_style_props(gfx_element_props props);
void gfx_pop_style_props(void);

bool gfx_hovered(vec2s pos, vec2s size);
bool gfx_area_hovered(vec2s pos, vec2s size);

void gfx_set_image_color(gfx_color color);
void gfx_unset_image_color(void);

void gfx_set_current_div_scroll(float scroll); 
float gfx_get_current_div_scroll(void); 
void gfx_set_current_div_scroll_velocity(float scroll_velocity);
float gfx_get_current_div_scroll_velocity(void);

void gfx_set_line_height(uint32_t line_height);
uint32_t gfx_get_line_height(void);
void gfx_set_line_should_overflow(bool overflow);

void gfx_set_div_hoverable(bool hoverable);

void gfx_push_element_id(int64_t id);
void gfx_pop_element_id(void);

gfx_color gfx_color_brightness(gfx_color color, float brightness);
gfx_color gfx_color_alpha(gfx_color color, uint8_t a);
vec4s gfx_color_to_zto(gfx_color color);
gfx_color gfx_color_from_hex(uint32_t hex);
gfx_color gfx_color_from_zto(vec4s zto);

void gfx_image(gfx_texture tex);
void gfx_image_no_block(float x, float y, gfx_texture tex, float cam_x, float cam_y, float cam_zoom);
void gfx_rect(float width, float height, gfx_color color, float corner_radius, float angle);
void gfx_rect_no_block(float x, float y, float width, float height, gfx_color color, float corner_radius, float angle);
void gfx_seperator(void);

void gfx_set_clipboard_text(const char *text);
char *gfx_get_clipboard_text(void);

void gfx_set_no_render(bool no_render);

