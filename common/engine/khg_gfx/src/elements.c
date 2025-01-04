#include "cglm/affine.h"
#include "cglm/mat4.h"
#include "cglm/types.h"
#include "cglm/types-struct.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/font.h"
#include "khg_gfx/internal.h"
#include "khg_gfx/texture.h"
#include "khg_gfx/ui.h"
#include "libclipboard/libclipboard.h"
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

void compute_bounding_box(unsigned int width, unsigned int height, float angle, float* boundingWidth, float* boundingHeight) {
  float cosA = fabs(cos(angle));
  float sinA = fabs(sin(angle));
  *boundingWidth = width * cosA + height * sinA;
  *boundingHeight = width * sinA + height * cosA;
}

gfx_theme gfx_default_theme() {
  gfx_theme theme = { 0 };
  theme.div_props = (gfx_element_props) {
    .color = (gfx_color){ 45, 45, 45, 255 },
    .border_color = (gfx_color){ 0, 0, 0, 0 }, 
    .border_width = 0.0f, 
    .corner_radius = 0.0f,
    .hover_color = GFX_NO_COLOR,
  };
  float global_padding = 10;
  float global_margin = 5;
  theme.text_props = (gfx_element_props) {
    .text_color = GFX_WHITE, 
    .border_color = GFX_NO_COLOR,
    .padding = 0, 
    .margin_left = global_margin, 
    .margin_right = global_margin, 
    .margin_top = global_margin, 
    .margin_bottom = global_margin,
    .border_width = global_margin,
    .corner_radius = 0, 
    .hover_color = GFX_NO_COLOR, 
    .hover_text_color = GFX_NO_COLOR
  };
  theme.button_props = (gfx_element_props) { 
    .color = GFX_PRIMARY_ITEM_COLOR, 
    .text_color = GFX_BLACK,
    .border_color = GFX_SECONDARY_ITEM_COLOR,
    .padding = global_padding,
    .margin_left = global_margin, 
    .margin_right = global_margin, 
    .margin_top = global_margin, 
    .margin_bottom = global_margin, 
    .border_width = 4, 
    .corner_radius = 0, 
    .hover_color = GFX_NO_COLOR, 
    .hover_text_color = GFX_NO_COLOR
  };
  theme.image_props = (gfx_element_props) { 
    .color = GFX_WHITE,
    .margin_left = global_margin, 
    .margin_right = global_margin, 
    .margin_top = global_margin, 
    .margin_bottom = global_margin,
    .corner_radius = 0,
    .hover_color = GFX_NO_COLOR, 
    .hover_text_color = GFX_NO_COLOR
  };
  theme.input_field_props = (gfx_element_props) { 
    .color = GFX_PRIMARY_ITEM_COLOR,
    .text_color = GFX_BLACK,
    .border_color = GFX_SECONDARY_ITEM_COLOR,
    .padding = global_padding, 
    .margin_left = global_margin, 
    .margin_right = global_margin, 
    .margin_top = global_margin, 
    .margin_bottom = global_margin, 
    .border_width = 4,
    .corner_radius = 0,
    .hover_color = GFX_NO_COLOR, 
    .hover_text_color = GFX_NO_COLOR
  };
  theme.checkbox_props = (gfx_element_props) { 
    .color = GFX_PRIMARY_ITEM_COLOR, 
    .text_color = GFX_WHITE, 
    .border_color = GFX_SECONDARY_ITEM_COLOR,
    .padding = global_padding, 
    .margin_left = global_margin, 
    .margin_right = global_margin, 
    .margin_top = global_margin, 
    .margin_bottom = global_margin, 
    .border_width = 4,
    .corner_radius = 0,
    .hover_color = GFX_NO_COLOR, 
    .hover_text_color = GFX_NO_COLOR
  };
  theme.slider_props = (gfx_element_props) { 
    .color = GFX_PRIMARY_ITEM_COLOR, 
    .text_color = GFX_SECONDARY_ITEM_COLOR, 
    .border_color = GFX_SECONDARY_ITEM_COLOR,
    .padding = global_padding,
    .margin_left = global_margin, 
    .margin_right = global_margin, 
    .margin_top = global_margin, 
    .margin_bottom = global_margin, 
    .border_width = 4,
    .corner_radius = 0,
    .hover_color = GFX_NO_COLOR, 
    .hover_text_color = GFX_NO_COLOR
  };
  theme.scrollbar_props = (gfx_element_props) { 
    .color = GFX_SECONDARY_ITEM_COLOR, 
    .border_color = GFX_BLACK,
    .padding = 0,
    .margin_left = 0, 
    .margin_right = 5, 
    .margin_top = 5, 
    .margin_bottom = 0, 
    .border_width = 0,
    .corner_radius = 0,
    .hover_color = GFX_NO_COLOR, 
    .hover_text_color = GFX_NO_COLOR
  };
  theme.font = gfx_load_font_asset("res/assets/fonts/inter.ttf", 24);
  theme.div_scroll_max_velocity = 100.0f; 
  theme.div_scroll_velocity_deceleration = 0.92;
  theme.div_scroll_acceleration = 2.5f;
  theme.div_scroll_amount_px = 20.0f;
  theme.div_smooth_scroll = true;
  theme.scrollbar_width = 8;
  return theme;
}

gfx_theme gfx_get_theme() {
  return GFX_STATE.theme;
}

void gfx_set_theme(gfx_theme theme) {
  GFX_STATE.theme = theme;
}

void gfx_resize_display(unsigned int display_width, unsigned int display_height) {
  GFX_STATE.dsp_w = display_width;
  GFX_STATE.dsp_h = display_height;
  gfx_internal_set_projection_matrix();
  GFX_STATE.current_div.aabb.size.x = GFX_STATE.dsp_w;
  GFX_STATE.current_div.aabb.size.y = GFX_STATE.dsp_h;
}

