#include "khg_ui/elements.h"
#include "khg_ui/texture.h"
#include "khg_ui/internal.h"
#include "khg_ui/ui.h"
#include "khg_utils/error_func.h"
#include "cglm/affine.h"
#include "cglm/mat4.h"
#include "libclipboard/libclipboard.h"

ui_theme ui_default_theme() {
  ui_theme theme = { 0 };
  theme.div_props = (ui_element_props) {
    .color = (ui_color){ 45, 45, 45, 255 },
    .border_color = (ui_color){ 0, 0, 0, 0 }, 
    .border_width = 0.0f, 
    .corner_radius = 0.0f,
    .hover_color = ui_no_color,
  };
  float global_padding = 10;
  float global_margin = 5;
  theme.text_props = (ui_element_props) {
    .text_color = ui_white, 
    .border_color = ui_no_color,
    .padding = 0, 
    .margin_left = global_margin, 
    .margin_right = global_margin, 
    .margin_top = global_margin, 
    .margin_bottom = global_margin,
    .border_width = global_margin,
    .corner_radius = 0, 
    .hover_color = ui_no_color, 
    .hover_text_color = ui_no_color
  };
  theme.button_props = (ui_element_props) { 
    .color = ui_primary_item_color, 
    .text_color = ui_black,
    .border_color = ui_secondary_item_color,
    .padding = global_padding,
    .margin_left = global_margin, 
    .margin_right = global_margin, 
    .margin_top = global_margin, 
    .margin_bottom = global_margin, 
    .border_width = 4, 
    .corner_radius = 0, 
    .hover_color = ui_no_color, 
    .hover_text_color = ui_no_color
  };
  theme.image_props = (ui_element_props) { 
    .color = ui_white,
    .margin_left = global_margin, 
    .margin_right = global_margin, 
    .margin_top = global_margin, 
    .margin_bottom = global_margin,
    .corner_radius = 0,
    .hover_color = ui_no_color, 
    .hover_text_color = ui_no_color
  };
  theme.input_field_props = (ui_element_props) { 
    .color = ui_primary_item_color,
    .text_color = ui_black,
    .border_color = ui_secondary_item_color,
    .padding = global_padding, 
    .margin_left = global_margin, 
    .margin_right = global_margin, 
    .margin_top = global_margin, 
    .margin_bottom = global_margin, 
    .border_width = 4,
    .corner_radius = 0,
    .hover_color = ui_no_color, 
    .hover_text_color = ui_no_color
  };
  theme.checkbox_props = (ui_element_props) { 
    .color = ui_primary_item_color, 
    .text_color = ui_white, 
    .border_color = ui_secondary_item_color,
    .padding = global_padding, 
    .margin_left = global_margin, 
    .margin_right = global_margin, 
    .margin_top = global_margin, 
    .margin_bottom = global_margin, 
    .border_width = 4,
    .corner_radius = 0,
    .hover_color = ui_no_color, 
    .hover_text_color = ui_no_color
  };
  theme.slider_props = (ui_element_props) { 
    .color = ui_primary_item_color, 
    .text_color = ui_secondary_item_color, 
    .border_color = ui_secondary_item_color,
    .padding = global_padding,
    .margin_left = global_margin, 
    .margin_right = global_margin, 
    .margin_top = global_margin, 
    .margin_bottom = global_margin, 
    .border_width = 4,
    .corner_radius = 0,
    .hover_color = ui_no_color, 
    .hover_text_color = ui_no_color
  };
  theme.scrollbar_props = (ui_element_props) { 
    .color = ui_secondary_item_color, 
    .border_color = ui_black,
    .padding = 0,
    .margin_left = 0, 
    .margin_right = 5, 
    .margin_top = 5, 
    .margin_bottom = 0, 
    .border_width = 0,
    .corner_radius = 0,
    .hover_color = ui_no_color, 
    .hover_text_color = ui_no_color
  };
  theme.font = ui_load_font_asset("inter", "ttf", 24);
  theme.div_scroll_max_velocity = 100.0f; 
  theme.div_scroll_velocity_deceleration = 0.92;
  theme.div_scroll_acceleration = 2.5f;
  theme.div_scroll_amount_px = 20.0f;
  theme.div_smooth_scroll = true;
  theme.scrollbar_width = 8;
  return theme;
}

ui_theme ui_get_theme() {
  return state.theme;
}

void ui_set_theme(ui_theme theme) {
  state.theme = theme;
}

void ui_resize_display(uint32_t display_width, uint32_t display_height) {
  state.dsp_w = display_width;
  state.dsp_h = display_height;
  set_projection_matrix();
  state.current_div.aabb.size.x = state.dsp_w;
  state.current_div.aabb.size.y = state.dsp_h;
}

ui_div *ui_div_begin_loc(vec2s pos, vec2s size, bool scrollable, float *scroll, float *scroll_velocity, const char *file, int32_t line) {
  bool hovered_div = ui_area_hovered(pos, size);
  if(hovered_div) {
    state.scroll_velocity_ptr = scroll_velocity;
    state.scroll_ptr = scroll;
  }
  uint64_t id = DJB2_INIT;
  id = djb2_hash(id, file, strlen(file));
  id = djb2_hash(id, &line, sizeof(line));
  if(state.element_id_stack != -1) {
    id = djb2_hash(id, &state.element_id_stack, sizeof(state.element_id_stack));
  }
  state.prev_pos_ptr = state.pos_ptr;
  state.prev_font_stack = state.font_stack;
  state.prev_line_height = state.current_line_height;
  state.prev_div = state.current_div;
  ui_element_props props = get_props_for(state.theme.div_props);
  state.div_props = props;
  ui_div div;
  div.aabb = (ui_aabb){ .pos = pos, .size = size };
  div.scrollable = scrollable;
  div.id = id;
  if(div.scrollable) {
    if(*scroll > 0) {
      *scroll = 0;
    }
    if(state.theme.div_smooth_scroll) {
      *scroll += *scroll_velocity;
      *scroll_velocity *= state.theme.div_scroll_velocity_deceleration;
      if(*scroll_velocity > -0.1 && state.div_velocity_accelerating) {
        *scroll_velocity = 0.0f;
      }
    }
  }
  state.pos_ptr = pos; 
  state.current_div = div;
  div.interact_state = div_container((vec2s){ pos.x - props.padding, pos.y - props.padding }, (vec2s){ size.x + props.padding * 2.0f, size.y + props.padding * 2.0f }, props, props.color, props.border_width, false, state.div_hoverable);
  if(hovered_div) {
    state.selected_div_tmp = div;
  }
  if(div.scrollable) {
    ui_set_ptr_y(*scroll + props.border_width + props.corner_radius);
  } 
  else {
    ui_set_ptr_y(props.border_width + props.corner_radius);
  }
  state.cull_start = (vec2s){ pos.x, pos.y + props.border_width };
  state.cull_end = (vec2s){ pos.x + size.x - props.border_width, pos.y + size.y - props.border_width };
  state.current_div = div;
  state.current_line_height = 0;
  state.font_stack = NULL;
  return &state.current_div;
}

