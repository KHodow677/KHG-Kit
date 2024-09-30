#pragma once

#include "khg_stm/state_machine.h"

void load_tutorial_scene(void *old_state_data, stm_event *event, void *new_state_data);
void load_game_scene(void *old_state_data, stm_event *event, void *new_state_data);
void load_title_scene(void *old_state_data, stm_event *event, void *new_state_data);

void run_title(void);
bool run_scene_gameplay(float delta);

