#include "khg_ui/hash_utils.h"
#include "khg_ui/menu_stack_hash_table.h"
#include <stdlib.h>

menu_stack_hash_table *create_menu_stack_table() {
  menu_stack_hash_table *table = (menu_stack_hash_table *)malloc(sizeof(menu_stack_hash_table));
  table->entries = (menu_stack_hash_entry **)malloc(TABLE_SIZE * sizeof(menu_stack_hash_entry *));
  for (int i = 0; i < TABLE_SIZE; i++) {
    table->entries[i] = NULL;
  }
  return table;
}

menu_stack_hash_entry *create_menu_stack_entry(int key, vector(char **) value) {
  menu_stack_hash_entry *entry = (menu_stack_hash_entry *)malloc(sizeof(menu_stack_hash_entry));
  entry->key = key;
  entry->value = value;
  entry->next = NULL;
  return entry;
}

void insert_menu_stack(menu_stack_hash_table *table, int key, vector(char **) value) {
  unsigned long index = hash_function_int(&key);
  menu_stack_hash_entry *entry = table->entries[index];
  if (entry == NULL) {
    table->entries[index] = create_menu_stack_entry(key, value);
    return;
  }
  menu_stack_hash_entry *prev = NULL;
  while (entry != NULL) {
    if (compare_keys_int(&entry->key, &key)) {
      entry->value = value;
      return;
    }
    prev = entry;
    entry = entry->next;
  }
  prev->next = create_menu_stack_entry(key, value);
}

vector(char **) retrieve_menu_stack(menu_stack_hash_table *table, int key) {
  unsigned long index = hash_function_int(&key);
  menu_stack_hash_entry *entry = table->entries[index];
  while (entry != NULL) {
    if (compare_keys_int(&entry->key, &key)) {
      return entry->value;
    }
    entry = entry->next;
  }
  return NULL;
}

menu_stack_hash_entry *retrieve_menu_stack_entry(menu_stack_hash_table *table, int key) {
  unsigned long index = hash_function_int(&key);
  menu_stack_hash_entry *entry = table->entries[index];
  while (entry != NULL) {
    if (compare_keys_int(&entry->key, &key)) {
      return entry;
    }
    entry = entry->next;
  }
  return NULL;
}

void delete_menu_stack(menu_stack_hash_table *table, int key) {
  unsigned long index = hash_function_int(&key);
  menu_stack_hash_entry *entry = table->entries[index];
  menu_stack_hash_entry *prev = NULL;
  while (entry != NULL && !compare_keys_int(&entry->key, &key)) {
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

menu_stack_hash_entry *get_end_menu_stack_entry(menu_stack_hash_table *table) {
  menu_stack_hash_entry *last_entry = NULL;
  for (int i = 0; i < TABLE_SIZE; i++) {
    menu_stack_hash_entry *entry = table->entries[i];
    while (entry != NULL) {
      last_entry = entry;
      entry = entry->next;
    }
  }
  return last_entry;
}