void ui_div_end() {
  if(state.current_div.scrollable) {
    draw_scrollbar_on(&state.selected_div_tmp);
  }
  state.pos_ptr = state.prev_pos_ptr;
  state.font_stack = state.prev_font_stack;
  state.current_line_height = state.prev_line_height;
  state.current_div = state.prev_div;
  state.cull_start = (vec2s){ -1, -1 };
  state.cull_end = (vec2s){ -1, -1 };
}

ui_clickable_item_state ui_item_loc(vec2s size, const char *file, int32_t line) {
  ui_element_props props = get_props_for(state.theme.button_props);
  next_line_on_overflow((vec2s){ size.x + props.padding * 2.0f + props.margin_right + props.margin_left, size.y + props.padding * 2.0f + props.margin_bottom + props.margin_top }, state.div_props.border_width);
  state.pos_ptr.x += props.margin_left;
  state.pos_ptr.y += props.margin_top;
  ui_clickable_item_state item = button(file, line, state.pos_ptr, size, props, props.color, props.border_width, false, true);
  state.pos_ptr.x += size.x + props.margin_left + props.padding * 2.0f;
  state.pos_ptr.y -= props.margin_top;
  return item;
}

ui_clickable_item_state ui_button_loc(const char *text, const char *file, int32_t line) {
  return button_element_loc((void *)text, file, line, false);
}

ui_clickable_item_state ui_button_wide_loc(const wchar_t *text, const char *file, int32_t line) {
  return button_element_loc((void *)text, file, line, true);
}

ui_clickable_item_state ui_image_button_loc(ui_texture img, const char *file, int32_t line) {
  ui_element_props props = get_props_for(state.theme.button_props);
  float padding = props.padding;
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom = props.margin_bottom; 
  ui_color color = props.color;
  ui_color text_color = state.theme.button_props.text_color;
  ui_font font = get_current_font();
  next_line_on_overflow((vec2s){ img.width + padding * 2.0f, img.height + padding * 2.0f }, state.div_props.border_width);
  state.pos_ptr.x += margin_left;
  state.pos_ptr.y += margin_top;
  ui_clickable_item_state ret = button(file, line, state.pos_ptr, (vec2s){ img.width + padding * 2, img.height + padding * 2 }, props, color, props.border_width, true, true);
  ui_color imageColor = ui_white;
  ui_image_render((vec2s){state.pos_ptr.x + padding, state.pos_ptr.y + padding}, imageColor, img, ui_no_color, 0, props.corner_radius);
  state.pos_ptr.x += img.width + margin_right + padding * 2.0f;
  state.pos_ptr.y -= margin_top;
  return ret; 
}

ui_clickable_item_state ui_image_button_fixed_loc(ui_texture img, float width, float height, const char *file, int32_t line) {
  ui_element_props props = get_props_for(state.theme.button_props);
  float padding = props.padding;
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom = props.margin_bottom; 
  ui_color color = props.color;
  ui_color text_color = state.theme.button_props.text_color;
  ui_font font = get_current_font();
  int32_t render_width = ((width == -1) ? img.width : width);
  int32_t render_height = ((height == -1) ? img.height : height);
  next_line_on_overflow((vec2s){ render_width + padding * 2.0f, render_height + padding * 2.0f }, state.div_props.border_width);
  state.pos_ptr.x += margin_left;
  state.pos_ptr.y += margin_top;
  ui_clickable_item_state ret = button(file, line, state.pos_ptr, (vec2s){ render_width + padding * 2, render_height + padding * 2 }, props, color, props.border_width, true, true);
  ui_color imageColor = ui_white; 
  ui_image_render((vec2s){state.pos_ptr.x + padding + (render_width - img.width) / 2.0f, state.pos_ptr.y + padding}, imageColor, img, ui_no_color, 0, props.corner_radius);
  state.pos_ptr.x += render_width + margin_right + padding * 2.0f;
  state.pos_ptr.y -= margin_top;
  return ret;
}

ui_clickable_item_state ui_button_fixed_loc(const char *text, float width, float height, const char *file, int32_t line) {
  return button_fixed_element_loc((void*)text, width, height, file, line, false);
}

ui_clickable_item_state ui_button_fixed_loc_wide(const wchar_t *text, float width, float height, const char *file, int32_t line) {
  return button_fixed_element_loc((void*)text, width, height, file, line, true);
}