gfx_div *gfx_div_begin_loc(vec2s pos, vec2s size, bool scrollable, float *scroll, float *scroll_velocity, const char *file, int line) {
  bool hovered_div = gfx_area_hovered(pos, size);
  if (hovered_div) {
    GFX_STATE.scroll_velocity_ptr = scroll_velocity;
    GFX_STATE.scroll_ptr = scroll;
  }
  unsigned long id = GFX_DJB2_INIT;
  id = gfx_internal_djb2_hash(id, file, strlen(file));
  id = gfx_internal_djb2_hash(id, &line, sizeof(line));
  if (GFX_STATE.element_id_stack != -1) {
    id = gfx_internal_djb2_hash(id, &GFX_STATE.element_id_stack, sizeof(GFX_STATE.element_id_stack));
  }
  GFX_STATE.prev_pos_ptr = GFX_STATE.pos_ptr;
  GFX_STATE.prev_font_stack = GFX_STATE.font_stack;
  GFX_STATE.prev_line_height = GFX_STATE.current_line_height;
  GFX_STATE.prev_div = GFX_STATE.current_div;
  gfx_element_props props = gfx_internal_get_props_for(GFX_STATE.theme.div_props);
  GFX_STATE.div_props = props;
  gfx_div div;
  div.aabb = (gfx_aabb){ .pos = pos, .size = size };
  div.scrollable = scrollable;
  div.id = id;
  if (div.scrollable) {
    if (*scroll > 0) {
      *scroll = 0;
    }
    if (GFX_STATE.theme.div_smooth_scroll) {
      *scroll += *scroll_velocity;
      *scroll_velocity *= GFX_STATE.theme.div_scroll_velocity_deceleration;
      if (*scroll_velocity > -0.1 && GFX_STATE.div_velocity_accelerating) {
        *scroll_velocity = 0.0f;
      }
    }
  }
  GFX_STATE.pos_ptr = pos; 
  GFX_STATE.current_div = div;
  div.interact_state = gfx_internal_div_container((vec2s){ pos.x - props.padding, pos.y - props.padding }, (vec2s){ size.x + props.padding * 2.0f, size.y + props.padding * 2.0f }, props, props.color, props.border_width, false, GFX_STATE.div_hoverable);
  if (hovered_div) {
    GFX_STATE.selected_div_tmp = div;
  }
  if (div.scrollable) {
    gfx_set_ptr_y(*scroll + props.border_width + props.corner_radius);
  } 
  else {
    gfx_set_ptr_y(props.border_width + props.corner_radius);
  }
  GFX_STATE.cull_start = (vec2s){ pos.x, pos.y + props.border_width };
  GFX_STATE.cull_end = (vec2s){ pos.x + size.x - props.border_width, pos.y + size.y - props.border_width };
  GFX_STATE.current_div = div;
  GFX_STATE.current_line_height = 0;
  GFX_STATE.font_stack = NULL;
  return &GFX_STATE.current_div;
}

void gfx_div_end() {
  if (GFX_STATE.current_div.scrollable) {
    gfx_internal_draw_scrollbar_on(&GFX_STATE.selected_div_tmp);
  }
  GFX_STATE.pos_ptr = GFX_STATE.prev_pos_ptr;
  GFX_STATE.font_stack = GFX_STATE.prev_font_stack;
  GFX_STATE.current_line_height = GFX_STATE.prev_line_height;
  GFX_STATE.current_div = GFX_STATE.prev_div;
  GFX_STATE.cull_start = (vec2s){ -1, -1 };
  GFX_STATE.cull_end = (vec2s){ -1, -1 };
}

gfx_clickable_item_state gfx_item_loc(vec2s size, const char *file, int line) {
  gfx_element_props props = gfx_internal_get_props_for(GFX_STATE.theme.button_props);
  gfx_internal_next_line_on_overflow((vec2s){ size.x + props.padding * 2.0f + props.margin_right + props.margin_left, size.y + props.padding * 2.0f + props.margin_bottom + props.margin_top }, GFX_STATE.div_props.border_width);
  GFX_STATE.pos_ptr.x += props.margin_left;
  GFX_STATE.pos_ptr.y += props.margin_top;
  gfx_clickable_item_state item = gfx_internal_button(file, line, GFX_STATE.pos_ptr, size, props, props.color, props.border_width, false, true);
  GFX_STATE.pos_ptr.x += size.x + props.margin_left + props.padding * 2.0f;
  GFX_STATE.pos_ptr.y -= props.margin_top;
  return item;
}

gfx_clickable_item_state gfx_button_loc(const char *text, const char *file, int line) {
  return gfx_internal_button_element_loc((void *)text, file, line);
}

gfx_clickable_item_state gfx_image_button_loc(gfx_texture img, const char *file, int line) {
  gfx_element_props props = gfx_internal_get_props_for(GFX_STATE.theme.button_props);
  float padding = props.padding;
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom = props.margin_bottom; 
  gfx_color color = props.color;
  gfx_color text_color = GFX_STATE.theme.button_props.text_color;
  gfx_font font = gfx_internal_get_current_font();
  gfx_internal_next_line_on_overflow((vec2s){ img.width + padding * 2.0f, img.height + padding * 2.0f }, GFX_STATE.div_props.border_width);
  GFX_STATE.pos_ptr.x += margin_left;
  GFX_STATE.pos_ptr.y += margin_top;
  gfx_clickable_item_state ret = gfx_internal_button(file, line, GFX_STATE.pos_ptr, (vec2s){ img.width + padding * 2, img.height + padding * 2 }, props, color, props.border_width, true, true);
  gfx_color imageColor = GFX_WHITE;
  gfx_image_render((vec2s){ GFX_STATE.pos_ptr.x + padding, GFX_STATE.pos_ptr.y + padding }, imageColor, img, GFX_NO_COLOR, 0, props.corner_radius, 0.0f, 0.0f, 0.0f, 1.0f, true, false);
  GFX_STATE.pos_ptr.x += img.width + margin_right + padding * 2.0f;
  GFX_STATE.pos_ptr.y -= margin_top;
  return ret; 
}

