#pragma once
#include "khg_2d/renderer_2d.h"
#include "khg_2d/texture.h"
#include "khg_2d/utils.h"
#include "khg_math/vec2.h"
#include "khg_math/vec4.h"
#include "khg_utils/map.h"
#include "khg_utils/string.h"
#include "khg_utils/vector.h"

typedef struct frame frame;
typedef struct box box;
typedef struct renderer_ui renderer_ui;

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

struct frame {
  int last_w;
  int last_h;
  int last_x;
  int last_y;
  bool loaded;
};

struct box {
  vec4 dimensions;
  float aspect;
  char x_center_state;
  char y_center_state;
  char dimension_state;
};

struct renderer_ui {
  struct internal {
    struct input_data {
      vec2 mouse_pos;
      bool mouse_click;
      bool mouse_held;
      bool mouse_released;
      bool escape_released;
    };
    struct widget {
      string *text2;
      string *text3;
      int type;
      bool just_created;
      bool enabled;
      bool display_text;
      bool only_one_enabled;
      bool used_this_frame;
      input_data last_frame_data;
      vec4 colors;
      vec4 colors2;
      vec4 colors3;
      texture t;
      texture t_over;
      vec4 texture_coords;
      bool return_from_update;
      bool custom_widget_used;
      void *pointerl;
      void *pointer2;
      bool clicked;
      bool hovered;
      float min;
      float max;
      int minInt;
      int maxInt;
      vec4 returnTransform;
      struct persistent_data {
        bool slider_being_dragged;
        bool slider_being_dragged2;
        bool slider_being_dragged3;
      } pd;
      size_t text_size;
    };
    struct align_settings {
      vec2 widget_size;
    } a_settings;
    vector *widgets_vector;
    map *widgets;
    map *all_menu_stacks;
    string *id_str;
    string *current_text_box;
  } intern;
};

void render_frame(renderer_ui *rui, renderer_2d *r2d, font *f, vec2 mouse_pos, bool mouse_click, bool mouse_held, bool mouse_released, bool escape_released, const string *typed_input, float delta_time);
bool button_ui(renderer_ui *rui, string *name, const vec4 color, const texture t);
void texture_ui(renderer_ui *rui, int id, texture t, vec4 color, vec4 texture_coords);
bool button_with_texture_ui(renderer_ui *rui, int id, texture t, vec4 color, vec4 texture_coords);
bool toggle_ui(renderer_ui *rui, string *name, const vec4 text_color, bool *toggle, const texture t, const vec4 button_color);
bool toggle_button_ui(renderer_ui *rui, string *name, const vec4 text_colors, bool *toggle, const texture t, const vec4 button_colors);
bool custom_widget_ui(renderer_ui *rui, int id, vec4 *transform, bool *hovered, bool *clicked);
void text_ui(renderer_ui *rui, string *name, vec4 color);
void input_text_ui(renderer_ui *rui, string *name, char *text, size_t text_with_null_char, vec4 color, const texture t, bool only_on_enabled, bool display_text, bool enabled);
void slider_float_ui(renderer_ui *rui, string *name, float *value, float min, float max, vec4 text_color, texture slider_texture, vec4 slider_color, texture ball_texture, vec4 ball_color);
void slider_int_ui(renderer_ui *rui, string *name, int *value, int min, int max, vec4 text_color, texture slider_texture, vec4 slider_color, texture ball_texture, vec4 ball_color);
void color_picker_ui(renderer_ui *rui, string *name, float *color3_component, texture slider_texture, texture ball_texture, vec4 color, vec4 color2);
void toggle_options_ui(renderer_ui *rui, string *name, string *options_separated_by_bars, size_t *current_index, bool show_text, vec4 text_color, vec4 *options_color, texture t, vec4 texture_color, string *tool_tip);
void new_column_ui(renderer_ui *rui, int id);
void push_id_ui(renderer_ui *rui, int id);
void pop_id_ui(renderer_ui *rui);
void begin_menu_ui(renderer_ui *rui, string *name, const vec4 colors, const texture t);
void end_menu_ui(renderer_ui *rui);
void begin_ui(renderer_ui *rui, int id);
void end_ui(renderer_ui *rui);
void set_align_mode_fixed_size_widgets(renderer_ui *rui, vec2 size);

frame *create_frame_1_ui(frame *other) {
  frame *new_frame = (frame *)malloc(sizeof(frame));
  new_frame->last_w = other->last_w;
  new_frame->last_h = other->last_h;
  new_frame->last_x = other->last_x;
  new_frame->last_y = other->last_y;
  new_frame->loaded = other->loaded;
  free(other);
}

frame *create_frame_3_ui(vec4 size);
void cleanup_frame(frame *f);

box *x_distance_pixels(box *b, int dist);
box *y_distance_pixels(box *b, int dist);
box *x_center(box *b, int dist);
box *y_center(box *b, int dist);
box *x_left(box *b, int dist);
box *x_left_perc(box *b, float perc);
box *y_top(box *b, int dist);
box *y_top_perc(box *b, float perc);
box *x_right(box *b, int dist);
box *y_bottom(box *b, int dist);
box *x_dimension_pixels(box *b, int dim);
box *y_dimension_pixels(box *b, int dim);
box *x_dimension_pixels(box *b, float p);
box *y_dimension_pixels(box *b, float p);
box *x_aspect_ratio(box *b, float r);
box *y_aspect_ratio(box *b, float r);
bool box_in_button_ui(const vec2 *p, const vec4 *box);
