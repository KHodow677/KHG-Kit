#include "khg_stm/state_machine.h"
#include <time.h>

static void stm_go_to_error_state(stm_state_machine *fsm, stm_event *const event) {
  fsm->previous_state = fsm->current_state;
  fsm->current_state = fsm->error_state;
  if (fsm->current_state && fsm->current_state->entry_action) {
    fsm->current_state->entry_action( fsm->current_state->data, event );
  }
}

static stm_transition *stm_get_transition(stm_state_machine *fsm, stm_state *state, stm_event *const event) {
  for (unsigned int i = 0; i < state->num_transitions; i++) {
    stm_transition *t = &state->transitions[ i ];
    if (t->event_type == event->type) {
      if ( !t->guard ) {
        return t;
      }
      else if ( t->guard( t->condition, event ) ) {
        return t;
      }
    }
  }
  return NULL;
}

void stm_init(stm_state_machine *fsm, stm_state *initial_state, stm_state *error_state) {
  if (!fsm) {
    return;
  }
  fsm->current_state = initial_state;
  fsm->previous_state = NULL;
  fsm->error_state = error_state;
}

int stm_handle_event(stm_state_machine *fsm, stm_event *event) {
  if (!fsm || !event) {
    return STM_ERR_ARG;
  }
  if (!fsm->current_state) {
    stm_go_to_error_state(fsm, event);
    return STM_ERROR_STATE_REACHED;
  }
  if ( !fsm->current_state->num_transitions ) {
    return STM_NO_STATE_CHANGE;
  }
  stm_state *next_state = fsm->current_state;
  do {
    stm_transition *transition = stm_get_transition(fsm, next_state, event);
    if (!transition) {
      next_state = next_state->parent_state;
      continue;
    }
    if (!transition->next_state) {
      stm_go_to_error_state( fsm, event );
      return STM_ERROR_STATE_REACHED;
    }
    next_state = transition->next_state;
    while (next_state->entry_state) {
      next_state = next_state->entry_state;
    }
    if (next_state != fsm->current_state && fsm->current_state->exit_action) {
      fsm->current_state->exit_action(fsm->current_state->data, event);
    }
    if (transition->action) {
      transition->action(fsm->current_state->data, event, next_state-> data);
    }
    if (next_state != fsm->current_state && next_state->entry_action) {
      next_state->entry_action(next_state->data, event);
    }
    fsm->previous_state = fsm->current_state;
    fsm->current_state = next_state;
    if (fsm->current_state == fsm->previous_state) {
      return STM_STATE_LOOP_SELF;
    }
    if (fsm->current_state == fsm->error_state) {
      return STM_ERROR_STATE_REACHED;
    }
    if (!fsm->current_state->num_transitions) {
      return STM_FINAL_STATE_REACHED;
    }
    return STM_STATE_CHANGED;
  } while (next_state);
  return STM_NO_STATE_CHANGE;
}

stm_state *stm_current_state(stm_state_machine *fsm) {
  if (!fsm) {
    return NULL;
  }
  return fsm->current_state;
}

stm_state *stm_previous_state(stm_state_machine *fsm) {
  if (!fsm) {
    return NULL;
  }
  return fsm->previous_state;
}

bool stm_stopped(stm_state_machine *fsm) {
  if (!fsm) {
    return true;
  }
  return fsm->current_state->num_transitions == 0;
}

