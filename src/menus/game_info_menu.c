#include "menus/game_info_menu.h"
#include "generators/components/texture_generator.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/ui.h"
#include "menus/game_menu_manager.h"
#include "menus/menu_utl.h"
#include "game_manager.h"
#include <stdio.h>

void game_info_menu_setup() {
  GAME_OVERLAY_TRACKER[GAME_INFO_MENU].active = true;
  GAME_OVERLAY_TRACKER[GAME_INFO_MENU].width = 250.0f;
  GAME_OVERLAY_TRACKER[GAME_INFO_MENU].height = 200.0f;
  GAME_OVERLAY_TRACKER[GAME_INFO_MENU].padding = 20.0f;
  GAME_OVERLAY_TRACKER[GAME_INFO_MENU].pos_x = gfx_get_display_width() - GAME_OVERLAY_TRACKER[GAME_INFO_MENU].width - GAME_OVERLAY_TRACKER[GAME_INFO_MENU].padding; 
  GAME_OVERLAY_TRACKER[GAME_INFO_MENU].pos_y = GAME_OVERLAY_TRACKER[GAME_INFO_MENU].padding;
}

bool render_game_info_menu() {
  float pos_x = GAME_OVERLAY_TRACKER[GAME_INFO_MENU].pos_x;
  float pos_y = GAME_OVERLAY_TRACKER[GAME_INFO_MENU].pos_y;
  float width = GAME_OVERLAY_TRACKER[GAME_INFO_MENU].width;
  float height = GAME_OVERLAY_TRACKER[GAME_INFO_MENU].height;
  render_div(pos_x, pos_y, width, height, 20.0f);
  gfx_element_props img_props = gfx_get_theme().image_props;
  img_props.margin_top = 0.0f;
  img_props.margin_left = 0.0f;
  img_props.border_width = 0.0f;
  img_props.color = gfx_no_color;
  img_props.padding = 10.0f;
  gfx_push_style_props(img_props);
  gfx_texture *tex1 = get_or_add_texture(MONEY_ICON);
  gfx_set_ptr_x(20.0f);
  gfx_set_ptr_y(20.0f);
  gfx_image_button(*tex1);
  gfx_pop_style_props();
  char money[10]; 
  snprintf(money, sizeof(money), "%d", GAME_INFO.cash);
  render_text(LARGE_FONT, width - 83.0f, tex1->height / 2.0f + 23.0f, money);
  gfx_push_style_props(img_props);
  gfx_texture *tex2 = get_or_add_texture(LIFE_ICON);
  gfx_set_ptr_x(20.0f);
  gfx_set_ptr_y(104.0f);
  gfx_image_button(*tex2);
  gfx_pop_style_props();
  char lives[10]; 
  snprintf(lives, sizeof(lives), "%d", GAME_INFO.lives);
  render_text(LARGE_FONT, width - 83.0f, tex2->height / 2.0f + 107.0f, lives);
  gfx_div_end();
  gfx_clear_style_props();
  return true;
}

