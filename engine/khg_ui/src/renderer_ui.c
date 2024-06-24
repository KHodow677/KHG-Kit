#include "khg_ui/renderer_ui.h"
#include "khg_2d/camera.h"
#include "khg_ui/data.h"
#include "khg_ui/widget.h"
#include "khg_utils/hashtable.h"
#include "khg_utils/vector.h"
#include <string.h>

int x_padd = 0;
int y_padd = 0;
int width = 0;
int height = 0;

float timer = 0.0f;
int current_id = 0;
bool id_was_set = 0;

float press_down_size = 0.04f;
float shadow_size = 0.1f;
float outline_size = 0.02f;
float text_fit = 1.2f;

float non_minimize_text_size = 0.9f;
float minimize_ratio = 0.8f;

float button_fit  = 0.6f;

float in_size_y = 0.8f;
float in_size_x = 0.8f;
float main_in_size_x = 0.9f;
float main_in_size_y = 0.9f;
float padding_columns = 0.9f;

vec4 compute_pos(renderer_ui *rui, renderer_2d *r2d, int elements_height, float *advance_size_y) {
  float size_with_padding_y = 0;
  if (rui->a_settings.widget_size.y != 0) {
    size_with_padding_y = rui->a_settings.widget_size.y;
  }
  else {
    size_with_padding_y = ((float) r2d->window_h / elements_height);
  }
  float size_y = size_with_padding_y * in_size_y;
  float padd_size_y = size_with_padding_y * (1 - in_size_y) / 2.0f;
  float size_with_padd_x = (float)r2d->window_w;
  float size_x = size_with_padd_x * in_size_x;
  float padd_size_x = size_with_padd_x * (1 - in_size_x) / 2.0f;
  vec4 computed_pos = { 
    padd_size_x + (float)r2d->window_w * (1 - main_in_size_x) * 0.5f,
    padd_size_y + (float)r2d->window_h * (1 - main_in_size_y) * 0.5f,
    size_x * main_in_size_x,
    size_y * main_in_size_y,
  };
  *advance_size_y = (padd_size_y * 2 + size_y) * main_in_size_y;
  return computed_pos;
}

void render_frame(renderer_ui *rui, renderer_2d *r2d, font *f, vec2 mouse_pos, bool mouse_click, bool mouse_held, bool mouse_released, bool escape_released, const char *typed_input, float delta_time) {
  if (!id_was_set) {
    return;
  }
  vector(char *) iter_menu_stack = ht_lookup(&rui->all_menu_stacks, &current_id);
  if (iter_menu_stack == NULL) {
    vector(char *) new_menu_stack;
    ht_insert(&rui->all_menu_stacks, &current_id, &new_menu_stack);
  }
  vector(char *) current_menu_stack = ht_lookup(&rui->all_menu_stacks, &current_id);
  id_was_set = 0;
  current_id = 0;
  if (escape_released && !vector_empty(current_menu_stack)) {
    vector_pop_back(current_menu_stack);
  }
  timer += delta_time * 2;
  if (timer >= 2.0f) {
    timer -= 2;
  }
  for (ht_node *i = rui->widgets.nodes[0]; i; i = i->next) {
    widget *w = i->value;
    if (w->type == widget_custom) {
      w->custom_widget_used = false;
    }
  }
  vector(widget_pair) widgets_copy;
  vector_reserve(widgets_copy, vector_size(rui->widgets_vector));
  vector(char *) current_menu_stack_copy;
  vector_copy(current_menu_stack, current_menu_stack_copy);
  vector(char *) menu_stack;
  char *next_menu = "";
  bool should_ignore = false;
  if (!vector_empty(current_menu_stack_copy)) {
    next_menu = (char *) vector_front(current_menu_stack_copy);
    vector_erase(current_menu_stack_copy, 0);
    should_ignore = true;
  }
  int next_stack_size_to_look = 0;
  int next_stack_size_to_look_min = 0;
  for (int i = 0; i < vector_size(rui->widgets_vector); i++) {
    widget_pair w = rui->widgets_vector[i];
    if (w.second.type == widget_begin_menu) {
      vector_push_back(menu_stack, w.first);
      if (w.first == next_menu) {
        if (!vector_empty(current_menu_stack_copy)) {
          next_menu = (char *) vector_front(current_menu_stack_copy);
          vector_erase(current_menu_stack_copy, 0);
          should_ignore = true;
        }
        else {
          should_ignore = false;
          next_stack_size_to_look_min = vector_size(menu_stack);
        }
      }
      else if (w.first != next_menu && !should_ignore) {
        should_ignore = true;
        next_stack_size_to_look = vector_size(menu_stack) - 1;
        vector_push_back(widgets_copy, w);
      }
      continue;
    }
    if (w.second.type == widget_end_menu) {
      w.first = strcat("##$", (char *)vector_back(menu_stack));
      vector_pop_back(menu_stack);
      if (next_stack_size_to_look == vector_size(menu_stack)) {
        should_ignore = false;
      }
      if (vector_size(menu_stack) < next_stack_size_to_look_min) {
        should_ignore = true;
      }
      continue;
    }
    if (should_ignore) {
      continue;
    }
    vector_push_back(widgets_copy, w);
  }
  vector(column_pair) columns;
  int widgets_count_until_new_column_w = 0;
  for (int i = 0; i < vector_size(widgets_copy); i++) {
    if (widgets_copy[i].second.type == widget_new_column_w) {
      float padd = 0;
      vec4 res = compute_pos(rui, r2d, widgets_count_until_new_column_w, &padd);
      column_pair c_pair = { res, padd };
      vector_push_back(columns, c_pair);
      widgets_count_until_new_column_w = 0;
    }
    else {
      widgets_count_until_new_column_w++;
    }
  }
  if (widgets_count_until_new_column_w != 0) {
    float padd = 0;
    vec4 res = compute_pos(rui, r2d, widgets_count_until_new_column_w, &padd);
    column_pair c_pair = { res, padd };
    vector_push_back(columns, c_pair);
    widgets_count_until_new_column_w = 0;
  }
  int current_column = 0;
  for (int j = 0; j < vector_size(columns); j++) {
    column_pair i = columns[j];
    i.first.z /= vector_size(columns);
  }
  float column_advance_size = columns[0].first.z;
  float begin_y = columns[0].first.y;
  for (int i = 0; i < vector_size(columns); i++) {
    columns[i].first.x += column_advance_size * i;
    columns[i].first.x += columns[i].first.z * ((1.0f - padding_columns) / 2.0f);
    columns[i].first.z *= padding_columns;
  }
  camera c = r2d->current_camera;
  set_default(&r2d->current_camera);
  input_data input = { 0 };
  input.mouse_pos = mouse_pos;
  input.mouse_click = mouse_click;
  input.mouse_held = mouse_held;
  input.mouse_released = mouse_released;
  input.escape_released = escape_released;

}
