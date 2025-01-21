#include "khg_utl/trigger.h"
#include <stdlib.h>
#include <string.h>

static void listener_disregard_trigger(utl_listener *const listener, const utl_trigger *const trigger);
static void listener_really_delete_inner(utl_listener *const listener);

static int eventname_equals(const char *const str1, const char *const str2) {
  return ((str1[0]==str2[0]) && (str1[1]==str2[1]) && (str1[2]==str2[2]) && (str1[3]==str2[3]));
}

static unsigned char name32_to_hash_key8(const char *const name) {
  return ((name[0])) ^ ((name[1] >> 2) | (name[1] << 6)) ^ ((name[2] >> 4) | (name[2] << 4)) ^ ((name[3] >> 6) | (name[3] << 2));    
}

static void trigger_free(utl_trigger *const trigger) {
  for (unsigned int i = 0; i < UTL_TRIGGER_TABLE_SIZE; i++) {
    if (trigger->event[i].allocated_listeners != 0) {
      free(trigger->event[i].listeners);
    }
  }
  free(trigger);
}

static void send_event_to_listener(utl_listener *const listener, const char *const eventname, const void *const eventdata) {
  if (eventname_equals(UTL_TRIGGER_DELETION_EVENT_NAME, eventname)) {
    listener_disregard_trigger(listener, eventdata);
    return;
  }
  if (NULL != listener->receptor_func) {
    listener->receptor_func(eventname, eventdata, listener->data);
  }
}

static int find_name_slot(const utl_trigger *const trigger, const char *const eventname, int *const index) {
  int vacant_index = -1;
  const unsigned char key = name32_to_hash_key8(eventname) % UTL_TRIGGER_TABLE_SIZE;
  for (unsigned int i = key; i < key + UTL_TRIGGER_TABLE_SIZE; i++) {
    if (eventname_equals(trigger->event[i % UTL_TRIGGER_TABLE_SIZE].name, eventname)) {
      *index = i % UTL_TRIGGER_TABLE_SIZE;
      return 1;
    }
    if (-1 == vacant_index && '\0' == trigger->event[i % UTL_TRIGGER_TABLE_SIZE].name[0]) {
      vacant_index = i % UTL_TRIGGER_TABLE_SIZE;
    }
  }
  *index = vacant_index;
  return 0;
}

static void trigger_add_listener(utl_trigger *const trigger, const char *const eventname, utl_listener *const listener) {
  int index;
  if (find_name_slot(trigger, eventname, &index) == 0) {
    if (index == -1) {
      return;
    }
    memcpy(trigger->event[index].name, eventname, 4);
    trigger->event[index].num_listeners = 1;
    trigger->event[index].allocated_listeners = 1;
    trigger->event[index].listeners = malloc(sizeof(utl_listener *));
    trigger->event[index].listeners[0] = listener;
  } 
  else {
    int free_listener_slot = -1;
    int i = trigger->event[index].allocated_listeners;
    while (i--) {
      if (trigger->event[index].listeners[i] == listener) {
	      return;
      }
      if (NULL == trigger->event[index].listeners[i]) {
	      free_listener_slot = i;
      }
    }
    if (free_listener_slot > -1) {
      trigger->event[index].listeners[free_listener_slot] = listener;
    } 
    else {
      trigger->event[index].allocated_listeners++;
      trigger->event[index].listeners = realloc(trigger->event[index].listeners, sizeof(utl_listener *)*trigger->event[index].allocated_listeners);
      trigger->event[index].listeners[trigger->event[index].allocated_listeners - 1] = listener;
    }
    trigger->event[index].num_listeners++;
  }
}

static void listener_add_trigger(utl_listener *listener, utl_trigger *trigger) {
  if (listener->allocated_triggers == 0) {
    listener->allocated_triggers = 1;
    listener->num_triggers = 1;
    listener->triggers = malloc(sizeof(utl_trigger *)*listener->allocated_triggers);
    listener->triggers[0] = trigger;
  } 
  else {
    int spare_index = -1;
    for (unsigned int i=0; i<listener->allocated_triggers; i++) {
      if (listener->triggers[i] == trigger) {
	      return;
      }
      if (NULL == listener->triggers[i]) {
	      spare_index = i;
      }
    }
    if (spare_index > -1) {
      listener->triggers[spare_index] = trigger;
    } 
    else {
      listener->allocated_triggers++;
      listener->triggers = realloc(listener->triggers, sizeof(utl_trigger *)*listener->allocated_triggers);
      listener->triggers[listener->allocated_triggers - 1] = trigger;
    }
    listener->num_triggers++;
  }
}


static void trigger_remove_listenerlist_index(utl_trigger *trigger, int slot, int listindex) {
  trigger->event[slot].listeners[listindex] = NULL;
  trigger->event[slot].num_listeners--;
  if (trigger->event[slot].num_listeners == 0) {
    trigger->event[slot].name[0] = '\0';
    trigger->event[slot].allocated_listeners = 0;
    free(trigger->event[slot].listeners);
    trigger->event[slot].listeners = NULL;
  }
}

