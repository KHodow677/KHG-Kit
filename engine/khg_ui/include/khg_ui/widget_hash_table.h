#pragma once

#include "khg_ui/widget.h"

typedef struct widget_hash_entry widget_hash_entry;

struct widget_hash_entry {
  char **key;
  widget value;
  widget_hash_entry *next;
};

typedef struct {
  widget_hash_entry **entries;
} widget_hash_table;

widget_hash_table *create_widget_table();
widget_hash_entry *create_widget_entry(char **key, widget value);
void insert_widget(widget_hash_table *table, char **key, widget value);
widget *retrieve_widget(widget_hash_table *table, char **key);
void delete_widget(widget_hash_table *table, char **key);

