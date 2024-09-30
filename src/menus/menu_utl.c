#include "menus/menu_utl.h"
#include "game_manager.h"
#include "generators/components/texture_generator.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/ui.h"
#include <stdio.h>

static void change_selected_button_border_color(gfx_color color) {
  gfx_element_props props = gfx_get_theme().button_props;
  props.margin_top = 10.0f;
  props.margin_left = 0.0f;
  props.border_width = 4.0f;
  props.text_color = gfx_black;
  props.border_color = color;
  props.color = (gfx_color){ 90, 90, 90, 255 };
  props.padding = 10.0f;
  gfx_push_style_props(props);
}

void render_div(float pos_x, float pos_y, float div_width, float div_height, float padding) {
  gfx_element_props div_props = gfx_get_theme().div_props;
  div_props.corner_radius = 0.0f;
  div_props.border_width = 4.0f;
  div_props.border_color = gfx_black;
  gfx_push_style_props(div_props);
  gfx_div_begin(((vec2s){ pos_x, pos_y }), ((vec2s){ div_width, div_height }), false);
}

void render_header_text(float div_width, float div_height, const char *text, float padding) {
  gfx_element_props text_props = gfx_get_theme().text_props;
  text_props.margin_left = 0.0f; 
  text_props.margin_right = 0.0f;
  text_props.text_color = gfx_black;
  gfx_push_font(&LARGE_FONT);
  gfx_set_ptr_x((div_width - gfx_text_dimension(text).x) / 2.0f);
  gfx_set_ptr_y(padding);
  gfx_push_style_props(text_props);
  gfx_text(text);
  gfx_pop_style_props();
  gfx_pop_font();
}

void render_body_text(float div_width, float div_height, const char *text, float padding) {
  gfx_element_props text_props = gfx_get_theme().text_props;
  text_props.margin_left = 0.0f; 
  text_props.margin_right = 0.0f;
  text_props.text_color = gfx_black;
  gfx_push_font(&MEDIUM_FONT);
  gfx_set_ptr_x((div_width - gfx_text_dimension(text).x) / 2.0f);
  gfx_set_ptr_y(padding);
  gfx_push_style_props(text_props);
  gfx_text(text);
  gfx_pop_style_props();
  gfx_pop_font();
}

void render_text(gfx_font font, float center_x, float center_y, const char *text) {
  gfx_element_props text_props = gfx_get_theme().text_props;
  text_props.margin_left = 0.0f; 
  text_props.margin_right = 0.0f;
  text_props.text_color = gfx_black;
  gfx_push_font(&font);
  gfx_set_ptr_x(center_x - gfx_text_dimension(text).x / 2.0f);
  gfx_set_ptr_y(center_y - gfx_text_dimension(text).y / 2.0f);
  gfx_push_style_props(text_props);
  gfx_text(text);
  gfx_pop_style_props();
  gfx_pop_font();
}

bool render_button(float div_width, float div_height, const char *btn_text, float padding) {
  gfx_element_props btn_props = gfx_get_theme().button_props;
  btn_props.margin_top = 0.0f;
  btn_props.margin_left = 0.0f;
  btn_props.border_width = 4.0f;
  btn_props.text_color = gfx_black;
  btn_props.border_color = gfx_black;
  btn_props.color = (gfx_color){ 90, 90, 90, 255 };
  btn_props.padding = 10.0f;
  const float btn_width = 180.0f;
  gfx_push_style_props(btn_props);
  gfx_set_ptr_x((div_width - (btn_width + btn_props.padding * 2.0f)) / 2.0f);
  gfx_set_ptr_y(padding);
  gfx_push_font(&MEDIUM_FONT);
  if(gfx_button_fixed(btn_text, btn_width, -1) == gfx_clickable_clicked) {
    return false;
  }
  gfx_pop_style_props();
  gfx_pop_font();
  return true;
}

void render_big_icon(float div_width, float div_height, int tex_id, float padding) {
  gfx_element_props div_props = gfx_get_theme().div_props;
  div_props.corner_radius = 10.0f;
  div_props.border_width = 4.0f;
  div_props.border_color = gfx_black;
  gfx_element_props img_props = gfx_get_theme().button_props;
  img_props.margin_top = 10.0f;
  img_props.margin_left = 0.0f;
  img_props.border_width = 0.0f;
  img_props.border_color = gfx_black;
  img_props.color = gfx_white;
  img_props.padding = 10.0f;
  gfx_pop_style_props();
  gfx_push_style_props(img_props);
  gfx_texture *tex = get_or_add_texture(tex_id);
  gfx_set_ptr_x((div_width - tex->width) / 2.0f);
  gfx_set_ptr_y(padding);
  gfx_image(*tex);
  gfx_pop_style_props();
  gfx_push_style_props(div_props);
}

void render_small_icon_buttons(float div_width, float div_height, float padding, float h_padding) {
  int *current_index = &SPAWN_SETTINGS.current_spawn_index;
  gfx_element_props div_props = gfx_get_theme().div_props;
  div_props.corner_radius = 0.0f;
  div_props.border_width = 4.0f;
  div_props.border_color = gfx_black;
  gfx_element_props btn_props = gfx_get_theme().button_props;
  btn_props.margin_top = 10.0f;
  btn_props.margin_left = 0.0f;
  btn_props.border_width = 4.0f;
  btn_props.text_color = gfx_black;
  btn_props.border_color = gfx_black;
  btn_props.color = (gfx_color){ 90, 90, 90, 255 };
  btn_props.padding = 10.0f;
  gfx_pop_style_props();
  gfx_push_style_props(btn_props);
  for (int i = 0; i < MAX_SELECTIONS; i++) {
    bool switched = false;
    if (*current_index == i) {
      change_selected_button_border_color(gfx_white);
      switched = true;
    }
    int tex_id = SPAWN_SELECTIONS[i].icon_tex_id;
    gfx_texture *tex = get_or_add_texture(tex_id);
    float item_length = tex->width + btn_props.padding * 2.0f;
    float total_length = (MAX_SELECTIONS) * item_length + (MAX_SELECTIONS - 1) * h_padding;
    float partial_length = total_length - i * 2 * item_length - i * 2 * h_padding;
    gfx_set_ptr_x((div_width - partial_length) / 2.0f);
    gfx_set_ptr_y(padding);
    if (gfx_image_button(*tex) == gfx_clickable_clicked) {
      *current_index = i;
    }
    if (switched) {
      gfx_pop_style_props();
    }
    char cost[10]; 
    snprintf(cost, sizeof(cost), "$%d", SPAWN_SELECTIONS[i].cost);
    render_text(MEDIUM_FONT, (div_width - partial_length) / 2.0f + item_length / 2.0f, padding + item_length + 20.0f, cost);
  }
  gfx_pop_style_props();
  gfx_push_style_props(div_props);
}
