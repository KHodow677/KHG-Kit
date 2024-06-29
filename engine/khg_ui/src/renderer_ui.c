#include "khg_ui/renderer_ui.h"
#include "khg_2d/camera.h"
#include "khg_2d/renderer_2d.h"
#include "khg_2d/texture.h"
#include "khg_2d/utils.h"
#include "khg_math/math.h"
#include "khg_math/minmax.h"
#include "khg_math/vec2.h"
#include "khg_math/vec3.h"
#include "khg_math/vec4.h"
#include "khg_ui/data.h"
#include "khg_ui/widget.h"
#include "khg_utils/error_func.h"
#include "khg_utils/hashtable.h"
#include "khg_utils/vector.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

static bool LOOK_SLIDER = 1;

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

bool aabb(vec4 transform, vec2 point) {
  if (point.x >= transform.x && point.y >= transform.y && point.x <= transform.x + transform.z && point.y <= transform.y + transform.w) {
    return true;
  }
  else {
    return false;
  }
}

char *get_string(char *s) {
  char *f = strstr(s, "##");
  if (f != NULL) {
    size_t len = f - s;
    char *result = (char *)malloc(len + 1);
    if (result != NULL) {
      strncpy(result, s, len);
      result[len] = '\0';
    }
    return result;
  }
  else {
    char *result = (char *)malloc(strlen(s) + 1);
    if (result != NULL) {
      strcpy(result, s);
    }
    return result;
  }
}

vec3 hsv_color(vec3 rgb) {
  vec3 hsv;
  float min, max, delta;
  min = fmin(rgb.x, rgb.z);
  max = fmax(rgb.x, fmaxf(rgb.y, rgb.z));
  hsv.z = max;
  delta = max - min;
  if (delta < 0.00001) {
    hsv.y = 0;
    hsv.x = 0;
    return hsv;
  }
  if (max >0.0) {
    hsv.y = delta / max;
  }
  else if (rgb.y >= max) {
    hsv.x = 2.0 + (rgb.z - rgb.x) / delta;
  }
  else {
    hsv.x = 4.0 + (rgb.x - rgb.y) / delta;
  }
  hsv.x *= 60.0;
  if (hsv.x < 0.0) {
    hsv.x += 360.0;
  }
  return hsv;
}

vec3 rgb_color(vec3 hsv) {
  vec3 rgb;
  float hh, p, q, t, ff;
  long i;
  if (hsv.y <= 0.0f) {
    rgb.x = hsv.z;
    rgb.y = hsv.z;
    rgb.z = hsv.z;
    return rgb;
  }
  hh = hsv.x;
  if (hh >= 360.0f) {
    hh = 0.0f;
  }
  hh /= 60.0f;
  i = (long)hh;
  ff = hh - i;
  p = hsv.z * (1.0 - hsv.y);
  q = hsv.z * (1.0 - (hsv.y * ff));
  t = hsv.z * (1.0 - (hsv.y * (1.0 - ff)));

  switch (i) {
    case 0:
      rgb.x = hsv.z;
      rgb.y = t;
      rgb.z = p;
      break;
    case 1:
      rgb.x = q;
      rgb.y = hsv.z;
      rgb.z = p;
      break;
    case 2:
      rgb.x = p;
      rgb.y = hsv.z;
      rgb.z = t;
      break;
    case 3:
      rgb.x = p;
      rgb.y = q;
      rgb.z = hsv.z;
      break;
    case 4:
      rgb.x = t;
      rgb.y = p;
      rgb.z = hsv.z;
      break;
    default:
      rgb.x = hsv.z;
      rgb.y = p;
      rgb.z = q;
      break;
  }
  return rgb;
}

vec4 step_color_up(vec4 color, float perc) {
  vec3 hsv = hsv_color((vec3){color.x, color.y, color.z});
  if (hsv.z >= 1.0f) {
    hsv.y = max(0.0f, hsv.y - perc * 2);
  }
  else {
    hsv.z = min(1.0f, hsv.y + perc);
  }
  hsv = rgb_color(hsv);
  color.x = hsv.x;
  color.y = hsv.y;
  color.z = hsv.z;
  return color;
}

vec4 step_color_down(vec4 color, float perc) {
  color.x *= perc;
  color.y *= perc;
  color.z *= perc;
  return color;
}