gfx_clickable_item_state gfx_image_button_fixed_loc(gfx_texture img, float width, float height, const char *file, int line) {
  gfx_element_props props = gfx_internal_get_props_for(GFX_STATE.theme.button_props);
  float padding = props.padding;
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom = props.margin_bottom; 
  gfx_color color = props.color;
  gfx_color text_color = GFX_STATE.theme.button_props.text_color;
  gfx_font font = gfx_internal_get_current_font();
  int render_width = ((width == -1) ? img.width : width);
  int render_height = ((height == -1) ? img.height : height);
  gfx_internal_next_line_on_overflow((vec2s){ render_width + padding * 2.0f, render_height + padding * 2.0f }, GFX_STATE.div_props.border_width);
  GFX_STATE.pos_ptr.x += margin_left;
  GFX_STATE.pos_ptr.y += margin_top;
  gfx_clickable_item_state ret = gfx_internal_button(file, line, GFX_STATE.pos_ptr, (vec2s){ render_width + padding * 2, render_height + padding * 2 }, props, color, props.border_width, true, true);
  gfx_color imageColor = GFX_WHITE; 
  gfx_image_render((vec2s){ GFX_STATE.pos_ptr.x + padding + (render_width - img.width) / 2.0f, GFX_STATE.pos_ptr.y + padding }, imageColor, img, GFX_NO_COLOR, 0, props.corner_radius, 0.0f, 0.0f, 0.0f, 1.0f, true, false);
  GFX_STATE.pos_ptr.x += render_width + margin_right + padding * 2.0f;
  GFX_STATE.pos_ptr.y -= margin_top;
  return ret;
}

gfx_clickable_item_state gfx_button_fixed_loc(const char *text, float width, float height, const char *file, int line) {
  return gfx_internal_button_fixed_element_loc((void*)text, width, height, file, line);
}

gfx_clickable_item_state gfx_slider_int_loc(gfx_slider *slider, const char *file, int line) {
  gfx_element_props props = gfx_internal_get_props_for(GFX_STATE.theme.button_props);
  float margin_left = props.margin_left, margin_right = props.margin_right, margin_top = props.margin_top, margin_bottom = props.margin_bottom;
  float handle_size;
  if (slider->handle_size != 0.0f) {
    handle_size = slider->handle_size;
  }
  else {
    handle_size = (slider->height != 0) ? slider->height * 4.0f : 20.0f;
  }
  if (slider->held) {
    handle_size = (slider->height != 0) ? slider->height * 4.5f : 22.5f;
  }
  float slider_width = (slider->width != 0) ? slider->width : 200;
  float slider_height = (slider->height != 0) ? slider->height : handle_size / 2.0f;
  gfx_color color = props.color;
  gfx_internal_next_line_on_overflow((vec2s){ slider_width + margin_right + margin_left, handle_size + margin_bottom + margin_top }, GFX_STATE.div_props.border_width);
  GFX_STATE.pos_ptr.x += margin_left;
  GFX_STATE.pos_ptr.y += margin_top;
  gfx_element_props slider_props = props;
  slider_props.border_width /= 2.0f;
  gfx_clickable_item_state slider_state = gfx_internal_button_ex(file, line, GFX_STATE.pos_ptr, (vec2s){ (float)slider_width, (float)slider_height }, slider_props, color, 0, false, false, (vec2s){ -1, handle_size });
  slider->handle_pos = gfx_internal_map_vals(*(int *)slider->val, slider->min, slider->max, handle_size / 2.0f, slider->width - handle_size / 2.0f) - (handle_size) / 2.0f;
  gfx_rect_render((vec2s){ GFX_STATE.pos_ptr.x + slider->handle_pos, GFX_STATE.pos_ptr.y - (handle_size) / 2.0f + slider_height / 2.0f }, (vec2s){ handle_size, handle_size }, props.text_color, props.border_color, props.border_width, slider->held ? props.corner_radius * 3.5f : props.corner_radius * 3.0f, 0.0f);
  if (slider_state == GFX_CLICKABLE_HELD || slider_state == GFX_CLICKABLE_CLICKED) {
    slider->held = true;
  }
  if (slider->held && gfx_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)) {
    slider->held = false;
    slider_state = GFX_CLICKABLE_CLICKED;
  }
  if(slider->held) {
    if (gfx_get_mouse_x() >= GFX_STATE.pos_ptr.x && gfx_get_mouse_x() <= GFX_STATE.pos_ptr.x + slider_width - handle_size) {
      slider->handle_pos = gfx_get_mouse_x() - GFX_STATE.pos_ptr.x;
      *(int *)slider->val = gfx_internal_map_vals(GFX_STATE.pos_ptr.x + slider->handle_pos, GFX_STATE.pos_ptr.x,  GFX_STATE.pos_ptr.x + slider_width - handle_size, slider->min, slider->max);
    }
    else if (gfx_get_mouse_x() <= GFX_STATE.pos_ptr.x) {
      *(int *)slider->val = slider->min;
      slider->handle_pos = 0;
    } 
    else if (gfx_get_mouse_x() >= GFX_STATE.pos_ptr.x + slider_width - handle_size) {
      *(int *)slider->val = slider->max;
      slider->handle_pos = slider_width - handle_size;
    }
    slider_state = GFX_CLICKABLE_HELD;
  }
  GFX_STATE.pos_ptr.x += slider_width + margin_right;
  GFX_STATE.pos_ptr.y -= margin_top;
  return slider_state;
}

gfx_clickable_item_state gfx_progress_bar_int_loc(float val, float min, float max, float width, float height, const char *file, int line) {
  gfx_element_props props = gfx_internal_get_props_for(GFX_STATE.theme.slider_props);
  float margin_left = props.margin_left, margin_right = props.margin_right, margin_top = props.margin_top, margin_bottom = props.margin_bottom; 
  gfx_color color = props.color;
  gfx_internal_next_line_on_overflow((vec2s){ width + margin_right + margin_left, height + margin_bottom + margin_top }, GFX_STATE.div_props.border_width);
  GFX_STATE.pos_ptr.x += margin_left;
  GFX_STATE.pos_ptr.y += margin_top;
  gfx_clickable_item_state bar = gfx_internal_button(file, line, GFX_STATE.pos_ptr, (vec2s){ (float)width, (float)height },props, color, props.border_width, false, false);
  float pos_x = gfx_internal_map_vals(val, min, max, 0, width);
  gfx_push_element_id(1);
  gfx_clickable_item_state handle = gfx_internal_button(file, line, GFX_STATE.pos_ptr, (vec2s){ (float)pos_x, (float)height }, props, props.text_color, props.border_width, false, false);
  gfx_pop_element_id();
  GFX_STATE.pos_ptr.x += width + margin_right;
  GFX_STATE.pos_ptr.y -= margin_top;
  return bar;
}

