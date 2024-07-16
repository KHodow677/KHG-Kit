#include "khg_2d/renderer_2d.h"
#include "khg_2d/texture.h"
#include "khg_2d/utils.h"
#include "khg_math/math.h"
#include "khg_math/minmax.h"
#include "khg_math/vec2.h"
#include "khg_math/vec3.h"
#include "khg_utils/string.h"
#include "khg_ui/glui.h"

enum widget_type {
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
  widget_custom_widget,
  widget_options_toggle,
};

bool use_look_slider = true;

float press_down_size = 0.04f;
float shadow_size = 0.1f;
float outline_size = 0.02f;
float text_fit = 1.2f;
float non_minimize_text_size = 0.9f;
float minimize_ratio = 0.8f;
float button_fit  = 0.6f;
float in_size_y = 0.8;
float in_size_x = 0.8;
float main_in_size_x = 0.9;
float main_in_size_y = 0.9;
float padding_columns = 0.9;

bool aabb_ui(vec4 transform, vec2 point) {
  if (point.x >= transform.x && point.y >= transform.y && point.x <= transform.x + transform.z && point.y <= transform.y + transform.w) {
    return true;
  }
  else {
    return false;
  }
}

void split_transforms_ui(vec4 *down, vec4 *newTransform, vec4 *transform) {
  down = transform;
  newTransform = transform;
  float border = shadow_size * min(transform->w, transform->z);
  down->w = border;
  newTransform->w -= border;
  down->y += newTransform->w;
}

