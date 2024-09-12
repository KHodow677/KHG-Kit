#pragma once

#include <stddef.h>
#include <stdbool.h>

enum stm_handle_event_return_vals {
  STM_ERR_ARG = -2,
  STM_ERROR_STATE_REACHED,
  STM_STATE_CHANGED,
  STM_STATE_LOOP_SELF,
  STM_NO_STATE_CHANGE,
  STM_FINAL_STATE_REACHED,
};

struct stm_event {
  int type;
  void *data;
};

struct stm_state;

struct stm_transition {
  int event_type;
  void *condition;
  bool (*guard)(void *condition, struct stm_event *event);
  void (*action)(void *current_state_data, struct stm_event *event, void *new_state_data);
  struct stm_state *next_state;
};

struct stm_state {
  struct stm_state *parent_state;
  struct stm_state *entry_state;
  struct stm_transition *transitions;
  size_t num_transitions;
  void *data;
  void (*entry_action)(void *state_data, struct stm_event *event);
  void (*exit_action)(void *state_data, struct stm_event *event);
};

struct stm_state_machine {
  struct stm_state *current_state;
  struct stm_state *previous_state;
  struct stm_state *error_state;
};

void stm_init(struct stm_state_machine *fsm, struct stm_state *initial_state, struct stm_state *error_state);

int stm_handle_event(struct stm_state_machine *fsm, struct stm_event *event);

struct stm_state *stm_current_state(struct stm_state_machine *fsm);
struct stm_state *stm_previous_state(struct stm_state_machine *fsm);

bool stm_stopped(struct stm_state_machine *fsm);

