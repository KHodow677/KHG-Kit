#include "khg_ui/elements.h"
#include "khg_ui/texture.h"
#include "khg_ui/internal.h"

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
