#include "data_utl/map_utl.h"
#include "game.h"
#include "khg_utl/map.h"
#include <stdio.h>
#include <stdlib.h>

int compare_i(const key_type a, const key_type b) {
  const int* ia = (const int*)a;
  const int* ib = (const int*)b;
  return (*ia - *ib);
}

void int_deallocator(void* data) {
  free(data);
}

int main(int argc, char *argv[]) {
  /*
  utl_map* myMap = utl_map_create(compare_i, int_deallocator, int_deallocator);
  int *key1 = malloc(sizeof(int));
  int *key2 = malloc(sizeof(int));
  int *key3 = malloc(sizeof(int));
  int *value1 = malloc(sizeof(int));
  int *value2 = malloc(sizeof(int));
  int *value3 = malloc(sizeof(int));
  *key1 = 0;
  *key2 = 1;
  *key3 = 2;

  utl_map_insert(myMap, key1, value1);
  utl_map_insert(myMap, key2, value2);
  utl_map_insert(myMap, key3, value3);
  utl_map_erase(myMap, key1);
  utl_map_erase(myMap, key2);
  utl_map_erase(myMap, key3);
  
  int lookupKey = 5;
  int *foundValue = utl_map_at(myMap, &lookupKey);
  if (foundValue) {
    printf("Found value: %d\n", *foundValue);
  }
  else {
    printf("Key not found.\n");
  }
  utl_map_deallocate(myMap);
  return 0;
  */
  return game_run();
}

