#pragma once

#define UTL_TRIGGER_TABLE_SIZE 256

#define UTL_TRIGGER_DELETION_EVENT_NAME "_tde"
#define UTL_LISTENER_DELETION_EVENT_NAME "_lde"
#define UTL_LISTENER_AUTODELETION_EVENT_NAME "_lau"

typedef struct utl_trigger utl_trigger;
typedef void (utl_listener_func)(const char *event_name, const void *event_data, void *listener_data);

typedef struct {
  unsigned short num_triggers;
  unsigned short allocated_triggers;
  utl_trigger **triggers;
  utl_listener_func *receptor_func;
  void *data;
  char auto_delete;
} utl_listener;

struct utl_trigger {
  struct {
    char name[4];
    unsigned short num_listeners;
    unsigned short allocated_listeners;
    utl_listener **listeners;
  } event[UTL_TRIGGER_TABLE_SIZE];
};

#define utl_listener_new_with_func(func) utl_listener_set_func(utl_listener_new(), (func))
utl_listener *utl_listener_new(void);
void utl_listener_init(utl_listener *const listener);
void utl_listener_delete(utl_listener *const listener);
void utl_listener_delete_inner(utl_listener *const listener);
utl_listener *utl_listener_set_func(utl_listener *const listener, utl_listener_func *const func);
utl_listener *utl_listener_set_data(utl_listener *const listener, void *const listener_data);
void *utl_listener_get_data(utl_listener *const listener);
utl_listener *utl_listener_allow_autodelete(utl_listener *const listener, const int free_on_delete);
int utl_listener_trigger_event_name_private(const char *const event_name);

utl_trigger *utl_trigger_new(void);
void utl_trigger_delete(utl_trigger *const trigger);
void utl_trigger_listen(utl_trigger *const trigger, const char *const event_name, utl_listener *const listener);
int utl_trigger_unlisten(utl_trigger *const trigger, const char *const event_name, utl_listener *const listener);
void utl_trigger_event(utl_trigger *const trigger, const char *const event_name, const void *const event_data);

