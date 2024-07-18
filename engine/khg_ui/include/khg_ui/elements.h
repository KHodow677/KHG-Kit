#pragma once

#include "khg_ui/texture.h"
#include "cglm/cglm.h"
#include "cglm/struct.h"
#include <wchar.h>

typedef struct {
  uint8_t r, g, b, a;
} ui_color;

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
} ui_input_field;

typedef struct {
  void *val;
  int32_t handle_pos;
  bool init;
  float min, max;
  float width, height;
  uint32_t handle_size;
  ui_color handle_color;
} ui_slider;

typedef enum {
  ui_clickable_released = -1,
  ui_clickable_idle = 0,
  ui_clickable_hovered = 1,
  ui_clickable_clicked = 2,
  ui_clickable_held = 3
} ui_clickable_item_state;

typedef struct {
  ui_color color, hover_color;
  ui_color text_color, hover_text_color;
  ui_color border_color;
  float padding;
  float margin_left;
  float margin_right;
  float margin_top;
  float margin_bottom;
  float border_width;
  float corner_radius;
} ui_element_props;

typedef struct {
  vec2s pos, size;
} ui_aabb;

typedef struct {
  int64_t id;
  ui_aabb aabb;
  ui_clickable_item_state interact_state;
  bool scrollable;
  vec2s total_area;
} ui_div;

typedef struct {
  ui_element_props button_props, div_props, text_props, image_props, input_field_props, checkbox_props, slider_props, scrollbar_props;
  ui_font font;
  bool div_smooth_scroll;
  float div_scroll_acceleration, div_scroll_max_velocity;
  float div_scroll_amount_px;
  float div_scroll_velocity_deceleration;
  float scrollbar_width;
} ui_theme;

typedef void (*ui_menu_item_callback)(uint32_t *);

ui_theme ui_default_theme();
ui_theme ui_get_theme();
void ui_set_theme(ui_theme theme);
void ui_resize_display(uint32_t display_width, uint32_t display_height);

#define ui_div_begin(pos, size, scrollable) {\
  static float scroll = 0.0f; \
  static float scroll_velocity = 0.0f; \
  ui_div_begin_loc(pos, size, scrollable, &scroll, &scroll_velocity, __FILE__, __LINE__);\
}

#define ui_div_begin_ex(pos, size, scrollable, scroll_ptr, scroll_velocity_ptr) ui_div_begin_loc(pos, size, scrollable, scroll_ptr, scroll_velocity_ptr, __FILE__, __LINE__);
ui_div *ui_div_begin_loc(vec2s pos, vec2s size, bool scrollable, float *scroll, float *scroll_velocity, const char* file, int32_t line);
void ui_div_end();

ui_clickable_item_state ui_item_loc(vec2s size,  const char *file, int32_t line);
#define ui_item(size) ui_item_loc(size, __FILE__, __LINE__)

#define ui_button(text) ui_button_loc(text, __FILE__, __LINE__)
ui_clickable_item_state ui_button_loc(const char *text, const char *file, int32_t line);
#define ui_button_wide(text) ui_button_wide_loc(text, __FILE__, __LINE__)
ui_clickable_item_state ui_button_wide_loc(const wchar_t *text, const char *file, int32_t line);

#define ui_image_button(img) ui_image_button_loc(img, __FILE__, __LINE__)
ui_clickable_item_state ui_image_button_loc(ui_texture img, const char *file, int32_t line);
#define ui_image_button_fixed(img, width, height) ui_image_button_fixed_loc(img, width, height, __FILE__, __LINE__)
ui_clickable_item_state ui_image_button_fixed_loc(ui_texture img, float width, float height, const char *file, int32_t line);

#define ui_button_fixed(text, width, height) ui_button_fixed_loc(text, width, height, __FILE__, __LINE__)
ui_clickable_item_state ui_button_fixed_loc(const char *text, float width, float height, const char *file, int32_t line);
#define ui_button_fixed_wide(text, width, height) ui_button_fixed_loc_wide(text, width, height, __FILE__, __LINE__)
ui_clickable_item_state ui_button_fixed_wide_loc(const wchar_t *text, float width, float height, const char *file, int32_t line);

#define ui_slider_int(slider) ui_slider_int_loc(slider, __FILE__, __LINE__)
ui_clickable_item_state ui_slider_int_loc(ui_slider *slider, const char *file, int32_t line);

