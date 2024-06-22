#pragma once

#include "khg_ui/menu_stack_hash_table.h"
#include "khg_ui/widget_hash_table.h"

extern int x_padd;
extern int y_padd;
extern int width;
extern int height;

typedef struct {
  vector(widget_pair) widget_vector;
  widget_hash_table widgets;
  menu_stack_hash_table all_menu_stacks;
  char **id_str;
  char **current_text_box;
} internal;

