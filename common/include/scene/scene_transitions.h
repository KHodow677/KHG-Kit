#pragma once

#include "khg_stm/state_machine.h"

void load_main_scene(void *old_state_data, stm_event *event, void *new_state_data);
void load_game_scene(void *old_state_data, stm_event *event, void *new_state_data);

