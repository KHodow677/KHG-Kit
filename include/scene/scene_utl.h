#pragma once

#include "khg_stm/state_machine.h"
#include <stdbool.h>

typedef enum event_transition_type {
  EVENT_SCENE_SWITCH,
} event_transition_type;

typedef enum event_transition_command {
  TO_MAIN_SCENE,
  TO_GAME_SCENE,
} event_transition_command;

void print_scene_error(void *state_data, stm_event *event);

bool compare_scene_switch_command(void *ch, stm_event *event);
const bool check_current_scene(const char *name);