ui_clickable_item_state ui_slider_int_loc(ui_slider *slider, const char *file, int32_t line) {
  ui_element_props props = get_props_for(state.theme.button_props);
  float margin_left = props.margin_left, margin_right = props.margin_right, margin_top = props.margin_top, margin_bottom = props.margin_bottom;
  float handle_size;
  if (slider->handle_size != 0.0f) {
    handle_size = slider->handle_size;
  }
  else {
    handle_size = (slider->height != 0) ? slider->height * 4.0f : 20.0f;
  }
  if(slider->held) {
    handle_size = (slider->height != 0) ? slider->height * 4.5f : 22.5f;
  }
  float slider_width = (slider->width != 0) ? slider->width : 200;
  float slider_height = (slider->height != 0) ? slider->height : handle_size / 2.0f;
  ui_color color = props.color;
  next_line_on_overflow((vec2s){ slider_width + margin_right + margin_left, handle_size + margin_bottom + margin_top }, state.div_props.border_width);
  state.pos_ptr.x += margin_left;
  state.pos_ptr.y += margin_top;
  ui_element_props slider_props = props;
  slider_props.border_width /= 2.0f;
  ui_clickable_item_state slider_state = button_ex(file, line, state.pos_ptr, (vec2s){ (float)slider_width, (float)slider_height }, slider_props, color, 0, false, false, (vec2s){ -1, handle_size });
  slider->handle_pos = map_vals(*(int32_t *)slider->val, slider->min, slider->max, handle_size / 2.0f, slider->width - handle_size / 2.0f) - (handle_size) / 2.0f;
  ui_rect_render((vec2s){ state.pos_ptr.x + slider->handle_pos, state.pos_ptr.y - (handle_size) / 2.0f + slider_height / 2.0f }, (vec2s){ handle_size, handle_size }, props.text_color, props.border_color, props.border_width, slider->held ? props.corner_radius * 3.5f : props.corner_radius * 3.0f);
  if(slider_state == ui_clickable_held || slider_state == ui_clickable_clicked) {
    slider->held = true;
  }
  if(slider->held && ui_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)) {
    slider->held = false;
    slider_state = ui_clickable_clicked;
  }
  if(slider->held) {
    if(ui_get_mouse_x() >= state.pos_ptr.x && ui_get_mouse_x() <= state.pos_ptr.x + slider_width - handle_size) {
      slider->handle_pos = ui_get_mouse_x() - state.pos_ptr.x;
      *(int32_t *)slider->val = map_vals(state.pos_ptr.x + slider->handle_pos, state.pos_ptr.x,  state.pos_ptr.x + slider_width - handle_size, slider->min, slider->max);
    }
    else if(ui_get_mouse_x() <= state.pos_ptr.x) {
      *(int32_t *)slider->val = slider->min;
      slider->handle_pos = 0;
    } 
    else if(ui_get_mouse_x() >= state.pos_ptr.x + slider_width - handle_size) {
      *(int32_t *)slider->val = slider->max;
      slider->handle_pos = slider_width - handle_size;
    }
    slider_state = ui_clickable_held;
  }
  state.pos_ptr.x += slider_width + margin_right;
  state.pos_ptr.y -= margin_top;
  return slider_state;
}

ui_clickable_item_state ui_progress_bar_int_loc(float val, float min, float max, float width, float height, const char *file, int32_t line) {
  ui_element_props props = get_props_for(state.theme.slider_props);
  float margin_left = props.margin_left, margin_right = props.margin_right, margin_top = props.margin_top, margin_bottom = props.margin_bottom; 
  ui_color color = props.color;
  next_line_on_overflow((vec2s){ width + margin_right + margin_left, height + margin_bottom + margin_top }, state.div_props.border_width);
  state.pos_ptr.x += margin_left;
  state.pos_ptr.y += margin_top;
  ui_clickable_item_state bar = button(file, line, state.pos_ptr, (vec2s){ (float)width, (float)height },props, color, props.border_width, false, false);
  float pos_x = map_vals(val, min, max, 0, width);
  ui_push_element_id(1);
  ui_clickable_item_state handle = button(file, line, state.pos_ptr, (vec2s){ (float)pos_x, (float)height }, props, props.text_color, 0, false, false);
  ui_pop_element_id();
  state.pos_ptr.x += width + margin_right;
  state.pos_ptr.y -= margin_top;
  return bar;
}

ui_clickable_item_state ui_progress_stripe_int_loc(ui_slider *slider, const char *file, int32_t line) {
  ui_element_props props = get_props_for(state.theme.slider_props);
  float margin_left = props.margin_left, margin_right = props.margin_right, margin_top = props.margin_top, margin_bottom =props.margin_bottom; 
  const float handle_size = 20;
  const float height = (slider->height != 0) ? slider->height : handle_size / 2.0f; // px
  ui_color color = props.color;
  next_line_on_overflow((vec2s){ slider->width + margin_right + margin_left, slider->height + margin_bottom + margin_top }, state.div_props.border_width);
  state.pos_ptr.x += margin_left;
  state.pos_ptr.y += margin_top;
  ui_clickable_item_state bar = button(file, line, state.pos_ptr, (vec2s){ (float)slider->width, (float)height },props, color, props.border_width, false, false);
  slider->handle_pos = map_vals(*(int32_t *)slider->val, slider->min, slider->max, 0, slider->width);
  ui_push_element_id(1);
  ui_clickable_item_state handle = button(file, line, state.pos_ptr, (vec2s){(float)slider->handle_pos, (float)height}, props, props.text_color, 0, false, false);
  ui_pop_element_id();
  ui_rect_render((vec2s){ state.pos_ptr.x + slider->handle_pos, state.pos_ptr.y - (float)height / 2.0f }, (vec2s){ (float)slider->height * 2, (float)slider->height * 2 }, props.text_color, (ui_color){0.0f, 0.0f, 0.0f, 0.0f}, 0, props.corner_radius);
  state.pos_ptr.x += slider->width + margin_right;
  state.pos_ptr.y -= margin_top;
  return bar;
}

ui_clickable_item_state ui_checkbox_loc(const char *text, bool *val, ui_color tick_color, ui_color tex_color, const char *file, int32_t line) { 
  return checkbox_element_loc((void *)text, val, tick_color, tex_color, file, line, false);
}

ui_clickable_item_state ui_checkbox_wide_loc(const wchar_t *text, bool *val, ui_color tick_color, ui_color tex_color, const char* file, int32_t line) {
  return checkbox_element_loc((void *)text, val, tick_color, tex_color, file, line, true);
}

int32_t ui_menu_item_list_loc(const char ** items, uint32_t item_count, int32_t selected_index, ui_menu_item_callback per_cb, bool vertical, const char *file, int32_t line) {
  return menu_item_list_item_loc((void **)items, item_count, selected_index, per_cb, vertical, file, line, false);
}

