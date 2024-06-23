#pragma once

#define TABLE_SIZE 100

unsigned long hash_function(char **key);
int compare_keys(char **key1, char **key2);
unsigned long hash_function_int(int *key);
int compare_keys_int(int *key1, int *key2);
