#include "menus/spawn_menu.h"
#include "game_manager.h"
#include "generators/components/texture_generator.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/ui.h"
#include "scenes/scene_utl.h"
#include <stdint.h>

spawn_menu_info SPAWN_SETTINGS = { 0 };

static void render_div(float div_width, float div_height, gfx_element_props div_props, float padding) {
  gfx_push_style_props(div_props);
  gfx_div_begin(((vec2s){ padding, gfx_get_display_height() - div_height + padding }), ((vec2s){ div_width, div_height }), false);
}

static void render_text(float div_width, float div_height, gfx_element_props text_props, const char *text, float padding) {
  gfx_push_font(&MEDIUM_FONT);
  gfx_pop_font();
  gfx_push_font(&LARGE_FONT);
  gfx_set_ptr_x((div_width - gfx_text_dimension(text).x) / 2.0f);
  gfx_set_ptr_y(padding);
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

static void render_hangar_image(float div_width, float div_height, gfx_element_props div_props) {
  gfx_pop_style_props();
  gfx_texture *tex = get_or_add_texture(HANGAR_ICON);
  gfx_set_ptr_x((div_width - tex->width) / 2.0f);
  gfx_image(*tex);
  gfx_push_style_props(div_props);
}

bool render_spawn_menu() {
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
  render_div(width, height, div_props, 20.0f);
  render_text(width, height, text_props, "Hangar", 20.0f);
  gfx_next_line();
  render_hangar_image(width, height, div_props);
  gfx_next_line();
  if (!render_btn(width, height, btn_props, "SPAWN")) {
    stm_handle_event(&SCENE_FSM, &(stm_event){ EVENT_SCENE_SWITCH, (void *)(intptr_t)TO_SANDBOX_SCENE });
  }
  gfx_next_line();
  if (!render_btn(width, height, btn_props, "Exit")) {
    return false;
  }
  gfx_div_end();
  gfx_pop_style_props();
  return true;
}

