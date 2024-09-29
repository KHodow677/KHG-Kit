#include "menus/spawn_menu.h"
#include "entity/entity.h"
#include "game_manager.h"
#include "generators/components/texture_generator.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/ui.h"
#include "khg_phy/body.h"
#include "spawners/spawn_tank.h"
#include <stdint.h>

spawn_menu_info SPAWN_SETTINGS = { 0 };

static int MAX_TOPS = 3;
static int MAX_BODIES = 3;

static int TEST_VAL = 200;

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

static void render_div(float div_width, float div_height, float padding) {
  gfx_element_props div_props = gfx_get_theme().div_props;
  div_props.corner_radius = 0.0f;
  div_props.border_width = 4.0f;
  div_props.border_color = gfx_black;
  gfx_push_style_props(div_props);
  gfx_div_begin(((vec2s){ padding, gfx_get_display_height() - div_height - padding }), ((vec2s){ div_width, div_height }), false);
}

static void render_header_text(float div_width, float div_height, const char *text, float padding) {
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

static void render_body_text(float div_width, float div_height, const char *text, float padding) {
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

static bool render_button(float div_width, float div_height, const char *btn_text, float padding) {
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

static void render_big_icon(float div_width, float div_height, int tex_id, float padding) {
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

static void render_small_icon_buttons(float div_width, float div_height, bool select_type, float padding, float h_padding) {
  int max = select_type == SPAWN_SELECT_TOP ? MAX_TOPS : MAX_BODIES;
  int tex_id = select_type == SPAWN_SELECT_TOP ? TANK_TOP_ICON_SMALL : TANK_BODY_ICON_SMALL;
  int *current_index = select_type == SPAWN_SELECT_TOP ? &SPAWN_SETTINGS.current_spawn_top_index : &SPAWN_SETTINGS.current_spawn_body_index;
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
  for (int i = 0; i < max; i++) {
    bool switched = false;
    if (*current_index == i) {
      change_selected_button_border_color(gfx_white);
      switched = true;
    }
    gfx_texture *tex = get_or_add_texture(tex_id);
    float total_length = (max) * (tex->width + btn_props.padding * 2.0f) + (max - 1) * h_padding;
    float partial_length = total_length - i * 2 * (tex->width + btn_props.padding * 2.0f) - i * 2 * h_padding;
    gfx_set_ptr_x((div_width - partial_length) / 2.0f);
    gfx_set_ptr_y(padding);
    if (gfx_image_button(*tex) == gfx_clickable_clicked) {
      *current_index = i;
    }
    if (switched) {
      gfx_pop_style_props();
    }
  }
  gfx_pop_style_props();
  gfx_push_style_props(div_props);
}

void spawn_menu_setup() {
  GAME_OVERLAY_TRACKER[SPAWN_MENU].width = 400.0f;
  GAME_OVERLAY_TRACKER[SPAWN_MENU].height = 575.0f;
  GAME_OVERLAY_TRACKER[SPAWN_MENU].padding = 20.0f;
  GAME_OVERLAY_TRACKER[SPAWN_MENU].pos_x = GAME_OVERLAY_TRACKER[SPAWN_MENU].padding; 
  GAME_OVERLAY_TRACKER[SPAWN_MENU].pos_y = gfx_get_display_height() - GAME_OVERLAY_TRACKER[SPAWN_MENU].height - GAME_OVERLAY_TRACKER[SPAWN_MENU].padding;
}

bool render_spawn_menu() {
  float width = 400.0f;
  float height = 575.0f;
  render_div(width, height, 20.0f);
  render_header_text(width, height, "Hangar", 20.0f);
  render_big_icon(width, height, HANGAR_ICON_BIG, 70.0f);
  render_body_text(width, height, "Select Turret", 220.0f);
  render_small_icon_buttons(width, height, SPAWN_SELECT_TOP, 250.0f, 20.0f);
  gfx_element_props props = gfx_get_theme().slider_props;
  props.margin_top = 0;
  props.margin_left = 0;
  props.margin_right = 0;
  props.border_width = 4.0f;
  props.color = gfx_green;
  props.text_color = gfx_white;
  props.border_color = gfx_black;
  gfx_push_style_props(props);
  gfx_set_ptr_x((width - 200) / 2.0f);
  gfx_set_ptr_y(400);
  gfx_clickable_item_state progress = gfx_progress_bar_int(TEST_VAL, 0, 500, 200, 50);
  gfx_pop_style_props();
  /*render_body_text(width, height, "Select Hull", 350.0f);*/
  /*render_small_icon_buttons(width, height, SPAWN_SELECT_BODY, 380.0f, 20.0f);*/
  if (!render_button(width, height, "SPAWN", 500.0f)) {
    phy_vect pos = phy_body_get_position(SPAWN_SETTINGS.comp_physics->body);
    float ang = phy_body_get_angle(SPAWN_SETTINGS.comp_physics->body);
    generic_entity *ge = spawn_tank(pos.x, pos.y, ang);
    SPAWN_SETTINGS.comp_selector->should_deselect = true;
  }
  gfx_div_end();
  gfx_pop_style_props();
  return true;
}

