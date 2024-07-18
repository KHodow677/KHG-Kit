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
