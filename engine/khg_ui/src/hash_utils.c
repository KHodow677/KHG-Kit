#include "khg_ui/hash_utils.h"
#include <string.h>

unsigned long hash_function(char **key) {
  unsigned long hash = 5381;
  int c;
  while ((c = **key++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash % TABLE_SIZE;
}

int compare_keys(char **key1, char **key2) {
  while (*key1 && *key2) {
    if (strcmp(*key1, *key2) != 0) {
      return 0;
    }
    key1++;
    key2++;
  }
  return *key1 == *key2;
}

unsigned long hash_function_int(int *key) {
  unsigned long hash = 5381;
  int num = *key;
  while (num) {
    hash = ((hash << 5) + hash) + (num % 10);
    num /= 10;
  }
  return hash % TABLE_SIZE;
}

int compare_keys_int(int *key1, int *key2) {
  return *key1 == *key2;
}