gfx_clickable_item_state gfx_progress_stripe_int_loc(gfx_slider *slider, const char *file, int line) {
  gfx_element_props props = gfx_internal_get_props_for(GFX_STATE.theme.slider_props);
  float margin_left = props.margin_left, margin_right = props.margin_right, margin_top = props.margin_top, margin_bottom =props.margin_bottom; 
  const float handle_size = 20;
  const float height = (slider->height != 0) ? slider->height : handle_size / 2.0f;
  gfx_color color = props.color;
  gfx_internal_next_line_on_overflow((vec2s){ slider->width + margin_right + margin_left, slider->height + margin_bottom + margin_top }, GFX_STATE.div_props.border_width);
  GFX_STATE.pos_ptr.x += margin_left;
  GFX_STATE.pos_ptr.y += margin_top;
  gfx_clickable_item_state bar = gfx_internal_button(file, line, GFX_STATE.pos_ptr, (vec2s){ (float)slider->width, (float)height },props, color, props.border_width, false, false);
  slider->handle_pos = gfx_internal_map_vals(*(int *)slider->val, slider->min, slider->max, 0, slider->width);
  gfx_push_element_id(1);
  gfx_clickable_item_state handle = gfx_internal_button(file, line, GFX_STATE.pos_ptr, (vec2s){ (float)slider->handle_pos, (float)height }, props, props.text_color, 0, false, false);
  gfx_pop_element_id();
  gfx_rect_render((vec2s){ GFX_STATE.pos_ptr.x + slider->handle_pos, GFX_STATE.pos_ptr.y - (float)height / 2.0f }, (vec2s){ (float)slider->height * 2, (float)slider->height * 2 }, props.text_color, (gfx_color){0.0f, 0.0f, 0.0f, 0.0f}, 0, props.corner_radius, 0.0f);
  GFX_STATE.pos_ptr.x += slider->width + margin_right;
  GFX_STATE.pos_ptr.y -= margin_top;
  return bar;
}

gfx_clickable_item_state gfx_checkbox_loc(const char *text, bool *val, gfx_color tick_color, gfx_color tex_color, const char *file, int line) { 
  return gfx_internal_checkbox_element_loc((void *)text, val, tick_color, tex_color, file, line);
}

int gfx_menu_item_list_loc(const char ** items, unsigned int item_count, int selected_index, gfx_menu_item_callback per_cb, bool vertical, const char *file, int line) {
  return gfx_internal_menu_item_list_item_loc((void **)items, item_count, selected_index, per_cb, vertical, file, line);
}

void gfx_dropdown_menu_loc(const char **items, const char *placeholder, unsigned int item_count, float width, float height, int *selected_index, bool *opened, const char *file, int line) {
  return gfx_internal_dropdown_menu_item_loc((void **)items, (void *)placeholder, item_count, width, height, selected_index, opened, file, line);
}

void gfx_input_text_loc(gfx_input_field* input, const char *file, int line) {
  gfx_internal_input_field(input, GFX_INPUT_TEXT, file, line);
}

void gfx_input_int_loc(gfx_input_field *input, const char *file, int line) {
  gfx_internal_input_field(input, GFX_INPUT_INT, file, line);
}

void gfx_input_float_loc(gfx_input_field *input, const char *file, int line) {
  gfx_internal_input_field(input, GFX_INPUT_FLOAT, file, line);
}

void gfx_input_insert_char_idx(gfx_input_field *input, char c, unsigned int idx) {
  gfx_input_field_unselect_all(input);
  gfx_internal_insert_i_str(input->buf, c, idx); 
}

void gfx_input_insert_str_idx(gfx_input_field *input, const char *insert, unsigned int len, unsigned int idx) {
  if (len > input->buf_size || strlen(input->buf) + len > input->buf_size) {
    return;
  }
  gfx_internal_insert_str_str(input->buf, insert, idx);
  gfx_input_field_unselect_all(input); 
}

void gfx_input_field_unselect_all(gfx_input_field *input) {
  input->selection_start = -1;
  input->selection_end = -1;
  input->selection_dir = 0;
}

bool gfx_input_grabbed() {
  return GFX_STATE.input_grabbed;
}

void gfx_div_grab(gfx_div div) {
  GFX_STATE.grabbed_div = div;
}

void gfx_div_ungrab() {
  memset(&GFX_STATE.grabbed_div, 0, sizeof(gfx_div));
  GFX_STATE.grabbed_div.id = -1;
}

bool gfx_div_grabbed() {
  return GFX_STATE.grabbed_div.id != -1;
}

gfx_div gfx_get_grabbed_div() {
  return GFX_STATE.grabbed_div;
}

void gfx_begin_loc(const char *file, int line) {
  GFX_STATE.pos_ptr = (vec2s){ 0.0f, 0.0f };
  gfx_internal_renderer_begin();
  gfx_element_props props = gfx_internal_get_props_for(GFX_STATE.theme.div_props);
  props.color = (gfx_color){ 0, 0, 0, 0 };
  gfx_push_style_props(props);
  gfx_div_begin(((vec2s){ 0.0f, 0.0f }), ((vec2s){ (float)GFX_STATE.dsp_w, (float)GFX_STATE.dsp_h }), true);
  gfx_pop_style_props();
}

void gfx_end() {
  gfx_div_end();
  GFX_STATE.selected_div = GFX_STATE.selected_div_tmp;
  gfx_internal_update_input();
  gfx_internal_clear_events();
  gfx_internal_renderer_flush();
  GFX_STATE.drawcalls = 0;
}

void gfx_next_line() {
  GFX_STATE.pos_ptr.x = GFX_STATE.current_div.aabb.pos.x + GFX_STATE.div_props.border_width;
  GFX_STATE.pos_ptr.y += GFX_STATE.current_line_height;
  GFX_STATE.current_line_height = 0;
}

