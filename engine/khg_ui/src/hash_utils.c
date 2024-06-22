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

