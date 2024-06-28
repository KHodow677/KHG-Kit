#pragma once

#include "khg_2d/texture.h"
#include "khg_math/vec4.h"
#include "khg_ui/data.h"

typedef enum {
  none = 0,
  widget_button,
  widget_toggle,
  widget_toggle_button,
  widget_text,
  widget_text_input,
  widget_begin_menu,
  widget_end_menu,
  widget_texture,
  widget_button_with_texture,
  widget_slider_float_w,
  widget_color_picker_w,
  widget_new_column_w,
  widget_slider_int_w,
  widget_custom,
  widget_options_toggle,
} widget_type;

typedef struct {
  char *text_2;
  char *text_3;
  int type;
  bool just_created;
  bool enabled;
  bool display_text;
  bool only_one_enabled;
  bool used_this_frame;
  input_data last_frame_data;
  vec4 color;
  vec4 color_2;
  vec4 color_3;
  texture texture;
  texture texture_over;
  vec4 texture_coords;
  bool return_from_update;
  bool custom_widget_used;
  void *pointer;
  void *pointer2;
  bool clicked;
  bool hovered;
  float min;
  float max;
  int min_int;
  int max_int;
  vec4 return_transform;
  persistent_data pd;
  size_t text_size;
} widget;

