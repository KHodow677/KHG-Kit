#pragma once

#include "khg_2d/font.h"
#include "khg_2d/renderer_2d.h"
#include "khg_2d/texture.h"
#include "khg_ui/data.h"
#include "khg_ui/menu_stack_hash_table.h"
#include "khg_ui/widget_hash_table.h"

extern int x_padd;
extern int y_padd;
extern int width;
extern int height;

extern float timer;
extern int current_id;
extern bool id_was_set;

typedef struct {
  aligned_settings a_settings;
  vector(widget_pair) widget_vector;
  widget_hash_table widgets;
  menu_stack_hash_table all_menu_stacks;
  char **id_str;
  char **current_text_box;
} renderer_ui;

void render_frame(renderer_ui *rui, renderer_2d *r2d, font *f, vec2 mouse_pos, bool mouse_click, bool mouse_held, bool mouse_released, bool escape_released, const char **typed_input, float delta_time);
bool renderer_ui_button(renderer_ui *rui, char **name, const vec4 colors, const texture tex);

