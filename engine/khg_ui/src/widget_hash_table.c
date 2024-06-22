#include "khg_ui/hash_utils.h"
#include "khg_ui/widget_hash_table.h"
#include <stdlib.h>

widget_hash_table *create_widget_table() {
  widget_hash_table *table = (widget_hash_table *)malloc(sizeof(widget_hash_table));
  table->entries = (widget_hash_entry **)malloc(TABLE_SIZE * sizeof(widget_hash_entry *));
  for (int i = 0; i < TABLE_SIZE; i++) {
    table->entries[i] = NULL;
  }
  return table;
}

widget_hash_entry *create_widget_entry(char **key, widget value) {
  widget_hash_entry *entry = (widget_hash_entry *)malloc(sizeof(widget_hash_entry));
  entry->key = key;
  entry->value = value;
  entry->next = NULL;
  return entry;
}

void insert_widget(widget_hash_table *table, char **key, widget value) {
  unsigned long index = hash_function(key);
  widget_hash_entry *entry = table->entries[index];
  if (entry == NULL) {
    table->entries[index] = create_widget_entry(key, value);
    return;
  }
  widget_hash_entry *prev = NULL;
  while (entry != NULL) {
    if (compare_keys(entry->key, key)) {
      entry->value = value;
      return;
    }
    prev = entry;
    entry = entry->next;
  }
  prev->next = create_widget_entry(key, value);
}

widget *retrieve_widget(widget_hash_table *table, char **key) {
  unsigned long index = hash_function(key);
  widget_hash_entry *entry = table->entries[index];
  while (entry != NULL) {
    if (compare_keys(entry->key, key)) {
      return &entry->value;
    }
    entry = entry->next;
  }
  return NULL;
}

void delete_widget(widget_hash_table *table, char **key) {
  unsigned long index = hash_function(key);
  widget_hash_entry *entry = table->entries[index];
  widget_hash_entry *prev = NULL;
  while (entry != NULL && !compare_keys(entry->key, key)) {
    prev = entry;
    entry = entry->next;
  }
  if (entry == NULL) {
    return;
  }
  if (prev == NULL) {
    table->entries[index] = entry->next;
  } 
  else {
    prev->next = entry->next;
  }
  free(entry);
}


