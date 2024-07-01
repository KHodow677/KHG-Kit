#pragma once

#include "khg_2d/font.h"
#include "khg_2d/renderer_2d.h"
#include "khg_2d/texture.h"
#include "khg_math/vec4.h"
#include "khg_ui/data.h"
#include "khg_ui/widget.h"
#include "khg_utils/hashtable.h"

extern int x_padd;
extern int y_padd;
extern int width;
extern int height;

extern float timer;
extern int current_id;
extern bool id_was_set;

extern float press_down_size;
extern float shadow_size;
extern float outline_size;
extern float text_fit;

extern float non_minimize_text_size;
extern float minimize_ratio;

extern float button_fit;

extern float in_size_y;
extern float in_size_x;
extern float main_in_size_x;
extern float main_in_size_y;
extern float padding_columns;

typedef struct {
  char *first;
  widget second;
} widget_pair;

typedef struct {
  vec4 first;
  float second;
} column_pair;

typedef struct {
  aligned_settings a_settings;
  vector(widget_pair) widgets_vector;
  hash_table widgets;
  hash_table all_menu_stacks;
  char *id_str;
  char *current_text_box;
} renderer_ui;

void render_frame(renderer_ui *rui, renderer_2d *r2d, font *f, vec2 mouse_pos, bool mouse_click, bool mouse_held, bool mouse_released, bool escape_released, char *typed_input, float delta_time);
bool button_ui(renderer_ui *rui, char *name, const vec4 color, const texture t);
void texture_ui(renderer_ui *rui, int id, texture t, vec4 color, vec4 texture_coods);
bool button_with_texture_ui(renderer_ui *rui, int id, texture t, vec4 color, vec4 texture_coords);
bool toggle_ui(renderer_ui *rui, char *name, const vec4 color, bool *toggle, const texture t, const texture over_texture);
bool toggle_button_ui(renderer_ui *rui, char *name, const vec4 text_color, bool *toggle, const texture t, const vec4 button_color);
bool custom_widget_ui(renderer_ui *rui, int id, vec4 *transform, bool *hovered, bool *clicked);
void text_ui(renderer_ui *rui, char *name, const vec4 color);

