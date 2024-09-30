#include "menus/title_menu.h"
#include "game_manager.h"
#include "khg_gfx/elements.h"
#include "menus/menu_utl.h"
#include "scenes/scene_utl.h"

bool render_title_menu() {
  float width = 400.0f;
  float height = 400.0f;
  render_div((gfx_get_display_width() - width) / 2.0f, (gfx_get_display_height() - height) / 2.0f, width, height, 0.0f);
  render_text(LARGE_FONT, width / 2, 60.0f, "It Tanks Two!");
  gfx_next_line();
  if (!render_button(width, height, "Map 1", 140.0f)) {
    stm_handle_event(&SCENE_FSM, &(stm_event){ EVENT_SCENE_SWITCH, (void *)(intptr_t)TO_TUTORIAL_SCENE });
  }
  if (!render_button(width, height, "Map 2", 220.0f)) {
    stm_handle_event(&SCENE_FSM, &(stm_event){ EVENT_SCENE_SWITCH, (void *)(intptr_t)TO_GAME_SCENE });
  }
  gfx_next_line();
  if (!render_button(width, height, "Exit", 300.0f)) {
    return false;
  }
  gfx_div_end();
  gfx_pop_style_props();
  return true;
}