int32_t ui_menu_item_list_loc_wide(const wchar_t **items, uint32_t item_count, int32_t selected_index, ui_menu_item_callback per_cb, bool vertical, const char *file, int32_t line) {
  return menu_item_list_item_loc((void **)items, item_count, selected_index, per_cb, vertical, file, line, true);
}

void ui_dropdown_menu_loc(const char **items, const char *placeholder, uint32_t item_count, float width, float height, int32_t *selected_index, bool *opened, const char *file, int32_t line) {
  return dropdown_menu_item_loc((void **)items, (void *)placeholder, item_count, width, height, selected_index, opened, file, line, false);
}

void ui_dropdown_menu_loc_wide(const wchar_t **items, const wchar_t *placeholder, uint32_t item_count, float width, float height, int32_t *selected_index, bool *opened, const char *file, int32_t line) {
  return dropdown_menu_item_loc((void **)items, (void *)placeholder, item_count, width, height, selected_index, opened, file, line, true);
}

void ui_input_text_loc(ui_input_field* input, const char *file, int32_t line) {
  input_field(input, INPUT_TEXT, file, line);
}

void ui_input_int_loc(ui_input_field *input, const char *file, int32_t line) {
  input_field(input, INPUT_INT, file, line);
}

void ui_input_float_loc(ui_input_field *input, const char *file, int32_t line) {
  input_field(input, INPUT_FLOAT, file, line);
}

void ui_input_insert_char_idx(ui_input_field *input, char c, uint32_t idx) {
  ui_input_field_unselect_all(input);
  insert_i_str(input->buf, c, idx); 
}

void ui_input_insert_str_idx(ui_input_field *input, const char *insert, uint32_t len, uint32_t idx) {
  if(len > input->buf_size || strlen(input->buf) + len > input->buf_size) {
    return;
  }
  insert_str_str(input->buf, insert, idx);
  ui_input_field_unselect_all(input); 
}

void ui_input_field_unselect_all(ui_input_field *input) {
  input->selection_start = -1;
  input->selection_end = -1;
  input->selection_dir = 0;
}

bool ui_input_grabbed() {
  return state.input_grabbed;
}

void ui_div_grab(ui_div div) {
  state.grabbed_div = div;
}

void ui_div_ungrab() {
  memset(&state.grabbed_div, 0, sizeof(ui_div));
  state.grabbed_div.id = -1;
}

bool ui_div_grabbed() {
  return state.grabbed_div.id != -1;
}

ui_div ui_get_grabbed_div() {
  return state.grabbed_div;
}

void ui_begin_loc(const char *file, int32_t line) {
  state.pos_ptr = (vec2s){ 0.0f, 0.0f };
  renderer_begin();
  ui_element_props props = get_props_for(state.theme.div_props);
  props.color = (ui_color){0, 0, 0, 0};
  ui_push_style_props(props);
  ui_div_begin(((vec2s){ 0.0f, 0.0f }), ((vec2s){ (float)state.dsp_w, (float)state.dsp_h }), true);
  ui_pop_style_props();
}

void ui_end() {
  ui_div_end();
  state.selected_div = state.selected_div_tmp;
  update_input();
  clear_events();
  renderer_flush();
  state.drawcalls = 0;
}

void ui_next_line() {
  state.pos_ptr.x = state.current_div.aabb.pos.x + state.div_props.border_width;
  state.pos_ptr.y += state.current_line_height;
  state.current_line_height = 0;
}

vec2s ui_text_dimension(const char* str) {
  return ui_text_dimension_ex(str, -1);
}

vec2s ui_text_dimension_ex(const char *str, float wrap_point) {
  ui_font font = get_current_font();
  ui_text_props props = ui_text_render((vec2s){ 0.0f, 0.0f }, str, font, state.theme.text_props.text_color, wrap_point, (vec2s){ -1, -1 }, true, false, -1, -1);
  return (vec2s){ (float)props.width, (float)props.height };
}

vec2s ui_text_dimension_wide(const wchar_t *str) {
  return ui_text_dimension_wide_ex(str, -1);
}

vec2s ui_text_dimension_wide_ex(const wchar_t *str, float wrap_point) {
  ui_font font = get_current_font();
  ui_text_props props = ui_text_render_wchar((vec2s){0.0f, 0.0f}, str, font, ui_no_color, wrap_point, (vec2s){-1, -1}, true, false, -1, -1);
  return (vec2s){ (float)props.width, (float)props.height };
}

vec2s ui_button_dimension(const char *text) {
  ui_element_props props = get_props_for(state.theme.button_props);
  float padding = props.padding;
  vec2s text_dimension = ui_text_dimension(text);
  return (vec2s){ text_dimension.x + padding * 2.0f, text_dimension.y + padding };
}

float ui_get_text_end(const char *str, float start_x) {
  ui_font font = get_current_font();
  ui_text_props props = text_render_simple((vec2s){ start_x, 0.0f }, str, font, state.theme.text_props.text_color, true);
  return props.end_x;
}

void ui_text(const char *text) {
  ui_element_props props = get_props_for(state.theme.text_props);
  float padding = props.padding;
  float margin_left = props.margin_left, margin_right = props.margin_right, margin_top = props.margin_top, margin_bottom = props.margin_bottom;
  ui_color text_color = props.text_color;
  ui_color color = props.color;
  ui_font font = get_current_font();
  ui_text_props text_props = ui_text_render(state.pos_ptr, text, font, text_color, state.text_wrap ? (state.current_div.aabb.size.x + state.current_div.aabb.pos.x) - margin_right - margin_left : -1, (vec2s){ -1, -1 }, true, false, -1, -1);
  next_line_on_overflow((vec2s){ text_props.width + padding * 2.0f + margin_left + margin_right, text_props.height + padding * 2.0f + margin_top + margin_bottom }, state.div_props.border_width);
  state.pos_ptr.x += margin_left;
  state.pos_ptr.y += margin_top;
  ui_text_render((vec2s){state.pos_ptr.x + padding, state.pos_ptr.y + padding}, text, font, text_color, state.text_wrap ? (state.current_div.aabb.size.x + state.current_div.aabb.pos.x) - margin_right - margin_left : -1, (vec2s){ -1, -1 }, false, false, -1, -1);
  state.pos_ptr.x += text_props.width + margin_right + padding;
  state.pos_ptr.y -= margin_top;
}

