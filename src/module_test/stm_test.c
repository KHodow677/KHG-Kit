#include "stm_test.h"
#include "khg_stm/state_machine.h"
#include <stdint.h>
#include <stdio.h>

enum event_type {
  EVENT_KEYBOARD,
};

enum transition_type {
  TO_H,
  TO_I,
  TO_A,
  TO_IDLE,
};

static void print_hi_msg(void *old_state_data, stm_event *event, void *new_state_data) {
  puts("Hi!");
}

static void print_ha_msg(void *old_state_data, stm_event *event, void *new_state_data) {
  puts("Ha-ha");
}

static void print_err_msg(void *state_data, stm_event *event) {
  puts("ERROR STATE!");
}

static void print_current_state(stm_state_machine *fsm) {
  printf("Current State: %s\n", (char *)fsm->current_state->data);
}

static void print_enter_msg(void *state_data, stm_event *event) {
  printf("Entering %s state\n", (char *)state_data);
}

static void print_exit_msg(void *state_data, stm_event *event) {
  printf("Exiting %s state\n", (char *)state_data);
}

static bool compare_keyboard_char(void *ch, stm_event *event) {
  if (event->type != EVENT_KEYBOARD) {
    return false;
  }
  return (intptr_t)ch == (intptr_t)event->data;
}

static void print_reset(void *old_state_data, stm_event *event, void *new_state_data) {
  puts("Resetting");
}

static stm_state check_chars_group_state, idle_state, h_state, i_state, a_state;

static stm_state check_chars_group_state = {
  .parent_state = NULL,
  .entry_state = &idle_state,
  .transitions = (stm_transition[]){ { EVENT_KEYBOARD, (void *)(intptr_t)'!', &compare_keyboard_char, &print_reset, &idle_state, } },
  .num_transitions = 2,
  .data = "group",
  .entry_action = &print_enter_msg,
  .exit_action = &print_exit_msg,
};

static stm_state idle_state = {
  .parent_state = &check_chars_group_state,
  .entry_state = NULL,
  .transitions = (stm_transition[]){ { EVENT_KEYBOARD, (void *)(intptr_t)TO_H, &compare_keyboard_char, NULL, &h_state } },
  .num_transitions = 1,
  .data = "idle",
  .entry_action = &print_enter_msg,
  .exit_action = &print_exit_msg,
};

static stm_state h_state = {
  .parent_state = &check_chars_group_state,
  .entry_state = NULL,
  .transitions = (stm_transition[]){ { EVENT_KEYBOARD, (void *)(intptr_t)TO_A, &compare_keyboard_char, NULL, &a_state }, { EVENT_KEYBOARD, (void *)(intptr_t)TO_I, &compare_keyboard_char, NULL, &i_state } },
  .num_transitions = 2,
  .data = "H",
  .entry_action = &print_enter_msg,
  .exit_action = &print_exit_msg,
};

static stm_state i_state = {
  .parent_state = &check_chars_group_state,
  .entry_state = NULL,
  .transitions = (stm_transition[]){ { EVENT_KEYBOARD, (void *)(intptr_t)TO_IDLE, &compare_keyboard_char, &print_hi_msg, &idle_state } },
  .num_transitions = 1,
  .data = "I",
  .entry_action = &print_enter_msg,
  .exit_action = &print_exit_msg,
};

static stm_state a_state = {
  .parent_state = &check_chars_group_state,
  .entry_state = NULL,
  .transitions = (stm_transition[]){ { EVENT_KEYBOARD, (void *)(intptr_t)TO_IDLE, &compare_keyboard_char, &print_ha_msg, &idle_state } },
  .num_transitions = 1,
  .data = "A",
  .entry_action = &print_enter_msg,
  .exit_action = &print_exit_msg
};

static stm_state errorState = {
  .entry_action = &print_err_msg
};

int stm_test(void) {
  stm_state_machine m;
  int ch;
  stm_init(&m, &idle_state, &errorState);
  stm_handle_event(&m, &(stm_event){ EVENT_KEYBOARD, (void *)(intptr_t)TO_H });
  stm_handle_event(&m, &(stm_event){ EVENT_KEYBOARD, (void *)(intptr_t)TO_I });
  stm_handle_event(&m, &(stm_event){ EVENT_KEYBOARD, (void *)(intptr_t)TO_IDLE });
  stm_handle_event(&m, &(stm_event){ EVENT_KEYBOARD, (void *)(intptr_t)TO_H });
  stm_handle_event(&m, &(stm_event){ EVENT_KEYBOARD, (void *)(intptr_t)TO_A });
  stm_handle_event(&m, &(stm_event){ EVENT_KEYBOARD, (void *)(intptr_t)TO_IDLE });
  return 0;
}