#define ui_slider_int_inl_ex(slider_val, slider_min, slider_max, slider_width, slider_height, slider_handle_size, state) {\
  static ui_slider slider = {\
    .val = slider_val,\
    .handle_pos = 0,\
    .min = slider_min,\
    .max = slider_max,\
    .width = slider_width,\
    .height = slider_height,\
    .handle_size = slider_handle_size\
  };\
  state = ui_slider_int(&slider);\
}\

#define ui_slider_int_inl(slider_val, slider_min, slider_max, state) ui_slider_int_inl_ex(slider_val, slider_min, slider_max, ui_get_current_div().aabb.size.x / 2.0f, 5, 0, state)

#define ui_progress_bar_val(width, height, min, max, val) ui_progress_bar_val_loc(width, height, min, max, val, __FILE__, __LINE__)
ui_clickable_item_state ui_progress_bar_val_loc(float width, float height, int32_t min, int32_t max, int32_t val, const char *file, int32_t line);
#define ui_progress_bar_int(val, min, max, width, height) ui_progress_bar_int_loc(val, min, max, width, height, __FILE__, __LINE__)
ui_clickable_item_state ui_progress_bar_int_loc(float val, float min, float max, float width, float height, const char *file, int32_t line);
#define ui_progress_stripe_int(slider) ui_progresss_stripe_int_loc(slider , __FILE__, __LINE__)
ui_clickable_item_state ui_progress_stripe_int_loc(ui_slider *slider, const char *file, int32_t line);

#define ui_checkbox(text, val, tick_color, tex_color) ui_checkbox_loc(text, val, tick_color, tex_color, __FILE__, __LINE__)
ui_clickable_item_state ui_checkbox_loc(const char *text, bool *val, ui_color tick_color, ui_color tex_color, const char *file, int32_t line);
#define ui_checkbox_wide(text, val, tick_color, tex_color) ui_checkbox_wide_loc(text, val, tick_color, tex_color, __FILE__, __LINE__)
ui_clickable_item_state _ui_checkbox_wide_loc(const wchar_t *text, bool *val, ui_color tick_color, ui_color tex_color, const char *file, int32_t line);

#define ui_menu_item_list(items, item_count, selected_index, per_cb, vertical) ui_menu_item_list_loc(__FILE__, __LINE__, items, item_count, selected_index, per_cb, vertical)
int32_t ui_menu_item_list_loc(const char **items, uint32_t item_count, int32_t selected_index, ui_menu_item_callback per_cb, bool vertical, const char *file, int32_t line);
#define ui_menu_item_list_wide(items, item_count, selected_index, per_cb, vertical) ui_menu_item_list_loc_wide(__FILE__, __LINE__, items, item_count, selected_index, per_cb, vertical)
int32_t ui_menu_item_list_loc_wide(const wchar_t **items, uint32_t item_count, int32_t selected_index, ui_menu_item_callback per_cb, bool vertical, const char *file, int32_t line);

#define ui_dropdown_menu(items, placeholder, item_count, width, height, selected_index, opened) ui_dropdown_menu_loc(items, placeholder, item_count, width, height, selected_index, opened, __FILE__, __LINE__)
void ui_dropdown_menu_loc(const char **items, const char *placeholder, uint32_t item_count, float width, float height, int32_t *selected_index, bool *opened, const char *file, int32_t line);
#define ui_dropdown_menu_wide(items, placeholder, item_count, width, height, selected_index, opened) ui_dropdown_menu_loc_wide(items, placeholder, item_count, width, height, selected_index, opened, __FILE__, __LINE__)
void ui_dropdown_menu_loc_wide(const wchar_t **items, const wchar_t *placeholder, uint32_t item_count, float width, float height, int32_t *selected_index, bool *opened, const char *file, int32_t line);

#define ui_input_text_inl_ex(buffer, buffer_size, input_width, placeholder_str) {\
  static ui_input_field input = {\
    .cursor_index = 0,\
    .width = input_width,\
    .buf = buffer,\
    .buf_size = buffer_size,\
    .placeholder = (char *)placeholder_str,\
    .selected = false\
  };\
  _ui_input_text_loc(&input, __FILE__, __LINE__);\
}\