void ui_text_wide(const wchar_t *text) {
  ui_element_props props = get_props_for(state.theme.text_props);
  float padding = props.padding;
  float margin_left = props.margin_left, margin_right = props.margin_right, 
  margin_top = props.margin_top, margin_bottom = props.margin_bottom;
  ui_color text_color = props.text_color;
  ui_color color = props.color;
  ui_font font = get_current_font();
  ui_text_props text_props = ui_text_render_wchar(state.pos_ptr, text, font, text_color, (state.text_wrap ? (state.current_div.aabb.size.x + state.current_div.aabb.pos.x) - margin_right - margin_left : -1), (vec2s){-1, -1}, true, false, -1, -1);
  next_line_on_overflow((vec2s){ text_props.width + padding * 2.0f + margin_left + margin_right, text_props.height + padding * 2.0f + margin_top + margin_bottom }, state.div_props.border_width);
  state.pos_ptr.x += margin_left;
  state.pos_ptr.y += margin_top;
  ui_rect_render(state.pos_ptr, (vec2s){ text_props.width + padding * 2.0f, text_props.height + padding * 2.0f }, props.color, props.border_color, props.border_width, props.corner_radius);
  ui_text_render_wchar((vec2s){ state.pos_ptr.x + padding, state.pos_ptr.y + padding }, text, font, text_color, (state.text_wrap ? (state.current_div.aabb.size.x + state.current_div.aabb.pos.x) - margin_right - margin_left : -1), (vec2s){-1, -1}, false, false, -1, -1);
  state.pos_ptr.x += text_props.width + padding * 2.0f + margin_right + padding;
  state.pos_ptr.y -= margin_top;
}

void ui_set_text_wrap(bool wrap) {
  state.text_wrap = wrap;
}

ui_div ui_get_current_div() {
  return state.current_div;
}

ui_div ui_get_selected_div() {
  return state.selected_div;
}

ui_div *ui_get_current_div_ptr() {
  return &state.current_div;
}

ui_div *ui_get_selected_div_ptr() {
  return &state.selected_div;
}

void ui_set_ptr_x(float x) {
  state.pos_ptr.x = x + state.current_div.aabb.pos.x;
}

void ui_set_ptr_y(float y) {
  state.pos_ptr.y = y + state.current_div.aabb.pos.y;
}
void ui_set_ptr_x_absolute(float x) {
  state.pos_ptr.x = x;
}

void ui_set_ptr_y_absolute(float y) {
  state.pos_ptr.y = y;
}

float ui_get_ptr_x() {
  return state.pos_ptr.x;
}

float ui_get_ptr_y() {
  return state.pos_ptr.y;
}

uint32_t ui_get_display_width() {
  return state.dsp_w;
}

uint32_t ui_get_display_height() {
  return state.dsp_h;
}

void ui_push_font(ui_font *font) {
  state.font_stack = font;
}

void ui_pop_font() {
  state.font_stack = NULL;
}

static void renderer_add_glyph(stbtt_aligned_quad q, int32_t max_descended_char_height, ui_color color, uint32_t tex_index) {
  vec2s texcoords[4] = { q.s0, q.t0, q.s1, q.t0, q.s1, q.t1, q.s0, q.t1 };
  vec2s verts[4] = { 
    (vec2s){ q.x0, q.y0 + max_descended_char_height }, 
    (vec2s){ q.x1, q.y0 + max_descended_char_height }, 
    (vec2s){ q.x1, q.y1 + max_descended_char_height },
    (vec2s){ q.x0, q.y1 + max_descended_char_height }
  }; 
  for(uint32_t i = 0; i < 4; i++) {
    if(state.render.vert_count >= MAX_RENDER_BATCH) {
      renderer_flush();
      renderer_begin();
    }
    const vec2 verts_arr = {verts[i].x, verts[i].y};
    memcpy(state.render.verts[state.render.vert_count].pos, verts_arr, sizeof(vec2));
    const vec4 border_color = {0, 0, 0, 0};
    memcpy(state.render.verts[state.render.vert_count].border_color, border_color, sizeof(vec4));
    state.render.verts[state.render.vert_count].border_width = 0;
    vec4s color_zto = ui_color_to_zto(color);
    const vec4 color_arr = {color_zto.r, color_zto.g, color_zto.b, color_zto.a};
    memcpy(state.render.verts[state.render.vert_count].color, color_arr, sizeof(vec4));
    const vec2 texcoord_arr = {texcoords[i].x, texcoords[i].y};
    memcpy(state.render.verts[state.render.vert_count].texcoord, texcoord_arr, sizeof(vec2));
    state.render.verts[state.render.vert_count].tex_index = tex_index;
    const vec2 scale_arr = {0, 0};
    memcpy(state.render.verts[state.render.vert_count].scale, scale_arr, sizeof(vec2));
    const vec2 pos_px_arr = {0, 0};
    memcpy(state.render.verts[state.render.vert_count].pos_px, pos_px_arr, sizeof(vec2));
    state.render.verts[state.render.vert_count].corner_radius = 0;
    const vec2 cull_start_arr = {state.cull_start.x, state.cull_start.y};
    const vec2 cull_end_arr = {state.cull_end.x, state.cull_end.y};
    memcpy(state.render.verts[state.render.vert_count].min_coord, cull_start_arr, sizeof(vec2));
    memcpy(state.render.verts[state.render.vert_count].max_coord, cull_end_arr, sizeof(vec2));
    state.render.vert_count++;
  }
  state.render.index_count += 6; 
}

static wchar_t *str_to_wstr(const char *str) {
  size_t len = strlen(str) + 1;
  wchar_t *wstr = (wchar_t *)malloc(len * sizeof(wchar_t));
  if (wstr == NULL) {
    error_func("Memory allocation failed", user_defined_data);
    return NULL;
  }
  if (mbstowcs(wstr, str, len) == (size_t)-1) {
    error_func("Conversion failed", user_defined_data);
    free(wstr);
    return NULL;
  }
  return wstr;
}

