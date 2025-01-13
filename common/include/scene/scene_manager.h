#pragma once

#include "scene/scene_utl.h"
#include "khg_stm/state_machine.h"

extern stm_state_machine SCENE_FSM;
extern stm_state INIT_SCENE;
extern stm_state MAIN_SCENE;
extern stm_state GAME_SCENE;
extern stm_state ERROR_SCENE;

void scenes_setup(void);
void scenes_switch(event_transition_command event);

