#pragma once

#include "khg_stm/state_machine.h"
#include <stdbool.h>

enum event_transition_type {
  EVENT_SCENE_SWITCH,
};

enum event_transition_command {
  TO_TITLE_SCENE,
  TO_TUTORIAL_SCENE,
};

void print_scene_error(void *state_data, stm_event *event);

bool compare_scene_switch_command(void *ch, stm_event *event);
bool check_current_scene(const char *name);