ui_text_props ui_text_render(vec2s pos, const char *str, ui_font font, ui_color color, int32_t wrap_point, vec2s stop_point, bool no_render, bool render_solid, int32_t start_index, int32_t end_index) {
  wchar_t *wstr = str_to_wstr(str);
  ui_text_props textprops = ui_text_render_wchar(pos, (const wchar_t *)wstr, font, color, wrap_point, stop_point, no_render, render_solid, start_index, end_index);
  free(wstr);
  return textprops;
}

ui_text_props ui_text_render_wchar(vec2s pos, const wchar_t *str, ui_font font, ui_color color, int32_t wrap_point, vec2s stop_point, bool no_render, bool render_solid, int32_t start_index, int32_t end_index) {
  bool culled = item_should_cull((ui_aabb){ .pos = (vec2s){ pos.x, pos.y + get_current_font().font_size }, .size = (vec2s){ -1, -1 } });
  float tex_index = -1.0f;
  if (!culled && !no_render) {
    if (state.render.tex_count - 1 >= MAX_TEX_COUNT_BATCH - 1) {
      renderer_flush();
      renderer_begin();
    }
    for (uint32_t i = 0; i < state.render.tex_count; i++) {
      if (state.render.textures[i].id == font.bitmap.id) {
        tex_index = (float)i;
        break;
      }
    }
    if (tex_index == -1.0f) {
      tex_index = (float)state.render.tex_index;
      ui_texture tex = font.bitmap;
      state.render.textures[state.render.tex_count++] = tex;
      state.render.tex_index++;
    }
  }
  ui_text_props ret = { 0 };
  float x = pos.x;
  float y = pos.y;
  int32_t max_descended_char_height = get_max_char_height_font(font);
  float last_x = x;
  float height = get_max_char_height_font(font);
  float width = 0;
  uint32_t i = 0;
  while (str[i] != L'\0') {
    if (str[i] >= font.num_glyphs) {
      i++;
      continue;
    }
    if (stbtt_FindGlyphIndex((const stbtt_fontinfo *)font.font_info, str[i] - 32) == 0 && 
      str[i] != L' ' && str[i] != L'\n' && str[i] != L'\t' && !iswdigit(str[i]) && !iswpunct(str[i])) {
      i++;
      continue;
    }
    if (i >= end_index && end_index != -1) {
      break;
    }
    float word_width = 0;
    uint32_t j = i;
    while (str[j] != L' ' && str[j] != L'\n' && str[j] != L'\0') {
      stbtt_aligned_quad q;
      stbtt_GetBakedQuad((stbtt_bakedchar *)font.cdata, font.tex_width, font.tex_height, str[j] - 32, &word_width, &y, &q, 0);
      j++;
    }
    if (x + word_width > wrap_point && wrap_point != -1) {
      y += font.font_size;
      height += font.font_size;
      if (x - pos.x > width) {
        width = x - pos.x;
      }
      x = pos.x;
      last_x = x;
    }
    if (str[i] == L'\n') {
      y += font.font_size;
      height += font.font_size;
      if (x - pos.x > width) {
        width = x - pos.x;
      }
      x = pos.x;
      last_x = x;
      i++;
      continue;
    }
    stbtt_aligned_quad q;
    stbtt_GetBakedQuad((stbtt_bakedchar *)font.cdata, font.tex_width, font.tex_height, str[i] - 32, &x, &y, &q, 1);
    if (i < start_index && start_index != -1) {
      last_x = x;
      ret.rendered_count++;
      i++;
      continue;
    }
    if (stop_point.x != -1 && stop_point.y != -1) {
      if (x >= stop_point.x && stop_point.x != -1 && y + get_max_char_height_font(font) >= stop_point.y && stop_point.y != -1) {
        break;
      }
    } else {
      if (y + get_max_char_height_font(font) >= stop_point.y && stop_point.y != -1) {
        break;
      }
    }
    if (!culled && !no_render && state.renderer_render) {
      if (render_solid) {
        ui_rect_render((vec2s){ x, y }, (vec2s){ last_x - x, get_max_char_height_font(font) }, color, ui_no_color, 0.0f, 0.0f);
      } else {
        renderer_add_glyph(q, max_descended_char_height, color, tex_index);
      }
      last_x = x;
    }
    ret.rendered_count++;
    i++;
  }
  if (x - pos.x > width) {
    width = x - pos.x;
  }
  ret.width = width;
  ret.height = height;
  ret.end_x = x;
  ret.end_y = y;
  return ret;
 }