static void trigger_disregard_listener(utl_trigger *trigger, utl_listener *listener) {
  for (unsigned int i = 0; i < UTL_TRIGGER_TABLE_SIZE; i++) {
    if (trigger->event[i].name[0] == '\0') {
      continue;
    }
    for (unsigned int s = 0; s < trigger->event[i].allocated_listeners; s++) {
      if (trigger->event[i].listeners[s] != listener) {
        continue;
      }
      trigger_remove_listenerlist_index(trigger, i, s);
      break;
    }
  }
}

static void listener_disregard_trigger(utl_listener *const listener, const utl_trigger *const trigger) {
  if (!listener->num_triggers) {
    return;
  }
  for (unsigned int i = 0; i < listener->allocated_triggers; i++) {
    if (listener->triggers[i] != trigger) {
      continue;
    }
    listener->triggers[i] = NULL;
    listener->num_triggers--;
    if (0 == listener->num_triggers) {
      listener->allocated_triggers = 0;
      free(listener->triggers);
      listener->triggers = NULL;
      if (listener->auto_delete == 2) {
        listener_really_delete_inner(listener);
      } 
      else if (listener->auto_delete == 1) {
        send_event_to_listener(listener, UTL_LISTENER_AUTODELETION_EVENT_NAME, listener);
      }
    }
    break;
  }
}

utl_trigger *utl_trigger_new(void) {
  utl_trigger *rtn = malloc(sizeof(utl_trigger));
  for (unsigned int i = 0; i < UTL_TRIGGER_TABLE_SIZE; i++) {
    rtn->event[i].name[0] = rtn->event[i].name[1] = '\0';
    rtn->event[i].num_listeners = 0;
    rtn->event[i].allocated_listeners = 0;
    rtn->event[i].listeners = NULL;
  }
  return rtn;
}

void utl_trigger_delete(utl_trigger *const trigger) {
  utl_trigger_event(trigger, UTL_TRIGGER_DELETION_EVENT_NAME, trigger);
  trigger_free(trigger);
}

void utl_trigger_event(utl_trigger *const trigger, const char *const eventname, const void *const eventdata) {
  int index;
  if (find_name_slot(trigger, eventname, &index) == 0) {
    return;
  }
  for (unsigned i = 0; i < trigger->event[index].allocated_listeners; i++) {
    if (trigger->event[index].listeners[i] != NULL) {
      send_event_to_listener(trigger->event[index].listeners[i], eventname, eventdata);
    }
  }
}

int utl_trigger_unlisten(utl_trigger *const trigger, const char *const eventname, utl_listener *const listener) {
  int index;
  if (find_name_slot(trigger, eventname, &index) == 0) {
    return 0;
  } 
  else {
    int i = trigger->event[index].allocated_listeners;
    while (i--) {
      if (trigger->event[index].listeners[i] == listener) {
	      trigger_remove_listenerlist_index(trigger, index, i);
        return 1;
      }
    }
  }
  return 0;
}

void utl_trigger_listen(utl_trigger *const trigger, const char *const eventname, utl_listener *const listener) {
  trigger_add_listener(trigger, UTL_TRIGGER_DELETION_EVENT_NAME, listener);
  listener_add_trigger(listener, trigger);
  trigger_add_listener(trigger, eventname, listener);
}

void utl_listener_init(utl_listener *const listener) {
  listener->num_triggers = listener->allocated_triggers = 0;
  listener->triggers = NULL;
  listener->receptor_func = NULL;
  listener->data = NULL;
  listener->auto_delete = 0;
}

utl_listener *utl_listener_new(void) {
  utl_listener *rtn = malloc(sizeof(utl_listener));
  utl_listener_init(rtn);
  return rtn;
}


static void listener_really_delete_inner(utl_listener *const listener) {
  send_event_to_listener(listener, UTL_LISTENER_DELETION_EVENT_NAME, listener->data);
  listener->data = NULL;
  if (listener->num_triggers) {
    for (unsigned int i = 0; i < listener->allocated_triggers; i++) {
      if (listener->triggers[i]) {
	      trigger_disregard_listener(listener->triggers[i], listener);
      }
    }
  }
  if (listener->allocated_triggers) {
    free(listener->triggers);
  }
}


void utl_listener_delete_inner(utl_listener *const listener) {
  listener_really_delete_inner(listener);
}


void utl_listener_delete(utl_listener *const listener) {
  utl_listener_delete_inner(listener);
  free(listener);
}

utl_listener *utl_listener_allow_autodelete(utl_listener *const listener, const int free_on_delete) {
  if (free_on_delete) {
    listener->auto_delete = 2;
  }
  else {
    listener->auto_delete = 1;
  }
  return listener;
}

int utl_listener_trigger_event_name_private(const char *const eventname) {
  return eventname_equals(eventname, UTL_LISTENER_DELETION_EVENT_NAME) || eventname_equals(eventname, UTL_LISTENER_AUTODELETION_EVENT_NAME) || eventname_equals(eventname, UTL_TRIGGER_DELETION_EVENT_NAME);
}


utl_listener *utl_listener_set_func(utl_listener *const listener, utl_listener_func *const func) {
  listener->receptor_func = func;
  return listener;
}

utl_listener *utl_listener_set_data(utl_listener *const listener, void *const listener_data) {
  listener->data = listener_data;
  return listener;
}

void *utl_listener_get_data(utl_listener *const listener) {
  return listener->data;
}

