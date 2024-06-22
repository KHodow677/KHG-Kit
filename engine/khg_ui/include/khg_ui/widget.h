#pragma once

#include "khg_2d/texture.h"
#include "khg_math/vec4.h"
#include "khg_ui/data.h"

typedef struct {
  char **text_2;
  char **text_3;
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

typedef struct {
  char **first;
  widget second;
} widget_pair;