#define ui_input_text_inl(buffer, buffer_size) ui_input_text_inl_ex(buffer, buffer_size, (int32_t)(ui_get_current_div().aabb.size.x / 2), "")
#define ui_input_text(input) ui_input_text_loc(input, __FILE__, __LINE__)
void ui_input_text_loc(ui_input_field *input, const char *file, int32_t line);
#define ui_input_int(input) ui_input_int_loc(input, __FILE__, __LINE__)
void ui_input_int_loc(ui_input_field *input, const char *file, int32_t line);
#define ui_input_float(input) ui_input_float_loc(input, __FILE__, __LINE__)
void ui_input_float_loc(ui_input_field *input, const char *file, int32_t line);
void ui_input_insert_char_idx(ui_input_field *input, char c, uint32_t idx);
void ui_input_insert_str_idx(ui_input_field *input, const char *insert, uint32_t len, uint32_t idx);

void ui_input_field_unselect_all(ui_input_field *input);
bool ui_input_grabbed();

void ui_div_grab(ui_div div);
void ui_div_ungrab();
bool ui_div_grabbed();
ui_div ui_get_grabbed_div();

#define ui_begin() ui_begin_loc(__FILE__, __LINE__)
void ui_begin_loc(const char* file, int32_t line);
void ui_end();
void ui_next_line();

vec2s ui_text_dimension(const char *str);
vec2s ui_text_dimension_ex(const char *str, float wrap_point);
vec2s ui_text_dimension_wide(const wchar_t *str);
vec2s ui_text_dimension_wide_ex(const wchar_t *str, float wrap_point);
vec2s ui_button_dimension(const char *text);

float ui_get_text_end(const char *str, float start_x);
void ui_text(const char *text);
void ui_text_wide(const wchar_t *text);
void ui_set_text_wrap(bool wrap);

ui_div ui_get_current_div();
ui_div ui_get_selected_div();
ui_div *ui_get_current_div_ptr();
ui_div *ui_get_selected_div_ptr();

void ui_set_ptr_x(float x);
void ui_set_ptr_y(float y);
void ui_set_ptr_x_absolute(float x);
void ui_set_ptr_y_absolute(float y);
float ui_get_ptr_x();
float ui_get_ptr_y();
uint32_t ui_get_display_width();
uint32_t ui_get_display_height();

void ui_push_font(ui_font *font);
void ui_pop_font();

ui_text_props ui_text_render(vec2s pos, const char *str, ui_font font, ui_color color, int32_t wrap_point, vec2s stop_point, bool no_render, bool render_solid, int32_t start_index, int32_t end_index);
ui_text_props ui_text_render_wchar(vec2s pos, const wchar_t *str, ui_font font, ui_color color, int32_t wrap_point, vec2s stop_point, bool no_render, bool render_solid, int32_t start_index, int32_t end_index);

void ui_rect_render(vec2s pos, vec2s size, ui_color color, ui_color border_color, float border_width, float corner_radius);
void ui_image_render(vec2s pos, ui_color color, ui_texture tex, ui_color border_color, float border_width, float corner_radius);

bool ui_point_intersects_aabb(vec2s p, ui_aabb aabb);
bool ui_aabb_intersects_aabb(ui_aabb a, ui_aabb b);

void ui_push_style_props(ui_element_props props);
void ui_pop_style_props();

bool ui_hovered(vec2s pos, vec2s size);
bool ui_area_hovered(vec2s pos, vec2s size);

void ui_set_image_color(ui_color color);
void ui_unset_image_color();

void ui_set_current_div_scroll(float scroll); 
float ui_get_current_div_scroll(); 
void ui_set_current_div_scroll_velocity(float scroll_velocity);
float ui_get_current_div_scroll_velocity();

void ui_set_line_height(uint32_t line_height);
uint32_t ui_get_line_height();
void ui_set_line_should_overflow(bool overflow);

void ui_set_div_hoverable(bool hoverable);

void ui_push_element_id(int64_t id);
void ui_pop_element_id();

ui_color ui_color_brightness(ui_color color, float brightness);
ui_color ui_color_alpha(ui_color color, uint8_t a);
vec4s ui_color_to_zto(ui_color color);
ui_color ui_color_from_hex(uint32_t hex);
ui_color ui_color_from_zto(vec4s zto);

void ui_image(ui_texture tex);
void ui_rect(float width, float height, ui_color color, float corner_radius);

void ui_seperator();
void ui_set_clipboard_text(const char* text);
char* ui_get_clipboard_text();
void ui_set_no_render(bool no_render);