void ui_rect_render(vec2s pos, vec2s size, ui_color color, ui_color border_color, float border_width, float corner_radius) {
  if(!state.renderer_render) return;
  if(item_should_cull((ui_aabb){ .pos = pos, .size = size })) {
    return;
  }
  vec2s pos_initial = pos;
  pos = (vec2s){pos.x + size.x / 2.0f, pos.y + size.y / 2.0f};
  vec2s texcoords[4] = {(vec2s){ 1.0f, 1.0f }, (vec2s){ 1.0f, 0.0f }, (vec2s){ 0.0f, 0.0f }, (vec2s){ 0.0f, 1.0f } };
  mat4 translate; 
  mat4 scale;
  mat4 transform;
  vec3 pos_xyz = {(corner_radius != 0.0f ? (float)state.dsp_w / 2.0f : pos.x), (corner_radius != 0.0f ? (float)state.dsp_h / 2.0f : pos.y), 0.0f};
  vec3 size_xyz = {corner_radius != 0.0f ? state.dsp_w : size.x, corner_radius != 0.0f ? state.dsp_h : size.y, 0.0f};
  glm_translate_make(translate, pos_xyz);
  glm_scale_make(scale, size_xyz);
  glm_mat4_mul(translate,scale,transform);
  for(uint32_t i = 0; i < 4; i++) {
    if(state.render.vert_count >= MAX_RENDER_BATCH) {
      renderer_flush();
      renderer_begin();
    }
    vec4 result;
    glm_mat4_mulv(transform, state.render.vert_pos[i].raw, result);
    state.render.verts[state.render.vert_count].pos[0] = result[0];
    state.render.verts[state.render.vert_count].pos[1] = result[1];
    vec4s border_color_zto = ui_color_to_zto(border_color);
    const vec4 border_color_arr = {border_color_zto.r, border_color_zto.g, border_color_zto.b, border_color_zto.a};
    memcpy(state.render.verts[state.render.vert_count].border_color, border_color_arr, sizeof(vec4));
    state.render.verts[state.render.vert_count].border_width = border_width; 
    vec4s color_zto = ui_color_to_zto(color);
    const vec4 color_arr = {color_zto.r, color_zto.g, color_zto.b, color_zto.a};
    memcpy(state.render.verts[state.render.vert_count].color, color_arr, sizeof(vec4));
    const vec2 texcoord_arr = {texcoords[i].x, texcoords[i].y};
    memcpy(state.render.verts[state.render.vert_count].texcoord, texcoord_arr, sizeof(vec2));
    state.render.verts[state.render.vert_count].tex_index = -1;
    const vec2 scale_arr = {size.x, size.y};
    memcpy(state.render.verts[state.render.vert_count].scale, scale_arr, sizeof(vec2));
    const vec2 pos_px_arr = {(float)pos_initial.x, (float)pos_initial.y};
    memcpy(state.render.verts[state.render.vert_count].pos_px, pos_px_arr, sizeof(vec2));
    state.render.verts[state.render.vert_count].corner_radius = corner_radius;
    const vec2 cull_start_arr = {state.cull_start.x, state.cull_start.y};
    memcpy(state.render.verts[state.render.vert_count].min_coord, cull_start_arr, sizeof(vec2));
    const vec2 cull_end_arr = {state.cull_end.x, state.cull_end.y};
    memcpy(state.render.verts[state.render.vert_count].max_coord, cull_end_arr, sizeof(vec2));
    state.render.vert_count++;
  }
  state.render.index_count += 6;
}

void ui_image_render(vec2s pos, ui_color color, ui_texture tex, ui_color border_color, float border_width, float corner_radius) {
  if(!state.renderer_render) return;
  if(item_should_cull((ui_aabb){ .pos = pos, .size = (vec2s){ tex.width, tex.height } })) {
    return;
  }
  if(state.render.tex_count - 1 >= MAX_TEX_COUNT_BATCH - 1) {
    renderer_flush();
    renderer_begin();
  }
  vec2s pos_initial = pos;
  pos = (vec2s){pos.x + tex.width / 2.0f, pos.y + tex.height / 2.0f};
  if(state.image_color_stack.a != 0.0) {
    color = state.image_color_stack;
  }
  vec2s texcoords[4] = { (vec2s){ 0.0f, 0.0f }, (vec2s){ 1.0f, 0.0f }, (vec2s){ 1.0f, 1.0f }, (vec2s){ 0.0f, 1.0f } };
  float tex_index = -1.0f;
  for(uint32_t i = 0; i < state.render.tex_count; i++) {
    if(tex.id == state.render.textures[i].id)  {
      tex_index = i;
      break;
    }
  }
  if(tex_index == -1.0f) {
    tex_index = (float)state.render.tex_index;
    state.render.textures[state.render.tex_count++] = tex;
    state.render.tex_index++;
  }
  mat4 translate = GLM_MAT4_IDENTITY_INIT; 
  mat4 scale = GLM_MAT4_IDENTITY_INIT;
  mat4 transform = GLM_MAT4_IDENTITY_INIT;
  vec3s pos_xyz = (vec3s){pos.x, pos.y, 0.0f};
  vec3 tex_size;
  tex_size[0] = tex.width;
  tex_size[1] = tex.height;
  tex_size[2] = 0;
  glm_translate_make(translate, pos_xyz.raw);
  glm_scale_make(scale, tex_size);
  glm_mat4_mul(translate,scale,transform);
  for(uint32_t i = 0; i < 4; i++) {
    if(state.render.vert_count >= MAX_RENDER_BATCH) {
      renderer_flush();
      renderer_begin();
    }
    vec4 result;
    glm_mat4_mulv(transform, state.render.vert_pos[i].raw, result);
    memcpy(state.render.verts[state.render.vert_count].pos, result, sizeof(vec2));
    vec4s border_color_zto = ui_color_to_zto(border_color);
    const vec4 border_color_arr = {border_color_zto.r, border_color_zto.g, border_color_zto.b, border_color_zto.a};
    memcpy(state.render.verts[state.render.vert_count].border_color, border_color_arr, sizeof(vec4));
    state.render.verts[state.render.vert_count].border_width = border_width; 
    vec4s color_zto = ui_color_to_zto(color);
    const vec4 color_arr = {color_zto.r, color_zto.g, color_zto.b, color_zto.a};
    memcpy(state.render.verts[state.render.vert_count].color, color_arr, sizeof(vec4));
    const vec2 texcoord_arr = {texcoords[i].x, texcoords[i].y};
    memcpy(state.render.verts[state.render.vert_count].texcoord, texcoord_arr, sizeof(vec2));
    state.render.verts[state.render.vert_count].tex_index = tex_index;
    const vec2 scale_arr = {(float)tex.width, (float)tex.height};
    memcpy(state.render.verts[state.render.vert_count].scale, scale_arr, sizeof(vec2));
    vec2 pos_px_arr = {(float)pos_initial.x, (float)pos_initial.y};
    memcpy(state.render.verts[state.render.vert_count].pos_px, pos_px_arr, sizeof(vec2));
    state.render.verts[state.render.vert_count].corner_radius = corner_radius;
    const vec2 cull_start_arr = {state.cull_start.x, state.cull_start.y};
    memcpy(state.render.verts[state.render.vert_count].min_coord, cull_start_arr, sizeof(vec2));
    const vec2 cull_end_arr = {state.cull_end.x, state.cull_end.y};
    memcpy(state.render.verts[state.render.vert_count].max_coord, cull_end_arr, sizeof(vec2));
    state.render.vert_count++;
  } 
  state.render.index_count += 6;
}

bool ui_point_intersects_aabb(vec2s p, ui_aabb aabb) {
  vec2s min = { aabb.pos.x, aabb.pos.y };
  vec2s max = { aabb.pos.x + aabb.size.x, aabb.pos.y + aabb.size.y };
  if (p.x >= min.x && p.x <= max.x &&
    p.y >= min.y && p.y <= max.y) {
    return true;
  }
  return false;
}