vec2s gfx_text_dimension(const char *str) {
  return gfx_text_dimension_ex(str, -1);
}

vec2s gfx_text_dimension_ex(const char *str, float wrap_point) {
  gfx_font font = gfx_internal_get_current_font();
  gfx_text_props props = gfx_text_render((vec2s){ 0.0f, 0.0f }, str, font, GFX_STATE.theme.text_props.text_color, wrap_point, (vec2s){ -1, -1 }, true, false, -1, -1);
  return (vec2s){ (float)props.width, (float)props.height };
}

vec2s gfx_button_dimension(const char *text) {
  gfx_element_props props = gfx_internal_get_props_for(GFX_STATE.theme.button_props);
  float padding = props.padding;
  vec2s text_dimension = gfx_text_dimension(text);
  return (vec2s){ text_dimension.x + padding * 2.0f, text_dimension.y + padding };
}

float gfx_get_text_end(const char *str, float start_x) {
  gfx_font font = gfx_internal_get_current_font();
  gfx_text_props props = gfx_internal_text_render_simple((vec2s){ start_x, 0.0f }, str, font, GFX_STATE.theme.text_props.text_color, true);
  return props.end_x;
}

void gfx_text(const char *text) {
  gfx_element_props props = gfx_internal_get_props_for(GFX_STATE.theme.text_props);
  float padding = props.padding;
  float margin_left = props.margin_left, margin_right = props.margin_right, margin_top = props.margin_top, margin_bottom = props.margin_bottom;
  gfx_color text_color = props.text_color;
  gfx_color color = props.color;
  gfx_font font = gfx_internal_get_current_font();
  gfx_text_props text_props = gfx_text_render(GFX_STATE.pos_ptr, text, font, text_color, GFX_STATE.text_wrap ? (GFX_STATE.current_div.aabb.size.x + GFX_STATE.current_div.aabb.pos.x) - margin_right - margin_left : -1, (vec2s){ -1, -1 }, true, false, -1, -1);
  gfx_internal_next_line_on_overflow((vec2s){ text_props.width + padding * 2.0f + margin_left + margin_right, text_props.height + padding * 2.0f + margin_top + margin_bottom }, GFX_STATE.div_props.border_width);
  GFX_STATE.pos_ptr.x += margin_left;
  GFX_STATE.pos_ptr.y += margin_top;
  gfx_text_render((vec2s){ GFX_STATE.pos_ptr.x + padding, GFX_STATE.pos_ptr.y + padding }, text, font, text_color, GFX_STATE.text_wrap ? (GFX_STATE.current_div.aabb.size.x + GFX_STATE.current_div.aabb.pos.x) - margin_right - margin_left : -1, (vec2s){ -1, -1 }, false, false, -1, -1);
  GFX_STATE.pos_ptr.x += text_props.width + margin_right + padding;
  GFX_STATE.pos_ptr.y -= margin_top;
}

void gfx_set_text_wrap(bool wrap) {
  GFX_STATE.text_wrap = wrap;
}

gfx_div gfx_get_current_div() {
  return GFX_STATE.current_div;
}

gfx_div gfx_get_selected_div() {
  return GFX_STATE.selected_div;
}

gfx_div *gfx_get_current_div_ptr() {
  return &GFX_STATE.current_div;
}

gfx_div *gfx_get_selected_div_ptr() {
  return &GFX_STATE.selected_div;
}

void gfx_set_ptr_x(float x) {
  GFX_STATE.pos_ptr.x = x + GFX_STATE.current_div.aabb.pos.x;
}

void gfx_set_ptr_y(float y) {
  GFX_STATE.pos_ptr.y = y + GFX_STATE.current_div.aabb.pos.y;
}
void gfx_set_ptr_x_absolute(float x) {
  GFX_STATE.pos_ptr.x = x;
}

void gfx_set_ptr_y_absolute(float y) {
  GFX_STATE.pos_ptr.y = y;
}

float gfx_get_ptr_x() {
  return GFX_STATE.pos_ptr.x;
}

float gfx_get_ptr_y() {
  return GFX_STATE.pos_ptr.y;
}

unsigned int gfx_get_display_width() {
  return GFX_STATE.dsp_w;
}

unsigned int gfx_get_display_height() {
  return GFX_STATE.dsp_h;
}

void gfx_push_font(gfx_font *font) {
  GFX_STATE.font_stack = font;
}

void gfx_pop_font() {
  GFX_STATE.font_stack = NULL;
}

gfx_text_props gfx_text_render(vec2s pos, const char *str, gfx_font font, gfx_color color, int wrap_point, vec2s stop_point, bool no_render, bool render_solid, int start_index, int end_index) {
  gfx_text_props textprops = gfx_text_render_char(pos, str, font, color, wrap_point, stop_point, no_render, render_solid, start_index, end_index);
  return textprops;
}

