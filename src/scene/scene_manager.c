#include "scene/scene_manager.h"
#include "scene/scene_transitions.h"
#include "scene/scene_utl.h"
#include "khg_stm/state_machine.h"
#include <stdint.h>

stm_state_machine SCENE_FSM;

stm_state PARENT_SCENE = {
  .parent_state = NULL,
  .entry_state = &MAIN_SCENE,
  .transitions = (stm_transition[]){ { EVENT_SCENE_SWITCH, (void *)(intptr_t)'!', &compare_scene_switch_command, NULL, &MAIN_SCENE } },
  .num_transitions = 2,
  .data = "GROUP",
};

stm_state MAIN_SCENE = {
  .parent_state = &PARENT_SCENE,
  .entry_state = NULL,
  .transitions = (stm_transition[]){ { EVENT_SCENE_SWITCH, (void *)(intptr_t)TO_GAME_SCENE, &compare_scene_switch_command, &load_game_scene, &GAME_SCENE } },
  .num_transitions = 2,
  .data = "MAIN",
};

stm_state GAME_SCENE = {
  .parent_state = &PARENT_SCENE,
  .entry_state = NULL,
  .transitions = (stm_transition[]){ { EVENT_SCENE_SWITCH, (void *)(intptr_t)TO_MAIN_SCENE, &compare_scene_switch_command, &load_main_scene, &MAIN_SCENE } },
  .num_transitions = 1,
  .data = "GAME",
};

stm_state ERROR_SCENE = {
  .data = "ERROR",
  .entry_action = &print_scene_error
};


void scenes_setup(void) {
  stm_init(&SCENE_FSM, &MAIN_SCENE, &ERROR_SCENE);
}
