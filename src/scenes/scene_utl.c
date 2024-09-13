#include "scenes/scene_utl.h"
#include "game_manager.h"
#include "khg_stm/state_machine.h"
#include "khg_utl/error_func.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

void print_scene_error(void *state_data, stm_event *event) {
  utl_error_func("Scene error", utl_user_defined_data);
}

bool compare_scene_switch_command(void *ch, stm_event *event) {
  if (event->type != EVENT_SCENE_SWITCH) {
    return false;
  }
  return (intptr_t)ch == (intptr_t)event->data;
}

bool check_current_scene(const char *name) {
  return strcmp(stm_current_state(&SCENE_FSM)->data, name) == 0;
}

