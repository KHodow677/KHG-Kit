#include "menus/title_menu.h"
#include "game_manager.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/ui.h"

static const char* text = "Game Title!";
static const char* btn_text = "Exit";

static void render_div(gfx_element_props div_props) {
  gfx_push_style_props(div_props);
  float width = 400.0f;
  float height = 400.0f;
  gfx_div_begin(((vec2s){(gfx_get_display_width() - width) / 2.0f, (gfx_get_display_height() - height) / 2.0f}), ((vec2s){width, height}), false);
  gfx_pop_style_props();
}

static void render_text(gfx_element_props btn_props, gfx_element_props text_props) {
  gfx_push_font(&LARGE_FONT);
  gfx_set_ptr_x_absolute((gfx_get_display_width() - gfx_text_dimension(text).x) / 2.0f);
  gfx_set_ptr_y_absolute((gfx_get_display_height() - (gfx_text_dimension(text).y + gfx_text_dimension(btn_text).y + btn_props.padding * 2.0f + btn_props.margin_top)) / 2.0f);
  gfx_push_style_props(text_props);
  gfx_text(text);
  gfx_pop_style_props();
  gfx_pop_font();
}

static bool render_btn(gfx_element_props btn_props) {
  const float exit_width = 180.0f;
  gfx_push_style_props(btn_props);
  gfx_set_ptr_x_absolute((gfx_get_display_width() - (exit_width + btn_props.padding * 2.0f)) / 2.0f);
  gfx_push_font(&MEDIUM_FONT);
  if(gfx_button_fixed(btn_text, exit_width, -1) == gfx_clickable_clicked) {
    return false;
  }
  gfx_pop_style_props();
  gfx_pop_font();
  return true;
}

bool render_title_menu() {
  gfx_element_props btn_props = gfx_get_theme().button_props;
  btn_props.margin_top = 10.0f;
  btn_props.margin_left = 0.0f;
  btn_props.border_width = 7.5f;
  btn_props.text_color = gfx_black;
  btn_props.border_color = gfx_black;
  btn_props.color = (gfx_color){ 90, 90, 90, 255 };
  btn_props.padding = 10.0f;
  gfx_element_props div_props = gfx_get_theme().div_props;
  div_props.corner_radius = 0.0f;
  div_props.border_width = 7.5f;
  div_props.border_color = gfx_black;
  gfx_element_props text_props = gfx_get_theme().text_props;
  text_props.margin_left = 0.0f; 
  text_props.margin_right = 0.0f;
  text_props.text_color = gfx_black;
  render_div(div_props);
  render_text(btn_props, text_props);
  gfx_next_line();
  if (!render_btn(btn_props)) {
    return false;
  }
  gfx_div_end();
  return true;
}

