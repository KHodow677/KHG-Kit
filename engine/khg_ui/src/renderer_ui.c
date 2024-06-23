#include "khg_ui/renderer_ui.h"
#include "khg_ui/menu_stack_hash_table.h"
#include "khg_ui/widget.h"
#include "khg_ui/widget_hash_table.h"
#include "khg_utils/vector.h"
#include <string.h>

int x_padd = 0;
int y_padd = 0;
int width = 0;
int height = 0;

float timer = 0;
int current_id = 0;
bool id_was_set = 0;

void reset_custom_widget(char **key, widget value) {
  if (value.type == widget_custom) {
    value.custom_widget_used = false;
  }
}

void manage_widget(char **key, widget value, vector(char **) *menu_stack, char ***next_menu, vector(char **) *current_menu_stack_copy, bool *should_ignore, int *next_stack_size_to_look, int *next_stack_size_to_look_min) {
  if (value.type == widget_begin_menu) {
    vector_push_back(*menu_stack, key);
    if (key == *next_menu) {
      if (!vector_empty(*current_menu_stack_copy)) {
        *next_menu = *vector_front(*current_menu_stack_copy);
        vector_erase(*current_menu_stack_copy, 0);
        *should_ignore = true;
      }
      else {
        *should_ignore = false;
        *next_stack_size_to_look_min = vector_size(*menu_stack);
      }
    }
    else if (key != *next_menu && *should_ignore != true) {
      *should_ignore = true;
      *next_stack_size_to_look = vector_size(*menu_stack) - 1;
      
    }
  }
}

void render_frame(renderer_ui *rui, renderer_2d *r2d, font *f, vec2 mouse_pos, bool mouse_click, bool mouse_held, bool mouse_released, bool escape_released, const char **typed_input, float delta_time) {
  if (!id_was_set) {
    return;
  }
  vector(char **) iter_menu_stack = retrieve_menu_stack(&rui->all_menu_stacks, current_id);
  if (iter_menu_stack == NULL) {
    vector(char **) new_menu_stack;
    insert_menu_stack(&rui->all_menu_stacks, current_id, new_menu_stack);
    iter_menu_stack = retrieve_menu_stack(&rui->all_menu_stacks, current_id);
  }
  vector(char **) current_menu_stack = iter_menu_stack;
  id_was_set = 0;
  current_id = 0;
  if (escape_released && !vector_empty(current_menu_stack)) {
    vector_pop_back(current_menu_stack);
  }
  timer += delta_time * 2;
  if (timer >= 2.0f) {
    timer -= 2;
  }
  iterate_widget_table(&rui->widgets, reset_custom_widget);
  vector(widget_pair) widgets_copy;
  vector_reserve(widgets_copy, vector_size(rui->widget_vector));
  vector(char **) current_menu_stack_copy = current_menu_stack;
  vector(char **) menu_stack;
  char **next_menu;
  *next_menu = strdup("");
  bool should_ignore = false;
  if (!vector_empty(current_menu_stack_copy)) {
    next_menu = *vector_front(current_menu_stack_copy);
    vector_erase(current_menu_stack_copy, 0);
    should_ignore = true;
  }
  int next_stack_size_to_look = 0;
  int next_stack_size_to_look_min = 0;

}