void gfx_rect_render(vec2s pos, vec2s size, gfx_color color, gfx_color border_color, float border_width, float corner_radius, float rotation_angle) {
  if (!GFX_STATE.renderer_render) return;
  float boundingWidth, boundingHeight;
  compute_bounding_box(size.x, size.y, rotation_angle, &boundingWidth, &boundingHeight);
  if (gfx_internal_item_should_cull((gfx_aabb){ .pos = pos, .size = (vec2s){ boundingWidth, boundingHeight } }, true)) {
    return;
  }
  vec2s pos_initial = pos;
  vec2s rect_centered = (vec2s){ size.x / 2.0f, size.y / 2.0f };
  vec2s pos_centered = (vec2s){ pos.x + rect_centered.x, pos.y + rect_centered.y };
  vec2s texcoords[4] = { (vec2s){ 1.0f, 1.0f }, (vec2s){ 1.0f, 0.0f }, (vec2s){ 0.0f, 0.0f }, (vec2s){ 0.0f, 1.0f } };
  mat4 translate = GLM_MAT4_IDENTITY_INIT;
  mat4 scale = GLM_MAT4_IDENTITY_INIT;
  mat4 rotation = GLM_MAT4_IDENTITY_INIT;
  mat4 transform = GLM_MAT4_IDENTITY_INIT;
  vec3s pos_xyz = (vec3s){ pos_centered.x, pos_centered.y, 0.0f };
  glm_translate_make(translate, pos_xyz.raw);
  glm_scale_make(scale, (vec3){ size.x, size.y, 1.0f });
  glm_rotate_make(rotation, rotation_angle, (vec3){ 0.0f, 0.0f, 1.0f });
  glm_mat4_mul(rotation, scale, transform);
  glm_mat4_mul(translate, transform, transform);
  for (unsigned int i = 0; i < 4; i++) {
    if (GFX_STATE.render.vert_count >= GFX_MAX_RENDER_BATCH) {
      gfx_internal_renderer_flush();
      gfx_internal_renderer_begin();
    }
    vec4 result;
    glm_mat4_mulv(transform, (vec4s){ GFX_STATE.render.vert_pos[i].x, GFX_STATE.render.vert_pos[i].y, 0.0f, 1.0f }.raw, result);
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].pos, result, sizeof(vec2));
    vec4s border_color_zto = gfx_color_to_zto(border_color);
    const vec4 border_color_arr = { border_color_zto.r, border_color_zto.g, border_color_zto.b, border_color_zto.a };
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].border_color, border_color_arr, sizeof(vec4));
    GFX_STATE.render.verts[GFX_STATE.render.vert_count].border_width = border_width;
    vec4s color_zto = gfx_color_to_zto(color);
    const vec4 color_arr = { color_zto.r, color_zto.g, color_zto.b, color_zto.a };
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].color, color_arr, sizeof(vec4));
    const vec2 texcoord_arr = { texcoords[i].x, texcoords[i].y };
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].texcoord, texcoord_arr, sizeof(vec2));
    GFX_STATE.render.verts[GFX_STATE.render.vert_count].tex_index = -1;
    const vec2 scale_arr = { size.x, size.y };
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].scale, scale_arr, sizeof(vec2));
    const vec2 pos_px_arr = { (float)pos_initial.x, (float)pos_initial.y };
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].pos_px, pos_px_arr, sizeof(vec2));
    GFX_STATE.render.verts[GFX_STATE.render.vert_count].corner_radius = corner_radius;
    const vec2 cull_start_arr = { GFX_STATE.cull_start.x, GFX_STATE.cull_start.y };
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].min_coord, cull_start_arr, sizeof(vec2));
    const vec2 cull_end_arr = { GFX_STATE.cull_end.x, GFX_STATE.cull_end.y };
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].max_coord, cull_end_arr, sizeof(vec2));
    GFX_STATE.render.vert_count++;
  }
  GFX_STATE.render.index_count += 6;
}

void gfx_image_render(vec2s pos, gfx_color color, gfx_texture tex, gfx_color border_color, float border_width, float corner_radius, float rotation_angle, float cam_x, float cam_y, float cam_zoom, bool cullable, bool flipped_x) {
  if (!GFX_STATE.renderer_render) {
    return;
  }
  float window_center_x = gfx_get_display_width() / 2.0f + cam_x;
  float window_center_y = gfx_get_display_height() / 2.0f + cam_y;
  tex.width *= cam_zoom;
  tex.height *= cam_zoom;
  pos.x = (pos.x - window_center_x) * cam_zoom + window_center_x - cam_x;
  pos.y = (pos.y - window_center_y) * cam_zoom + window_center_y - cam_y;
  float bounding_width, bounding_height;
  compute_bounding_box(tex.width, tex.height, rotation_angle, &bounding_width, &bounding_height);
  if (gfx_internal_item_should_cull((gfx_aabb){ .pos = pos, .size = (vec2s){ bounding_width, bounding_height } }, cullable)) {
    return;
  }
  if (GFX_STATE.render.tex_count - 1 >= GFX_MAX_TEX_COUNT_BATCH - 1) {
    gfx_internal_renderer_flush();
    gfx_internal_renderer_begin();
  }
  vec2s pos_initial = pos;
  vec2s tex_centered = (vec2s){ tex.width / 2.0f, tex.height / 2.0f };
  vec2s pos_centered = (vec2s){ pos.x + tex_centered.x, pos.y + tex_centered.y };
  if (GFX_STATE.image_color_stack.a != 0.0) {
    color = GFX_STATE.image_color_stack;
  }
  /*vec2s texcoords[4] = { (vec2s){ 0.0f, 0.0f }, (vec2s){ 1.0f, 0.0f }, (vec2s){ 1.0f, 1.0f }, (vec2s){ 0.0f, 1.0f } };*/
  vec2s texcoords[4] = {
    flipped_x ? (vec2s){ 1.0f, 0.0f } : (vec2s){ 0.0f, 0.0f },
    flipped_x ? (vec2s){ 0.0f, 0.0f } : (vec2s){ 1.0f, 0.0f },
    flipped_x ? (vec2s){ 0.0f, 1.0f } : (vec2s){ 1.0f, 1.0f },
    flipped_x ? (vec2s){ 1.0f, 1.0f } : (vec2s){ 0.0f, 1.0f }
  };
  float tex_index = -1.0f;
  for (uint32_t i = 0; i < GFX_STATE.render.tex_count; i++) {
    if (tex.id == GFX_STATE.render.textures[i].id) {
      tex_index = i;
      break;
    }
  }
  if (tex_index == -1.0f) {
    tex_index = (float)GFX_STATE.render.tex_index;
    GFX_STATE.render.textures[GFX_STATE.render.tex_count++] = tex;
    GFX_STATE.render.tex_index++;
  }
  mat4 translate = GLM_MAT4_IDENTITY_INIT;
  mat4 scale = GLM_MAT4_IDENTITY_INIT;
  mat4 rotation = GLM_MAT4_IDENTITY_INIT;
  mat4 transform = GLM_MAT4_IDENTITY_INIT;
  vec3s pos_xyz = (vec3s){ pos_centered.x, pos_centered.y, 0.0f };
  vec3 tex_size = { tex.width, tex.height, 1.0f };
  glm_translate_make(translate, pos_xyz.raw);
  glm_scale_make(scale, tex_size);
  glm_rotate_make(rotation, rotation_angle, (vec3){ 0.0f, 0.0f, 1.0f });
  glm_mat4_mul(rotation, scale, transform);
  glm_mat4_mul(translate, transform, transform);
  for (unsigned int i = 0; i < 4; i++) {
    if (GFX_STATE.render.vert_count >= GFX_MAX_RENDER_BATCH) {
      gfx_internal_renderer_flush();
      gfx_internal_renderer_begin();
    }
    vec4 result;
    glm_mat4_mulv(transform, GFX_STATE.render.vert_pos[i].raw, result);
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].pos, result, sizeof(vec2));
    vec4s border_color_zto = gfx_color_to_zto(border_color);
    const vec4 border_color_arr = { border_color_zto.r, border_color_zto.g, border_color_zto.b, border_color_zto.a };
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].border_color, border_color_arr, sizeof(vec4));
    GFX_STATE.render.verts[GFX_STATE.render.vert_count].border_width = border_width;
    vec4s color_zto = gfx_color_to_zto(color);
    const vec4 color_arr = { color_zto.r, color_zto.g, color_zto.b, color_zto.a };
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].color, color_arr, sizeof(vec4));
    const vec2 texcoord_arr = { texcoords[i].x, texcoords[i].y };
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].texcoord, texcoord_arr, sizeof(vec2));
    GFX_STATE.render.verts[GFX_STATE.render.vert_count].tex_index = tex_index;
    const vec2 scale_arr = { (float)tex.width, (float)tex.height };
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].scale, scale_arr, sizeof(vec2));
    vec2 pos_px_arr = { (float)pos_initial.x, (float)pos_initial.y };
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].pos_px, pos_px_arr, sizeof(vec2));
    GFX_STATE.render.verts[GFX_STATE.render.vert_count].corner_radius = corner_radius;
    const vec2 cull_start_arr = { GFX_STATE.cull_start.x, GFX_STATE.cull_start.y };
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].min_coord, cull_start_arr, sizeof(vec2));
    const vec2 cull_end_arr = { GFX_STATE.cull_end.x, GFX_STATE.cull_end.y };
    memcpy(GFX_STATE.render.verts[GFX_STATE.render.vert_count].max_coord, cull_end_arr, sizeof(vec2));
    GFX_STATE.render.vert_count++;
  }
  GFX_STATE.render.index_count += 6;
}