bool ui_aabb_intersects_aabb(ui_aabb a, ui_aabb b) {
  vec2s minA = a.pos;
  vec2s maxA = (vec2s){ a.pos.x + a.size.x, a.pos.y + a.size.y };
  vec2s minB = b.pos;
  vec2s maxB = (vec2s){ b.pos.x + b.size.x, b.pos.y + b.size.y };
  return (minA.x >= minB.x && minA.x <= maxB.x && 
  minA.y >= minB.y && minA.y <= maxB.y);
}

void ui_push_style_props(ui_element_props props) {
  props_stack_push(&state.props_stack, props);
}

void ui_pop_style_props() {
  props_stack_pop(&state.props_stack);
}

bool ui_hovered(vec2s pos, vec2s size) {
  bool hovered = ui_get_mouse_x() <= (pos.x + size.x) && ui_get_mouse_x() >= (pos.x) && ui_get_mouse_y() <= (pos.y + size.y) && ui_get_mouse_y() >= (pos.y) && ((state.selected_div.id == state.current_div.id && state.grabbed_div.id == -1) || (state.grabbed_div.id == state.current_div.id && state.grabbed_div.id != -1));
  return hovered;
}

bool ui_area_hovered(vec2s pos, vec2s size) {
  bool hovered = ui_get_mouse_x() <= (pos.x + size.x) && ui_get_mouse_x() >= (pos.x) && ui_get_mouse_y() <= (pos.y + size.y) && ui_get_mouse_y() >= (pos.y);
  return hovered;
}

void ui_set_image_color(ui_color color) {
  state.image_color_stack = color;
}

void ui_unset_image_color() {
  state.image_color_stack = ui_no_color;
}

void ui_set_current_div_scroll(float scroll) {
  *state.scroll_ptr = scroll;
}

float ui_get_current_div_scroll() {
  return *state.scroll_ptr;
}

void ui_set_current_div_scroll_velocity(float scroll_velocity) {
  *state.scroll_velocity_ptr = scroll_velocity;
}

float ui_get_current_div_scroll_velocity() {
  return *state.scroll_ptr;
}

void ui_set_line_height(uint32_t line_height) {
  state.current_line_height = line_height;
}

uint32_t ui_get_line_height() {
  return state.current_line_height;
}

void ui_set_line_should_overflow(bool overflow) {
  state.line_overflow = overflow;
}

void ui_set_div_hoverable(bool clickable) {
  state.div_hoverable = clickable;
}

void ui_push_element_id(int64_t id) {
  state.element_id_stack = id;
}

void ui_pop_element_id() {
  state.element_id_stack = -1;
}

ui_color ui_color_brightness(ui_color color, float brightness) {
  uint32_t adjustedR = (int)(color.r * brightness);
  uint32_t adjustedG = (int)(color.g * brightness);
  uint32_t adjustedB = (int)(color.b * brightness);
  color.r = (unsigned char)(adjustedR > 255 ? 255 : adjustedR);
  color.g = (unsigned char)(adjustedG > 255 ? 255 : adjustedG);
  color.b = (unsigned char)(adjustedB > 255 ? 255 : adjustedB);
  return color; 
}

ui_color ui_color_alpha(ui_color color, uint8_t a) {
  return (ui_color){ color.r, color.g, color.b, a };
}

vec4s ui_color_to_zto(ui_color color) {
  return (vec4s){ color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
}

ui_color ui_color_from_hex(uint32_t hex) {
  ui_color color;
  color.r = (hex>> 16) & 0xFF;
  color.g = (hex >> 8) & 0xFF; 
  color.b = hex& 0xFF; 
  color.a = 255; 
  return color;
}

ui_color ui_color_from_zto(vec4s zto) {
  return (ui_color){ (uint8_t)(zto.r * 255.0f), (uint8_t)(zto.g * 255.0f), (uint8_t)(zto.b * 255.0f), (uint8_t)(zto.a * 255.0f) };
}

void ui_image(ui_texture tex) {
  ui_element_props props = get_props_for(state.theme.image_props);
  float margin_left = props.margin_left, margin_right = props.margin_right, margin_top = props.margin_top, margin_bottom = props.margin_bottom;
  ui_color color = props.color;
  next_line_on_overflow((vec2s){ tex.width + margin_left + margin_right, tex.height + margin_top + margin_bottom }, state.div_props.border_width);
  state.pos_ptr.x += margin_left; 
  state.pos_ptr.y += margin_top;
  ui_image_render(state.pos_ptr, color, tex, props.border_color, props.border_width, props.corner_radius);
  state.pos_ptr.x += tex.width + margin_right;
  state.pos_ptr.y -= margin_top;
}

void ui_rect(float width, float height, ui_color color, float corner_radius) {
  next_line_on_overflow((vec2s){ (float)width, (float)height }, state.div_props.border_width);
  ui_rect_render(state.pos_ptr, (vec2s){(float)width, (float)height}, color, (ui_color){ 0.0f, 0.0f, 0.0f, 0.0f }, 0, corner_radius);
  state.pos_ptr.x += width;
}

void ui_seperator() {
  ui_next_line();
  ui_element_props props = get_props_for(state.theme.button_props);
  state.pos_ptr.x += props.margin_left;
  state.pos_ptr.y += props.margin_top;
  const uint32_t seperator_height = 1;
  ui_set_line_height(props.margin_top + seperator_height + props.margin_bottom);
  ui_rect_render(state.pos_ptr, (vec2s){ state.current_div.aabb.size.x - props.margin_left * 2.0f, seperator_height }, props.color, ui_no_color, 0, props.corner_radius);
  state.pos_ptr.y -= props.margin_top;
  ui_next_line();
}

void ui_set_clipboard_text(const char* text) {
  clipboard_set_text(state.clipboard, text);
}

char *ui_get_clipboard_text() {
  return clipboard_text(state.clipboard);
}

void ui_set_no_render(bool no_render) {
  state.renderer_render = !no_render;
}

