#include "stm_test.h"
#include "khg_stm/state_machine.h"
#include <stdint.h>
#include <stdio.h>

enum event_type {
  EVENT_KEYBOARD,
};

static void printHiMsg(void *old_state_data, stm_event *event, void *new_state_data) {
  puts("Hi!");
}

static void printHaMsg(void *old_state_data, stm_event *event, void *new_state_data) {
  puts("Ha-ha");
}

static void printErrMsg(void *state_data, stm_event *event) {
  puts("ERROR STATE!");
}

static void print_current_state(stm_state_machine *fsm) {
  printf("Current State: %s\n", (char *)fsm->current_state->data);
}

static void printEnterMsg(void *state_data, stm_event *event) {
  printf("Entering %s state\n", (char *)state_data);
}

static void printExitMsg(void *state_data, stm_event *event) {
  printf("Exiting %s state\n", (char *)state_data);
}

static bool compareKeyboardChar(void *ch, stm_event *event) {
  if (event->type != EVENT_KEYBOARD) {
    return false;
  }
  return (intptr_t)ch == (intptr_t)event->data;
}

static void printReset(void *old_state_data, stm_event *event, void *new_state_data) {
  puts("Resetting");
}


static stm_state checkCharsGroupState, idleState, hState, iState, aState;

static stm_state checkCharsGroupState = {
   .parent_state = NULL,
   .entry_state = &idleState,
   .transitions = (stm_transition[]){ { EVENT_KEYBOARD, (void *)(intptr_t)'!', &compareKeyboardChar, &printReset, &idleState, } },
   .num_transitions = 2,
   .data = "group",
   .entry_action = &printEnterMsg,
   .exit_action = &printExitMsg,
};

static stm_state idleState = {
 .parent_state = &checkCharsGroupState,
 .entry_state = NULL,
 .transitions = (stm_transition[]){ { EVENT_KEYBOARD, (void *)(intptr_t)'h', &compareKeyboardChar, NULL, &hState } },
 .num_transitions = 1,
 .data = "idle",
 .entry_action = &printEnterMsg,
 .exit_action = &printExitMsg,
};

static stm_state hState = {
 .parent_state = &checkCharsGroupState,
 .entry_state = NULL,
 .transitions = (stm_transition[]){ { EVENT_KEYBOARD, (void *)(intptr_t)'a', &compareKeyboardChar, NULL, &aState }, { EVENT_KEYBOARD, (void *)(intptr_t)'i', &compareKeyboardChar, NULL, &iState } },
 .num_transitions = 2,
 .data = "H",
 .entry_action = &printEnterMsg,
 .exit_action = &printExitMsg,
};

static stm_state iState = {
 .parent_state = &checkCharsGroupState,
 .entry_state = NULL,
 .transitions = (stm_transition[]){ { EVENT_KEYBOARD, (void *)(intptr_t)'0', &compareKeyboardChar, &printHiMsg, &idleState } },
 .num_transitions = 1,
 .data = "I",
 .entry_action = &printEnterMsg,
 .exit_action = &printExitMsg,
};

static stm_state aState = {
 .parent_state = &checkCharsGroupState,
 .entry_state = NULL,
 .transitions = (stm_transition[]){ { EVENT_KEYBOARD, (void *)(intptr_t)'0', &compareKeyboardChar, &printHaMsg, &idleState } },
 .num_transitions = 1,
 .data = "A",
 .entry_action = &printEnterMsg,
 .exit_action = &printExitMsg
};

static stm_state errorState = {
  .entry_action = &printErrMsg
};

int stm_test(void) {
  stm_state_machine m;
  int ch;
  stm_init(&m, &idleState, &errorState);
  stm_handle_event(&m, &(stm_event){ EVENT_KEYBOARD, (void *)(intptr_t)'h' });
  stm_handle_event(&m, &(stm_event){ EVENT_KEYBOARD, (void *)(intptr_t)'i' });
  stm_handle_event(&m, &(stm_event){ EVENT_KEYBOARD, (void *)(intptr_t)'0' });
  stm_handle_event(&m, &(stm_event){ EVENT_KEYBOARD, (void *)(intptr_t)'h' });
  stm_handle_event(&m, &(stm_event){ EVENT_KEYBOARD, (void *)(intptr_t)'a' });
  stm_handle_event(&m, &(stm_event){ EVENT_KEYBOARD, (void *)(intptr_t)'0' });
  return 0;
}