bool gfx_point_intersects_aabb(vec2s p, gfx_aabb aabb) {
  vec2s min = { aabb.pos.x, aabb.pos.y };
  vec2s max = { aabb.pos.x + aabb.size.x, aabb.pos.y + aabb.size.y };
  if (p.x >= min.x && p.x <= max.x &&
    p.y >= min.y && p.y <= max.y) {
    return true;
  }
  return false;
}

bool gfx_aabb_intersects_aabb(gfx_aabb a, gfx_aabb b) {
  vec2s minA = a.pos;
  vec2s maxA = (vec2s){ a.pos.x + a.size.x, a.pos.y + a.size.y };
  vec2s minB = b.pos;
  vec2s maxB = (vec2s){ b.pos.x + b.size.x, b.pos.y + b.size.y };
  return (minA.x >= minB.x && minA.x <= maxB.x && 
  minA.y >= minB.y && minA.y <= maxB.y);
}

void gfx_push_style_props(gfx_element_props props) {
  gfx_internal_props_stack_push(&GFX_STATE.props_stack, props);
}

void gfx_pop_style_props() {
  gfx_internal_props_stack_pop(&GFX_STATE.props_stack);
}

void gfx_clear_style_props() {
  gfx_internal_props_stack_clear(&GFX_STATE.props_stack);
}

bool gfx_hovered(vec2s pos, vec2s size) {
  bool hovered = gfx_get_mouse_x() <= (pos.x + size.x) && gfx_get_mouse_x() >= (pos.x) && gfx_get_mouse_y() <= (pos.y + size.y) && gfx_get_mouse_y() >= (pos.y) && ((GFX_STATE.selected_div.id == GFX_STATE.current_div.id && GFX_STATE.grabbed_div.id == -1) || (GFX_STATE.grabbed_div.id == GFX_STATE.current_div.id && GFX_STATE.grabbed_div.id != -1));
  return hovered;
}

bool gfx_area_hovered(vec2s pos, vec2s size) {
  bool hovered = gfx_get_mouse_x() <= (pos.x + size.x) && gfx_get_mouse_x() >= (pos.x) && gfx_get_mouse_y() <= (pos.y + size.y) && gfx_get_mouse_y() >= (pos.y);
  return hovered;
}

void gfx_set_image_color(gfx_color color) {
  GFX_STATE.image_color_stack = color;
}

void gfx_unset_image_color() {
  GFX_STATE.image_color_stack = GFX_NO_COLOR;
}

void gfx_set_current_div_scroll(float scroll) {
  *GFX_STATE.scroll_ptr = scroll;
}

float gfx_get_current_div_scroll() {
  return *GFX_STATE.scroll_ptr;
}

void gfx_set_current_div_scroll_velocity(float scroll_velocity) {
  *GFX_STATE.scroll_velocity_ptr = scroll_velocity;
}

float gfx_get_current_div_scroll_velocity() {
  return *GFX_STATE.scroll_ptr;
}

void gfx_set_line_height(unsigned int line_height) {
  GFX_STATE.current_line_height = line_height;
}

unsigned int gfx_get_line_height() {
  return GFX_STATE.current_line_height;
}

void gfx_set_line_should_overflow(bool overflow) {
  GFX_STATE.line_overflow = overflow;
}

void gfx_set_div_hoverable(bool clickable) {
  GFX_STATE.div_hoverable = clickable;
}

void gfx_push_element_id(long id) {
  GFX_STATE.element_id_stack = id;
}

void gfx_pop_element_id() {
  GFX_STATE.element_id_stack = -1;
}

