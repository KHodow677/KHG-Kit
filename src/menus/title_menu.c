#include "menus/title_menu.h"
#include "game_manager.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/ui.h"
#include "scenes/scene_utl.h"

static void render_div(float div_width, float div_height, gfx_element_props div_props) {
  gfx_push_style_props(div_props);
  gfx_div_begin(((vec2s){(gfx_get_display_width() - div_width) / 2.0f, (gfx_get_display_height() - div_height) / 2.0f}), ((vec2s){ div_width, div_height }), false);
}

static void render_text(float div_width, float div_height, gfx_element_props btn_props, gfx_element_props text_props, const char *text, const char *btn_text) {
  float text_height = gfx_text_dimension(text).y;
  gfx_push_font(&MEDIUM_FONT);
  float btn_text_height = gfx_text_dimension(btn_text).y;
  gfx_pop_font();
  float total_height = text_height + btn_text_height * 2 + btn_props.padding * 4 + btn_props.margin_top * 2 + btn_props.border_width * 4;
  gfx_push_font(&LARGE_FONT);
  gfx_set_ptr_x((div_width - gfx_text_dimension(text).x) / 2.0f);
  gfx_set_ptr_y((div_height - total_height) / 2.0f);
  gfx_push_style_props(text_props);
  gfx_text(text);
  gfx_pop_style_props();
  gfx_pop_font();
}

static bool render_btn(float div_width, float div_height, gfx_element_props btn_props, const char *btn_text) {
  const float exit_width = 180.0f;
  gfx_push_style_props(btn_props);
  gfx_set_ptr_x((div_width - (exit_width + btn_props.padding * 2.0f)) / 2.0f);
  gfx_push_font(&MEDIUM_FONT);
  if(gfx_button_fixed(btn_text, exit_width, -1) == gfx_clickable_clicked) {
    return false;
  }
  gfx_pop_style_props();
  gfx_pop_font();
  return true;
}

bool render_title_menu() {
  float width = 400.0f;
  float height = 400.0f;
  gfx_element_props btn_props = gfx_get_theme().button_props;
  btn_props.margin_top = 10.0f;
  btn_props.margin_left = 0.0f;
  btn_props.border_width = 4.0f;
  btn_props.text_color = gfx_black;
  btn_props.border_color = gfx_black;
  btn_props.color = (gfx_color){ 90, 90, 90, 255 };
  btn_props.padding = 10.0f;
  gfx_element_props div_props = gfx_get_theme().div_props;
  div_props.corner_radius = 0.0f;
  div_props.border_width = 4.0f;
  div_props.border_color = gfx_black;
  gfx_element_props text_props = gfx_get_theme().text_props;
  text_props.margin_left = 0.0f; 
  text_props.margin_right = 0.0f;
  text_props.text_color = gfx_black;
  render_div(width, height, div_props);
  render_text(width, height, btn_props, text_props, "Game Title!", "Start");
  gfx_next_line();
  if (!render_btn(width, height, btn_props, "Start")) {
    stm_handle_event(&SCENE_FSM, &(stm_event){ EVENT_SCENE_SWITCH, (void *)(intptr_t)TO_TUTORIAL_SCENE });
  }
  gfx_next_line();
  if (!render_btn(width, height, btn_props, "Exit")) {
    return false;
  }
  gfx_div_end();
  gfx_pop_style_props();
  return true;
}