vec3 hsv_color_ui(vec3 rgb) {
  vec3 hsv;
  float min, max, delta;
  min = min(rgb.x, rgb.z);
  max = max(rgb.x, max(rgb.y, rgb.z));
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

vec3 rgb_color_ui(vec3 hsv) {
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

vec4 step_color_up_ui(vec4 color, float perc) {
  vec3 hsv = hsv_color_ui((vec3){ color.x, color.y, color.z });
  if (hsv.z >= 1.f) {
    hsv.y = max(0.f, hsv.y - perc * 2);
  }
  else {
    hsv.z = min(1.f, hsv.z + perc);
  }
  hsv = rgb_color_ui(hsv);
  color.x = hsv.x;
  color.y = hsv.y;
  color.z = hsv.z;
  return color;
}

vec4 step_color_down_ui(vec4 color, float perc) {
  color.x *= perc;
  color.y *= perc;
  color.z *= perc;
  return color;
}

string *get_string_ui(string *s) {
  int f = string_find(s, "##", 0);
  if (f != -1) {
    string *subs = string_substr(s, 0, (size_t)f);
    string_deallocate(s);
    return subs;
  }
  return s;
}

void render_fancy_box_ui(renderer_2d *r2d, vec4 transform, vec4 color, texture t, bool hovered, bool clicked) {
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
    new_color = step_color_up_ui(color, color_dim);
  }
  else if(color_dim < 0) {
    new_color = step_color_down_ui(color, -color_dim);
  }
  else {
    new_color = color;
  }
  vec4 light_color = step_color_up_ui(new_color, 0.02);
  vec4 dark_color = step_color_down_ui(new_color, 0.5f);
  vec4 darker_color = step_color_down_ui(new_color, 0.25f);
  vec4 outline_color = step_color_up_ui(new_color, 0.3);
  vec4 color_vector[4] = {dark_color, dark_color, light_color, light_color};
  vec4 darker_color_vector[4] = { darker_color, darker_color, darker_color, darker_color };
  if (t.id == 0) {
    float calculated_outline = outline_size * min(transform.w, transform.z);
    if (hovered) {
      vec4 colors[4] = { outline_color, outline_color, outline_color, outline_color };
      render_rectangle(r2d, transform, colors, (vec2){ 0.0f, 0.0f }, 0);
    }
    else {
      vec4 colors[4] = { dark_color, dark_color, dark_color, dark_color };
      render_rectangle(r2d, transform, colors, (vec2){ 0.0f, 0.0f }, 0);
    }
    transform.x += calculated_outline;
    transform.y += calculated_outline;
    transform.z -= calculated_outline * 2;
    transform.w -= calculated_outline * 2;
    vec4 middle = { 0 };
    vec4 down = { 0 };
    split_transforms_ui(&down, &middle, &transform);
    render_rectangle(r2d, middle, color_vector, (vec2){ 0.0f, 0.0f }, 0.0f);
    render_rectangle(r2d, down, darker_color_vector, (vec2){ 0.0f, 0.0f }, 0.0f);
  }
  else {
    render_nine_patch(r2d, transform, new_color, (vec2){ 0.0f, 0.0f }, 0.0f, t, default_texture_coords, (vec4){ 0.2, 0.8, 0.8, 0.2 });
  }
}

float determine_text_size_ui(renderer_2d *r2d, string *str, font *f, vec4 transform, bool minimize) {
  string *new_str = get_string_ui(str);
  float size = text_fit;
  vec2 s = get_text_size(r2d, string_data(new_str), *f, size, 4, 3);
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

vec4 determine_text_pos_ui(renderer_2d *r2d, string *str, font *f, vec4 transform, bool no_texture, bool minimize) {
  string *new_str = get_string_ui(str);
  float new_s = determine_text_size_ui(r2d, new_str, f, transform, true);
  vec2 s = get_text_size(r2d, string_data(new_str), *f, new_s, 4, 3);
  vec2 pos = { transform.x, transform.y };
  pos.x += transform.z / 2.0f;
  pos.y += transform.w / 2.0f;
  vec2 offset = { s.x / 2.0f, s.y / 2.0f };
  pos = vec2_subtract(&pos, &offset);
  return (vec4){ pos.x, pos.y, s.x, s.y };
}

void render_text_ui(renderer_2d *r2d, string *str, font *f, vec4 transform, vec4 color, bool no_texture, bool minimize, bool align_left) {
  string *new_str = get_string_ui(str);
  float new_s = determine_text_size_ui(r2d, new_str, f, transform, minimize);
  vec2 pos = { transform.x, transform.y };
  if (!align_left) {
    pos.x += transform.z / 2.f;
    pos.y += transform.w / 3.f;
    render_text(r2d, pos, string_data(new_str), *f, color, new_s, 4, 3, true, (vec4){ 0.1f, 0.1f, 0.1f, 1.0f }, (vec4){ 0.0f, 0.0f, 0.0f, 0.0f });
  }
  else {
    pos.y += transform.w * 0.4;
    render_text(r2d, pos, string_data(new_str), *f, color, new_s, 4, 3, false, (vec4){ 0.1f, 0.1f, 0.1f, 1.0f }, (vec4){ 0.0f, 0.0f, 0.0f, 0.0f });
  }
}

vec4 compute_texture_new_position_ui(vec4 transform, texture t) {
  vec2 t_size = get_texture_size(&t);
  if (t_size.y == 0) {
    return (vec4) { 0.0f, 0.0f, 0.0f, 0.0f };
  }
  if (transform.w == 0) {
    return (vec4) { 0.0f, 0.0f, 0.0f, 0.0f };
  }
  float aspect_ratio = t_size.x / (float)t_size.y;
  float box_aspect_ratio = transform.z / transform.w;
  if (aspect_ratio < box_aspect_ratio) {
    vec2 new_size = {};
    new_size.y = transform.w;
    new_size.x = aspect_ratio * new_size.y;
    vec4 new_pos = { transform.x, transform.y, new_size.x, new_size.y };
    new_pos.x += (transform.z - new_size.x) / 2.0f;
    return new_pos;
  }
  else if (aspect_ratio > box_aspect_ratio) {
    vec2 new_size = {};
    new_size.x = transform.z;
    new_size.y = new_size.x / aspect_ratio;
    vec4 new_pos = { transform.x, transform.y, new_size.x, new_size.y };
    new_pos.y += (transform.w - new_size.y) / 2.0f;
    return new_pos;
  }
  return transform;
}

void render_texture_ui(renderer_2d *r2d, vec4 transform, texture t, vec4 c, vec4 tex_coords) {
  vec4 new_pos = compute_texture_new_position_ui(transform, t);
  vec4 colors[4] = { c, c, c, c };
  render_rectangle_texture(r2d, new_pos, t, colors, (vec2){ 0.0f, 0.0f }, 0.0f, default_texture_coords);
}

void renderer_slider_float_ui(renderer_2d *r2d, vec4 transform, float *value, float min, float max, bool *slider_being_dragged, texture bar_t, vec4 bar_c, texture ball_t, vec4 ball_c, struct input_data *input) {
  float bar_size = 7.0f;
  float bar_indent = 16.0f;
  float bullet_size = 14.0f;
  if (use_look_slider) {
    bar_size = transform.w;
    bar_indent = 0;
    bullet_size = transform.w;
  }
  vec4 bar_transform = { transform.x + bar_indent, transform.y + (transform.w - bar_size) / 2.0f, transform.z - bar_indent * 2.0f, bar_size };
  vec4 bullet_transform = { bar_transform.x, bar_transform.y + (bar_size - bullet_size) / 2.0f, bullet_size / 2.0f, bullet_size };
  bullet_transform.x += max(min((*value - min) / (max - min), 1.0f), 0.f);
  render_fancy_box_ui(r2d, bar_transform, bar_c, bar_t, false, false);
  bool hovered = false;
  bool clicked = true;
  if (*slider_being_dragged == true && input->mouse_held) {
    hovered = true;
    clicked = true;
  }
  else {
    if (aabb_ui(bar_transform, input->mouse_pos)) {
      hovered = true;
      if (input->mouse_click) {
        clicked = true;
      }
    }
  }
  if (clicked) {
    *slider_being_dragged = true;
    float ball_size_half = bullet_transform.z / 2;
    int begin = bar_transform.x + ball_size_half;
    int end = bar_transform.x + bar_transform.z - ball_size_half;
    int mouse_x = input->mouse_pos.x;
    float mouse_val = (mouse_x - (float)begin) / (end - (float)begin);
    mouse_val = clampf(mouse_val, 0.0f, 1.0f);
    mouse_val *= max - min;
    mouse_val += min;
    *value = mouse_val;
  }
  else {
    *slider_being_dragged = false;
  }
  render_fancy_box_ui(r2d, bullet_transform, ball_c, ball_t, hovered, clicked);
}

void renderer_slider_int_ui(renderer_2d *r2d, vec4 transform, int *value, int min, int max, bool *slider_being_dragged, texture bar_t, vec4 bar_c, texture ball_t, vec4 ball_c, struct input_data *input) {
  float bar_size = 7.0f;
  float bar_indent = 16.0f;
  float bullet_size = 14.0f;
  if (use_look_slider) {
    bar_size = transform.w;
    bar_indent = 0;
    bullet_size = transform.w;
  }
  vec4 bar_transform = { transform.x + bar_indent, transform.y + (transform.w - bar_size) / 2.0f, transform.z - bar_indent * 2.0f, bar_size };
  vec4 bullet_transform = { bar_transform.x, bar_transform.y + (bar_size - bullet_size) / 2.0f, bullet_size / 2.0f, bullet_size };
  bullet_transform.x += max(min((*value - min) / (float)(max - min), 1.0f), 0.f);
  render_fancy_box_ui(r2d, bar_transform, bar_c, bar_t, false, false);
  bool hovered = false;
  bool clicked = true;
  if (*slider_being_dragged == true && input->mouse_held) {
    hovered = true;
    clicked = true;
  }
  else {
    if (aabb_ui(bar_transform, input->mouse_pos)) {
      hovered = true;
      if (input->mouse_click) {
        clicked = true;
      }
    }
  }
  if (clicked) {
    *slider_being_dragged = true;
    float ball_size_half = bullet_transform.z / 2;
    int begin = bar_transform.x + ball_size_half;
    int end = bar_transform.x + bar_transform.z - ball_size_half;
    int mouse_x = input->mouse_pos.x;
    float mouse_val = (mouse_x - (float)begin) / (end - (float)begin);
    mouse_val = clampf(mouse_val, 0.0f, 1.0f);
    mouse_val *= max - min;
    mouse_val += min;
    *value = mouse_val;
  }
  else {
    *slider_being_dragged = false;
  }
  render_fancy_box_ui(r2d, bullet_transform, ball_c, ball_t, hovered, clicked);
}