gfx_color gfx_color_brightness(gfx_color color, float brightness) {
  unsigned int adjustedR = (int)(color.r * brightness);
  unsigned int adjustedG = (int)(color.g * brightness);
  unsigned int adjustedB = (int)(color.b * brightness);
  color.r = (unsigned char)(adjustedR > 255 ? 255 : adjustedR);
  color.g = (unsigned char)(adjustedG > 255 ? 255 : adjustedG);
  color.b = (unsigned char)(adjustedB > 255 ? 255 : adjustedB);
  return color; 
}

gfx_color gfx_color_alpha(gfx_color color, unsigned char a) {
  return (gfx_color){ color.r, color.g, color.b, a };
}

vec4s gfx_color_to_zto(gfx_color color) {
  return (vec4s){ color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
}

gfx_color gfx_color_from_hex(unsigned int hex) {
  gfx_color color;
  color.r = (hex>> 16) & 0xFF;
  color.g = (hex >> 8) & 0xFF; 
  color.b = hex& 0xFF; 
  color.a = 255; 
  return color;
}

gfx_color gfx_color_from_zto(vec4s zto) {
  return (gfx_color){ (unsigned char)(zto.r * 255.0f), (unsigned char)(zto.g * 255.0f), (unsigned char)(zto.b * 255.0f), (unsigned char)(zto.a * 255.0f) };
}

void gfx_image(gfx_texture tex) {
  float w, h;
  compute_bounding_box(tex.width, tex.height, tex.angle, &w, &h);
  gfx_element_props props = gfx_internal_get_props_for(GFX_STATE.theme.image_props);
  float margin_left = props.margin_left, margin_right = props.margin_right, margin_top = props.margin_top, margin_bottom = props.margin_bottom;
  gfx_color color = props.color;
  gfx_internal_next_line_on_overflow((vec2s){ w + margin_left + margin_right, h + margin_top + margin_bottom }, GFX_STATE.div_props.border_width);
  GFX_STATE.pos_ptr.x += margin_left; 
  GFX_STATE.pos_ptr.y += margin_top;
  gfx_image_render(GFX_STATE.pos_ptr, color, tex, props.border_color, props.border_width, props.corner_radius, tex.angle, 0.0f, 0.0f, 1.0f, true, false);
  GFX_STATE.pos_ptr.x += w + margin_right;
  GFX_STATE.pos_ptr.y -= margin_top;
}

void gfx_image_no_block(float x, float y, gfx_texture tex, float cam_x, float cam_y, float cam_zoom, bool cullable, bool flipped_x) {
  float init_x = GFX_STATE.pos_ptr.x, init_y = GFX_STATE.pos_ptr.y;
  float w, h;
  compute_bounding_box(tex.width, tex.height, tex.angle, &w, &h);
  gfx_element_props props = gfx_internal_get_props_for(GFX_STATE.theme.image_props);
  float margin_left = props.margin_left, margin_right = props.margin_right, margin_top = props.margin_top, margin_bottom = props.margin_bottom;
  gfx_color color = props.color;
  gfx_internal_next_line_on_overflow((vec2s){ w + margin_left + margin_right, h + margin_top + margin_bottom }, GFX_STATE.div_props.border_width);
  GFX_STATE.pos_ptr.x += margin_left; 
  GFX_STATE.pos_ptr.y += margin_top;
  gfx_image_render((vec2s){ x - tex.width / 2.0f, y - tex.height / 2.0f}, color, tex, props.border_color, props.border_width, props.corner_radius, tex.angle, cam_x, cam_y, cam_zoom, cullable, flipped_x);
  GFX_STATE.pos_ptr.x += w + margin_right;
  GFX_STATE.pos_ptr.y -= margin_top;
  GFX_STATE.pos_ptr.x = init_x;
  GFX_STATE.pos_ptr.y = init_y;
}

void gfx_rect(float width, float height, gfx_color color, float corner_radius, float angle) {
  float w, h;
  compute_bounding_box(width, height, angle, &w, &h);
  gfx_internal_next_line_on_overflow((vec2s){ w, h }, GFX_STATE.div_props.border_width);
  gfx_rect_render(GFX_STATE.pos_ptr, (vec2s){ (float)width, (float)height }, color, (gfx_color){ 0.0f, 0.0f, 0.0f, 0.0f }, 0, corner_radius, angle);
  GFX_STATE.pos_ptr.x += w;
}

void gfx_rect_no_block(float x, float y, float width, float height, gfx_color color, float corner_radius, float angle) {
  float init_x = GFX_STATE.pos_ptr.x, init_y = GFX_STATE.pos_ptr.y;
  float w, h;
  compute_bounding_box(width, height, angle, &w, &h);
  gfx_internal_next_line_on_overflow((vec2s){ w, h }, GFX_STATE.div_props.border_width);
  gfx_rect_render((vec2s){ x - width / 2.0f, y - height / 2.0f}, (vec2s){ (float)width, (float)height }, color, (gfx_color){ 0.0f, 0.0f, 0.0f, 0.0f }, 0, corner_radius, angle);
  GFX_STATE.pos_ptr.x += w;
  GFX_STATE.pos_ptr.x = init_x;
  GFX_STATE.pos_ptr.y = init_y;
}

void gfx_seperator() {
  gfx_next_line();
  gfx_element_props props = gfx_internal_get_props_for(GFX_STATE.theme.button_props);
  GFX_STATE.pos_ptr.x += props.margin_left;
  GFX_STATE.pos_ptr.y += props.margin_top;
  const unsigned int seperator_height = 1;
  gfx_set_line_height(props.margin_top + seperator_height + props.margin_bottom);
  gfx_rect_render(GFX_STATE.pos_ptr, (vec2s){ GFX_STATE.current_div.aabb.size.x - props.margin_left * 2.0f, seperator_height }, props.color, GFX_NO_COLOR, 0, props.corner_radius, 0.0f);
  GFX_STATE.pos_ptr.y -= props.margin_top;
  gfx_next_line();
}

void gfx_set_clipboard_text(const char *text) {
  clipboard_set_text(GFX_STATE.clipboard, text);
}

char *gfx_get_clipboard_text() {
  return clipboard_text(GFX_STATE.clipboard);
}

void gfx_set_no_render(bool no_render) {
  GFX_STATE.renderer_render = !no_render;
}