void split_transforms(vec4 *down, vec4 *new_transform, vec4 transform) {
  *down = transform;
  *new_transform = transform;
  float border = shadow_size * min(transform.w, transform.z);
  down->w = border;
  new_transform->w -= border;
  down->y += new_transform->w;
}

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

vec4 compute_texture_new_position(vec4 transform, texture t) {
  vec2 t_size = get_texture_size(&t);
  if (t_size.y == 0) {
    vec4 vec = { 0 };
    return vec;
  }
  if (transform.w == 0) {
    vec4 vec = { 0 };
    return vec;
  }
  float aspect_ratio = t_size.x / (float)t_size.y;
  float box_aspect_ratio = transform.z / transform.w;
  if (aspect_ratio < box_aspect_ratio) {
    vec2 new_size = { 0 };
    new_size.y = transform.w;
    new_size.x = aspect_ratio * new_size.y;
    vec4 new_pos = { transform.x, transform.y, new_size.x, new_size.y };
    new_pos.x += (transform.z - new_size.x) / 2.0f;
    return new_pos;
  }
  else if (aspect_ratio > box_aspect_ratio) {
    vec2 new_size = { 0 };
    new_size.x = transform.z;
    new_size.y = new_size.x / aspect_ratio;
    vec4 new_pos = { transform.x, transform.y, new_size.x, new_size.y };
    new_pos.y += (transform.w - new_size.y) / 2.0f;
    return new_pos;
  }
  else {
    return transform;
  }
}

float determine_text_size(renderer_2d *r2d, char *str, font *f, vec4 transform, bool minimize) {
  char *new_str = strdup(str);
  float size = text_fit;
  vec2 s = get_text_size(r2d, new_str, *f, size, 4, 3);
  float ratio_x = transform.z / s.x;
  float ratio_y = transform.w / s.y;
  if (!(ratio_x > 1 && ratio_y > 1)) {
    if (ratio_x < ratio_y) {
      size *= ratio_x;
    }
    else {
      size *= ratio_y;
    }
  }
  if (minimize) {
    size *= minimize_ratio;
  }
  else {
    size *= non_minimize_text_size;
  }
  return size;
}

vec4 determine_text_pos(renderer_2d *r2d, char *str, font *f, vec4 transform, bool no_texture, bool minimize) {
  char *new_str = strdup(str);
  float new_s = determine_text_size(r2d, new_str, f, transform, true);
  vec2 s = get_text_size(r2d, new_str, *f, new_s, 4, 3);
  vec2 pos = { transform.x, transform.y };
  pos.x += transform.z / 2.f;
  pos.y += transform.w / 2.f;
  s = vec2_multiply_num_on_vec2(1.0f / 2.0f, &s);
  pos.x -= s.x;
  pos.y -= s.y;
  return (vec4){ pos.x, pos.y, s.x, s.y };
}

void render_fancy_box(renderer_2d *r2d, vec4 transform, vec4 color, texture t, bool hovered, bool clicked) {
  if (color.w <= 0.01f) {
    return;
  }
  float color_dim = 0.0f;
  if (hovered) {
    color_dim = 0.2f;
    if (clicked) {
      color_dim = -0.8f;
    }
  }
  vec4 new_color = { 0 };
  if (color_dim > 0) {
    new_color = step_color_up(color, color_dim);
  }
  else if (color_dim < 0) {
    new_color = step_color_down(color, -color_dim);
  }
  else {
    new_color = color;
  }
  vec4 light_color = step_color_up(new_color, 0.02f);
  vec4 dark_color = step_color_down(new_color, 0.5f);
  vec4 darker_color = step_color_down(new_color, 0.25f);
  vec4 outline_color = step_color_up(new_color, 0.3f);
  vec4 color_vector[4] = { dark_color, dark_color, light_color, light_color };
  if (t.id == 0) {
    float calculated_outline = outline_size * min(transform.w, transform.z);
    if (hovered) {
      vec4 outline_colors[4] = { outline_color, outline_color, outline_color, outline_color };
      render_rectangle(r2d, transform, outline_colors, (vec2){ 0.0f, 0.0f }, 0.0f);
    }
    else {
      vec4 dark_colors[4] = { dark_color, dark_color, dark_color, dark_color };
      render_rectangle(r2d, transform, dark_colors, (vec2){ 0.0f, 0.0f }, 0.0f);
    }
    transform.x += calculated_outline;
    transform.y += calculated_outline;
    transform.z -= calculated_outline * 2;
    transform.w -= calculated_outline * 2;
    vec4 middle = { 0 };
    vec4 down = { 0 };
    split_transforms(&down, &middle, transform);
    render_rectangle(r2d, middle, color_vector, (vec2){ 0.0f, 0.0f }, 0.0f);
    render_rectangle(r2d, down, color_vector, (vec2){ 0.0f, 0.0f }, 0.0f);
  }
}

