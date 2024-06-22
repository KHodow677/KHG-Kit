#pragma once

#define TABLE_SIZE 100

unsigned long hash_function(char **key);
int compare_keys(char **key1, char **key2);
