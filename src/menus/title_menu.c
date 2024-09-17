#include "menus/title_menu.h"
#include "game_manager.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/ui.h"

bool render_title_menu() {
  const char* text = "Game Title!";
  const char* btntext = "Exit";
  gfx_element_props btnprops = gfx_get_theme().button_props;
  btnprops.margin_left = 0.0f; 
  btnprops.margin_top = 15.0f;
  btnprops.border_width = 0.0f;
  btnprops.corner_radius = 9.0f;
  btnprops.text_color = gfx_white;
  btnprops.color = (gfx_color){90, 90, 90, 255}; 
  gfx_element_props props = gfx_get_theme().div_props;
  props.corner_radius = 10.0f;
  gfx_push_style_props(props);
  float width = 400.0f, height = 400.0f;
  gfx_div_begin(((vec2s){(gfx_get_display_width() - width) / 2.0f, (gfx_get_display_height() - height) / 2.0f}), ((vec2s){width, height}), false);
  gfx_pop_style_props();
  gfx_push_font(&LARGE_FONT);
  gfx_set_ptr_x_absolute((gfx_get_display_width() - gfx_text_dimension(text).x) / 2.0f);
  gfx_set_ptr_y_absolute((gfx_get_display_height() - (gfx_text_dimension(text).y + gfx_text_dimension(btntext).y + btnprops.padding * 2.0f + btnprops.margin_top)) / 2.0f);
  props = gfx_get_theme().text_props;
  props.margin_left = 0.0f; props.margin_right = 0.0f;
  gfx_push_style_props(props);
  gfx_text(text);
  gfx_pop_style_props();
  gfx_pop_font();
  gfx_next_line();
  const float exit_width = 180.0f;
  gfx_push_style_props(btnprops);
  gfx_set_ptr_x_absolute((gfx_get_display_width() - (exit_width + btnprops.padding * 2.0f)) / 2.0f);
  if(gfx_button_fixed(btntext, exit_width, -1) == gfx_clickable_clicked) {
    return false;
  }
  gfx_pop_style_props();
  gfx_div_end();
  return true;
}

