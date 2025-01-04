#pragma once

#include <stdbool.h>

enum stm_handle_event_return_vals {
  STM_ERR_ARG = -2,
  STM_ERROR_STATE_REACHED,
  STM_STATE_CHANGED,
  STM_STATE_LOOP_SELF,
  STM_NO_STATE_CHANGE,
  STM_FINAL_STATE_REACHED,
};

typedef struct stm_event {
  int type;
  void *data;
} stm_event;

typedef struct stm_state stm_state;

typedef struct stm_transition {
  int event_type;
  void *condition;
  bool (*guard)(void *condition, stm_event *event);
  void (*action)(void *current_state_data, stm_event *event, void *new_state_data);
  stm_state *next_state;
} stm_transition;

struct stm_state {
  stm_state *parent_state;
  stm_state *entry_state;
  stm_transition *transitions;
  unsigned int num_transitions;
  void *data;
  void (*entry_action)(void *state_data, stm_event *event);
  void (*exit_action)(void *state_data, stm_event *event);
};

typedef struct stm_state_machine {
  stm_state *current_state;
  stm_state *previous_state;
  stm_state *error_state;
} stm_state_machine;

void stm_init(stm_state_machine *fsm, stm_state *initial_state, stm_state *error_state);

int stm_handle_event(stm_state_machine *fsm, stm_event *event);

stm_state *stm_current_state(stm_state_machine *fsm);
stm_state *stm_previous_state(stm_state_machine *fsm);

bool stm_stopped(stm_state_machine *fsm);