void render_texture_ui (renderer_2d *r2d, vec4 transform, texture t, vec4 c, vec4 texture_coordinates) {
  vec4 new_pos = compute_texture_new_position(transform, t);
  vec4 colors[4] = { c, c, c, c };
  render_rectangle_texture(r2d, new_pos, t, colors, (vec2) { 0.0f, 0.0f }, 0.0f, default_texture_coords);
}

void render_text_ui(renderer_2d *r2d, char *str, font *f, vec4 transform, vec4 color, bool no_texture, bool minimize, bool align_left) {
  char *new_str = get_string(str);
  float new_s = determine_text_size(r2d, new_str, f, transform, minimize);
  vec2 pos = { transform.x, transform.y };
  if (!align_left) {
    pos.x += transform.z / 2.0f;
    pos.y += transform.w / 3.0f;
    render_text(r2d, pos, new_str, *f, color, new_s, 4, 3, true, (vec4){ 0.1f, 0.1f, 0.1f, 0.1f }, (vec4){ 0.0f, 0.0f, 0.0f, 0.0f });
  }
  else {
    pos.y += transform.w * 0.4f;
    render_text(r2d, pos, new_str, *f, color, new_s, 4, 3, false, (vec4){ 0.1f, 0.1f, 0.1f, 0.1f }, (vec4){ 0.0f, 0.0f, 0.0f, 0.0f });
  }
}

void render_slider_float(renderer_2d *r2d, vec4 transform, float *value, float min, float max, bool *slider_being_dragged, texture bar_t, vec4 bar_c, texture ball_t, vec4 ball_c, input_data *input) {
  float bar_size = 7.0f;
  float bar_indent = 16.0f;
  float bullet_size = 14.0f;
  if (LOOK_SLIDER) {
    bar_size = transform.w;
    bar_indent = 0;
    bullet_size = transform.w;
  }
  vec4 bar_transform = { transform.x + bar_indent, transform.y + (transform.w - bar_size) / 2.0f, transform.z - bar_indent * 2.0f, bar_size };
  vec4 bullet_transform = { bar_transform.x, bar_transform.y + (bar_size - bullet_size) / 2.0f, bullet_size / 2.0f, bullet_size };
  bullet_transform.x += max(min((*value - min) / (max - min), 1.0f), 0.0f) * (bar_transform.z - bullet_transform.z);
  render_fancy_box(r2d, bar_transform, bar_c, bar_t, 0, 0);
  bool hovered = false;
  bool clicked = false;
  if (slider_being_dragged && input->mouse_held) {
    hovered = true;
    clicked = true; 
  }
  else {
    if (aabb(bar_transform, input->mouse_pos)) {
      hovered = true;
      if (input->mouse_click) {
        clicked = true;
      }
    }
  }
  if (clicked) {
    *slider_being_dragged = true;
    float ball_size_half = bullet_transform.z / 2.0f;
    int begin = bar_transform.x + ball_size_half;
    int end = bar_transform.x + bar_transform.z - ball_size_half;
    int mouse_x = input->mouse_pos.x;
    float mouse_val = (mouse_x - (float)begin) / (end - (float)begin);
    mouse_val = clampf(mouse_val, 0.0f, 1.0f);
    mouse_val += min;
    *value = mouse_val;
  }
  else {
    *slider_being_dragged = false;
  }
  render_fancy_box(r2d, bullet_transform, ball_c, ball_t, hovered, clicked);
}

