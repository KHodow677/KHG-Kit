#pragma once

#include "khg_utils/vector.h"

typedef struct menu_stack_hash_entry menu_stack_hash_entry;

struct menu_stack_hash_entry {
  char **key;
  vector(char **) value;
  menu_stack_hash_entry *next;
};

typedef struct {
  menu_stack_hash_entry **entries;
} menu_stack_hash_table;

menu_stack_hash_table *create_menu_stack_table();
menu_stack_hash_entry *create_menu_stack_entry(char **key, vector(char **) value);
void insert_menu_stack(menu_stack_hash_table *table, char **key, vector(char **) value);
vector(char **) *retrieve_menu_stack(menu_stack_hash_table *table, char **key);
void delete_menu_stack(menu_stack_hash_table *table, char **key);

