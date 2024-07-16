#include "khg_math/minmax.h"
#include "khg_math/vec3.h"
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

bool aabb(vec4 transform, vec2 point) {
  if (point.x >= transform.x && point.y >= transform.y && point.x <= transform.x + transform.z && point.y <= transform.y + transform.w) {
    return true;
  }
  else {
    return false;
  }
}

void split_transforms(vec4 *down, vec4 *newTransform, vec4 *transform) {
  down = transform;
  newTransform = transform;
  float border = shadow_size * min(transform->w, transform->z);
  down->w = border;
  newTransform->w -= border;
  down->y += newTransform->w;
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
  vec3 hsv = hsv_color((vec3){ color.x, color.y, color.z });
  if (hsv.z >= 1.f) {
    hsv.y = max(0.f, hsv.y - perc * 2);
  }
  else {
    hsv.z = min(1.f, hsv.z + perc);
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

string *get_string(string *s) {
  int f = string_find(s, "##", 0);
  if (f != -1) {
    string *subs = string_substr(s, 0, (size_t)f);
    string_deallocate(s);
    return subs;
  }
  return s;
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
  else if(color_dim < 0) {
    new_color = step_color_down(color, -color_dim);
  }
  else {
    new_color = color;
  }
  vec4 light_color = step_color_up(new_color, 0.02);
  vec4 dark_color = step_color_down(new_color, 0.5f);
  vec4 darker_color = step_color_down(new_color, 0.25f);
  vec4 outline_color = step_color_up(new_color, 0.3);
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
    split_transforms(&down, &middle, &transform);
    render_rectangle(r2d, middle, color_vector, (vec2){ 0.0f, 0.0f }, 0.0f);
    render_rectangle(r2d, down, darker_color_vector, (vec2){ 0.0f, 0.0f }, 0.0f);
  }
  else {
    render_nine_patch(r2d, transform, new_color, (vec2){ 0.0f, 0.0f }, 0.0f, t, default_texture_coords, (vec4){ 0.2, 0.8, 0.8, 0.2 });
  }
}