void render_slider_int(renderer_2d *r2d, vec4 transform, int *value, int min, int max, bool *slider_being_dragged, texture bar_t, vec4 bar_c, texture ball_t, vec4 ball_c, input_data *input) {
  float bar_size = 7.0f;
  float bar_indent = 16.0f;
  float bullet_size = 14.0f;
  if (LOOK_SLIDER) {
    bar_size = transform.w;
    bar_indent = 0;
    bullet_size = transform.w;
  }
  vec4 bar_transform = { transform.x + bar_indent, transform.y + (transform.w - bar_size) / 2.0f, transform.z - bar_indent * 2.0f, bar_size };
  vec4 bullet_transform = { bar_transform.x, bar_transform.y + (bar_size - bullet_size) / 2.0f, bullet_size / 2.0f, bullet_size };
  bullet_transform.x += max(min((*value - min) / (float)(max - min), 1.0f), 0.0f) * (bar_transform.z - bullet_transform.z);
  render_fancy_box(r2d, bar_transform, bar_c, bar_t, 0, 0);
  bool hovered = false;
  bool clicked = false;
  if (slider_being_dragged && input->mouse_held) {
    hovered = true;
    clicked = true; 
  }
  else {
    if (aabb(bar_transform, input->mouse_pos)) {
      hovered = true;
      if (input->mouse_click) {
        clicked = true;
      }
    }
  }
  if (clicked) {
    *slider_being_dragged = true;
    float ball_size_half = bullet_transform.z / 2.0f;
    int begin = bar_transform.x + ball_size_half;
    int end = bar_transform.x + bar_transform.z - ball_size_half;
    int mouse_x = input->mouse_pos.x;
    float mouse_val = (mouse_x - (float)begin) / (end - (float)begin);
    mouse_val = clampf(mouse_val, 0.0f, 1.0f);
    mouse_val += min;
    *value = mouse_val;
  }
  else {
    *slider_being_dragged = false;
  }
  render_fancy_box(r2d, bullet_transform, ball_c, ball_t, hovered, clicked);
}

bool draw_button(renderer_ui *rui, renderer_2d *r2d, font *f, vector(column_pair) columns, int current_column, char *first, widget *w, input_data input) {
  vec4 transform_drawn = columns[current_column].first;
  vec4 aabb_transform = columns[current_column].first;
  bool hovered = 0;
  bool clicked = 0;
  vec4 text_color = color_white;
  if (w->color.w <= 0.01f) {
    vec4 p = determine_text_pos(r2d, first, f, transform_drawn, true, true);
    aabb_transform = p;
  }
  if (aabb(aabb_transform, input.mouse_pos)) {
    hovered = true;
    if (input.mouse_held) {
      clicked = true;
      transform_drawn.x += transform_drawn.w * press_down_size;
      transform_drawn.y += transform_drawn.w * press_down_size;
      transform_drawn.z += transform_drawn.w * press_down_size;
      transform_drawn.w += transform_drawn.w * press_down_size;
    }
  }
  if (hovered && w->color.w <= 0.01f) {
    text_color = step_color_down(text_color, 0.8);
  }
  if (input.mouse_released && aabb(aabb_transform, input.mouse_pos)) {
    w->return_from_update = true;
  }
  else {
    w->return_from_update = false;
  }
  render_fancy_box(r2d, transform_drawn, w->color, w->texture, hovered, clicked);
  if (w->color.w <= 0.01f || w->texture.id == 0) {
    render_text_ui(r2d, first, f, transform_drawn, text_color, true, !hovered, false);
  }
  else {
    render_text_ui(r2d, first, f, transform_drawn, text_color, false, !hovered, false);
  }
  return w->return_from_update;
}

bool is_in_button(const vec2 *p, const vec4 *box) {
	return(p->x >= box->x && p->x <= box->x + box->z && p->y >= box->y && p->y <= box->y + box->w);
}

void render_frame(renderer_ui *rui, renderer_2d *r2d, font *f, vec2 mouse_pos, bool mouse_click, bool mouse_held, bool mouse_released, bool escape_released, char *typed_input, float delta_time) {
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
  for (int i = 0; i < vector_size(widgets_copy); i++) {
    widget_pair pair = widgets_copy[i];
    widget *find = ht_lookup(&rui->widgets, pair.first);
    if (find == NULL) {
      pair.second.used_this_frame = true;
      pair.second.just_created = true;
      ht_insert(&rui->widgets, &pair.first, &pair.second);
    }
    else {
      if (find->type != pair.second.type) {
        error_func("Reupdated a widget with a different type", user_defined_data);
      }
      if (find->used_this_frame == true) {
        error_func("Used a widget name more than once", user_defined_data);
      }
      persistent_data pd = find->pd;
      *find = pair.second;
      find->just_created = false;
      find->pd = pd;
      find->used_this_frame = true;
    }
    widget *w = ht_lookup(&rui->widgets, pair.first);
    switch (w->type) {
      case widget_button: {
        draw_button(rui, r2d, f, columns, current_column, pair.first, w, input);
        break;
      }
      case widget_toggle: {
        vec4 transform_drawn;
        transform_drawn = columns[current_column].first;
        bool hovered = 0;
        bool clicked = 0;
        vec4 toggle_transform = transform_drawn;
        vec4 text_transform = transform_drawn;
        text_transform.z -= toggle_transform.w;
        toggle_transform.z = toggle_transform.w;
        vec4 p = determine_text_pos(r2d, pair.first, f, text_transform, true, true);
        toggle_transform.x = p.x + p.z;
        vec4 aabb_box = p;
        aabb_box.z += toggle_transform.z;
        aabb_box.y = min(toggle_transform.y, text_transform.y);
        aabb_box.w = max(toggle_transform.w, text_transform.w);
        if (aabb(aabb_box, input.mouse_pos)) {
          hovered = true;
          if (input.mouse_held) {
            clicked = true;
            text_transform.y += transform_drawn.w * press_down_size;
            toggle_transform.y += transform_drawn.w *press_down_size;
          }
        }
        if (input.mouse_released && aabb(aabb_box, input.mouse_pos)) {
          *(bool *)(w->pointer) = !(*(bool *)(w->pointer));
        }
        w->return_from_update = *(bool *)(w->pointer);
        render_fancy_box(r2d, toggle_transform, w->color_2, w->texture, hovered, clicked);
        char * text = get_string(pair.first);
        if (w->return_from_update) {
          strcat(text, ": ON");
        }
        else {
          strcat(text, ": OFF");
        }
        if (hovered) {
          render_text_ui(r2d, text, f, text_transform, step_color_down(color_white, 0.8f), true, false, false); 
        }
        else {
          render_text_ui(r2d, text, f, text_transform, color_white, true, true, false); 
        }
        break;
      }
      case widget_text: {
        render_text_ui(r2d, pair.first, f, columns[current_column].first, pair.second.color, true, true, false);
        break;
      }
      case widget_text_input: {
        char *text = (char *)pair.second.pointer;
        size_t n = pair.second.text_size;
        int pos = strlen(text);
        bool enabled = pair.second.enabled;
        vec4 transform = columns[current_column].first;
        bool hovered = 0;
        bool clicked = 0;
        if (pair.second.only_one_enabled && pair.second.enabled) {
          if (is_in_button(&mouse_pos, &transform)) {
            hovered = true;
            if (mouse_click || mouse_held) {
              rui->current_text_box = pair.first;
              clicked = true;
            }
          }
          if (pair.first != rui->current_text_box) {
            enabled = 0;
          }
        }
        if (enabled) {
          for (char *i = typed_input; *i != '\0'; i++) {
            if (*i == 8) {
              if (pos > 0) {
                pos--;
                text[pos] = 0;
              }
            }
            else if (*i == '\n') {
              
            }
            else {
              if (pos < n -1) {
                text[pos] = *i;
                pos++;
                text[pos] = 0;
              }
            }
          }
        }
        if (pair.second.texture.id != 0) {
          render_fancy_box(r2d, transform, pair.second.color, w->texture, hovered, clicked);
        }
        char *text_copy = strdup(text);
        if (pair.second.display_text) {
          text_copy = strcat(get_string(pair.first), text_copy);
        }
        if (enabled) {
          if ((int)timer % 2) {
            strcat(text_copy, "|");
          }
        }
        render_text_ui(r2d, text_copy, f, transform, color_white, true, !hovered, false);
        break;
      }
      case widget_begin_menu: {
        if (draw_button(rui, r2d, f, columns, current_column, pair.first, w, input)) {
          vector_push_back(current_menu_stack, pair.first);
        }
        break;
      }
      case widget_texture: {
        render_texture_ui(r2d, columns[current_column].first, pair.second.texture, pair.second.color, pair.second.texture_coords);
        break;
      }
      case widget_button_with_texture: {
        bool hovered = false;
        bool clicked = false;
        vec4 transform_drawn = compute_texture_new_position(columns[current_column].first, pair.second.texture);
        vec4 aabb_pos = transform_drawn;
        vec4 color = pair.second.color;
        if (aabb(aabb_pos, input.mouse_pos)) {
          hovered = true;
          if (input.mouse_held) {
            clicked = true;
            transform_drawn.y += transform_drawn.w * press_down_size;
          }
        }
        if (hovered) {
          color = step_color_down(color, 0.8f);
        }
        if (input.mouse_released && aabb(aabb_pos, input.mouse_pos)) {
          w->return_from_update = true;
        }
        else {
          w->return_from_update = false;
        }
        render_texture_ui(r2d, transform_drawn, pair.second.texture, color, pair.second.texture_coords);
        break;
      }
      case widget_slider_float_w: {
        if (pair.second.max <= pair.second.min) {
          break;
        }
        vec4 computed_pos = columns[current_column].first;
        vec4 text_transform = { computed_pos.x, computed_pos.y, computed_pos.z / 2.0f, computed_pos.w };
        vec4 slider_transform = { computed_pos.x + computed_pos.z / 2.0f, computed_pos.y, computed_pos.z / 2.0f, computed_pos.w };
        if (LOOK_SLIDER) {
          text_transform = computed_pos;
          slider_transform = computed_pos;
        }
        float *value = (float *)pair.second.pointer;
        if (!value) {
          break;
        }
        *value = min(*value, pair.second.max);
        *value = max(*value, pair.second.min);
        char *text = pair.first;
        char buffer[50];
        snprintf(buffer, sizeof(buffer), "%0.2f", *value);
        text = get_string(text);
        strcat(text, ": ");
        strcat(text, buffer);
        render_slider_float(r2d, slider_transform, value, pair.second.min, pair.second.max, &pair.second.pd.slider_being_dragged, pair.second.texture, pair.second.color, pair.second.texture_over, pair.second.color_2, &input);
        render_text_ui(r2d, text, f, text_transform, pair.second.color_3, true, true, false);
        break;
      }
      case widget_color_picker_w: {
        vec4 computed_pos = columns[current_column].first;
        vec4 text_transform = { computed_pos.x, computed_pos.y, computed_pos.z / 4, computed_pos.w };
        vec4 transform_1 = { computed_pos.x + (computed_pos.z / 4.0f) * 1.0f, computed_pos.y, computed_pos.z / 4.0f, computed_pos.w };
        vec4 transform_2 = { computed_pos.x + (computed_pos.z / 4.0f) * 2.0f, computed_pos.y, computed_pos.z / 4.0f, computed_pos.w };
        vec4 transform_3 = { computed_pos.x + (computed_pos.z / 4.0f) * 3.0f, computed_pos.y, computed_pos.z / 4.0f, computed_pos.w };
        float *value;
        value = (float *)pair.second.pointer;
        if (!value) { 
          break;
        }
        vec4 color = { value[0], value[1], value[2], 1 };
        if (pair.second.color.w) {
          render_fancy_box(r2d, text_transform, color, pair.second.texture, false, false);
          render_text_ui(r2d, pair.first, f, text_transform, pair.second.color, true, true, false);
        }
        else {
          render_text_ui(r2d, pair.first, f, text_transform, color, true, true, false);
        }
        if (pair.second.color_2.w) {
          render_slider_float(r2d, transform_1, value + 0, 0, 1, &pair.second.pd.slider_being_dragged, pair.second.texture, pair.second.color_2, pair.second.texture_over, (vec4){ 1.0f, 0.0f, 0.0f, 1.0f }, &input);
          render_slider_float(r2d, transform_2, value + 1, 0, 1, &pair.second.pd.slider_being_dragged_2, pair.second.texture, pair.second.color_2, pair.second.texture_over, (vec4){ 0.0f, 1.0f, 0.0f, 1.0f }, &input);
          render_slider_float(r2d, transform_3, value + 2, 0, 1, &pair.second.pd.slider_being_dragged_3, pair.second.texture, pair.second.color_2, pair.second.texture_over, (vec4){ 0.0f, 0.0f, 1.0f, 1.0f }, &input);
        }
        else {
          render_slider_float(r2d, transform_1, value + 0, 0, 1, &pair.second.pd.slider_being_dragged, pair.second.texture, (vec4){ 1.0f, 0.0f, 0.0f, 1.0f }, pair.second.texture_over, (vec4) { 1.0f, 0.0f, 0.0f, 1.0f }, &input);
          render_slider_float(r2d, transform_2, value + 1, 0, 1, &pair.second.pd.slider_being_dragged_2, pair.second.texture, (vec4){ 0.0f, 1.0f, 0.0f, 1.0f }, pair.second.texture_over, (vec4) { 0.0f, 1.0f, 0.0f, 1.0f }, &input);
          render_slider_float(r2d, transform_3, value + 2, 0, 1, &pair.second.pd.slider_being_dragged_3, pair.second.texture, (vec4){ 0.0f, 0.0f, 1.0f, 1.0f }, pair.second.texture_over, (vec4) { 0.0f, 0.0f, 1.0f, 1.0f }, &input);
        }
        break;
      }
      case widget_new_column_w: {
        current_column++;
        columns[current_column].first.y -= columns[current_column].second;
        break;
      }
      case widget_slider_int_w: {
        if (pair.second.max_int <= pair.second.min_int) {
          break;
        }
        vec4 computed_pos = columns[current_column].first;
        vec4 text_transform = { computed_pos.x, computed_pos.y, computed_pos.z / 2.0f, computed_pos.w };
        vec4 slider_transform = { computed_pos.x + computed_pos.z / 2.0f, computed_pos.y, computed_pos.z / 2.0f, computed_pos.w };
        if (LOOK_SLIDER) {
          text_transform = computed_pos;
          slider_transform = computed_pos;
        }
        int *value = (int *)pair.second.pointer;
        if (!value) {
          break;
        }
        *value = min(*value, pair.second.max_int);
        *value = max(*value, pair.second.min_int);
        char *text = pair.first;
        char buffer[50];
        snprintf(buffer, sizeof(buffer), "%i", *value);
        text = get_string(text);
        strcat(text, ": ");
        strcat(text, buffer);
        render_slider_int(r2d, slider_transform, value, pair.second.min_int, pair.second.max_int, &pair.second.pd.slider_being_dragged, pair.second.texture, pair.second.color, pair.second.texture_over, pair.second.color_2, &input);
        render_text_ui(r2d, text, f, text_transform, pair.second.color_3, true, true, false);
        break;
      }
      case widget_custom: {
        pair.second.return_transform = columns[current_column].first;
        pair.second.custom_widget_used = true;
        pair.second.hovered = aabb(pair.second.return_transform, mouse_pos);
        pair.second.clicked = aabb(pair.second.return_transform, mouse_pos) && mouse_held;
        break;
      }
      case widget_options_toggle: {
        vec4 transform_drawn = columns[current_column].first;
        vec4 aabb_transform = columns[current_column].first;
        bool hovered = 0;
        bool clicked = 0;
        vec4 text_color = pair.second.color;
        size_t *index = (size_t *)pair.second.pointer;
        size_t stub = 0;
        if (!index) {
          index = &stub;
          error_func("NULL passed as an index for widget_toggle_options", user_defined_data);
        }
        if (w->color.w <= 0.01f) {
          vec4 p = determine_text_pos(r2d, pair.first, f, transform_drawn, true, true);
          aabb_transform = p;
        }
        int max_size = 1;
        int text_2_size = sizeof(pair.second.text_2);
        text_2_size /= sizeof(char *);
        for (int i = 0; i < text_2_size; i++) {
          char c = pair.second.text_2[i];
          if (c == '|') {
            max_size++;
          }
        }
        if (text_2_size == 0) {
          max_size = 0;
        }
        if (*index > max_size - 1) {
          *index = 0;
        }
        if (pair.second.pointer2) {
          text_color = ((vec4 *)pair.second.pointer2)[*index];
        }
        if (aabb(aabb_transform, input.mouse_pos)) {
          hovered = true;
          if (input.mouse_held) {
            clicked = true;
            transform_drawn.y += transform_drawn.w * press_down_size;
          }
        }
        if (hovered && w->color.w < 0.01f) {
          text_color = step_color_down(text_color, 0.8f);
        }
        if (input.mouse_released && aabb(text_color, input.mouse_pos)) {
          w->return_from_update = true;
          (*index)++;
        }
        else {
          w->return_from_update = false;
        }
        render_fancy_box(r2d, transform_drawn, w->color, w->texture, hovered, clicked);
        char *final_text;
        if (pair.second.display_text) {
          final_text = pair.first;
        }
        int current_increment = 0;
        for (int i = 0; i < text_2_size; i++) {
          if (current_increment == *index) {
            char c = pair.second.text_2[i];
            if (c == '|') {
              break;
            }
            final_text += c;
          }
          char c = pair.second.text_2[i];
          if (c == '|') {
            current_increment++;
          }
        }
        if ((w->color.w <= 0.01f || pair.second.texture.id == 0)) {
          render_text_ui(r2d, final_text, f, transform_drawn, text_color, true, !hovered, false);
        }
        else {
          render_text_ui(r2d, final_text, f, transform_drawn, text_color, false, !hovered, false);
        }
        int text_3_size = sizeof(pair.second.text_2);
        text_3_size /= sizeof(char *);
        if (pair.second.text_3 != 0 && hovered) {
          vec4 transform = transform_drawn;
          transform.x += transform.z * 0.1f;
          transform.y += transform.w * 1.1f;
          int lines = 1;
          for (int i = 0; i < text_3_size; i++) {
            char c = pair.second.text_3[i];
            if (c == '\n' || c == '\v') {
              lines++;
            }
          }
          transform.w *= lines;
          render_fancy_box(r2d, transform, step_color_down(w->color_2, 0.8f), w->texture, 0, 0);
          transform.x += transform.z * 0.1f;
          transform.y += transform.w * 0.1f;
          transform.z *= 0.9f;
          transform.w *= 0.9f;
          transform.w /= lines;
          int ind = 0;
          char *copy = "";
          for (int l = 1; l <=lines;) {
            if (pair.second.text_3[ind] == '\n' || pair.second.text_3[ind] == '\v') {
              render_text_ui(r2d, copy, f, transform, pair.second.color, 0, true, true);
              l++;
              copy = "";
              transform.y += transform.w;
            }
            else {
              copy += pair.second.text_3[ind];
            }
            ind++;
            if (ind >= text_3_size) {
              break;
            }
          }
          render_text_ui(r2d, copy, f, transform, pair.second.color, 0, true, true);
        }
        break;
      }
    }
    w->just_created = false;
    w->last_frame_data = input;
    columns[current_column].first.y += columns[current_column].second;
  }
  hash_table widgets2;
  ht_reserve(&widgets2, rui->widgets.size);
  for (int i = 0; i < rui->widgets.size; i++) {
    widget *k = (widget *)rui->widgets.nodes[i]->key;
    widget *w = (widget *)rui->widgets.nodes[i]->value;
    if (w->used_this_frame) {
      w->used_this_frame = false;
      ht_insert(&widgets2, k, w);
    }
  }
  rui->widgets = widgets2;
  r2d->current_camera = c;
  vector_clear(rui->widgets_vector);
  int id_str_size = sizeof(rui->id_str);
  id_str_size /= sizeof(char *);
  if (id_str_size != 0) {
    error_func("More pushes than pops", user_defined_data);
  }
  rui->id_str = "";
}

bool button_ui(renderer_ui *rui, char *name, const vec4 color, const texture t) {
  strcat(name, rui->id_str);
  widget w = { 0 };
  w.type = widget_button;
  w.color = color;
  w.texture = t;
  w.used_this_frame = true;
  w.just_created = true;
  widget_pair wp = { name, w };
  vector_push_back(rui->widgets_vector, wp);
  widget_pair *find = (widget_pair *)ht_lookup(&rui->widgets, name);
  if (find != NULL) {
    return find->second.return_from_update;
  }
  else {
    return false;
  }
}
