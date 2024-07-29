#include "gfx/strman.h"
#include "gfx/gfx.h"
#include <string.h>

void remove_i_str(char *str, int32_t index) {
  int32_t len = strlen(str);
  if (index >= 0 && index < len) {
    for (int32_t i = index; i < len - 1; i++) {
      str[i] = str[i + 1];
    }
    str[len - 1] = '\0';
  }
}

void remove_substr_str(char *str, int start_index, int end_index) {
  int len = strlen(str);

  memmove(str + start_index, str + end_index + 1, len - end_index);
  str[len - (end_index - start_index) + 1] = '\0'; 
}

void insert_i_str(char *str, char ch, int32_t index) {
  int len = strlen(str);

  if (index < 0 || index > len) {
    LF_ERROR("Invalid string index for inserting.\n");
    return;
  }

  for (int i = len; i > index; i--) {
    str[i] = str[i - 1];
  }

  str[index] = ch;
  str[len + 1] = '\0'; 
}

void insert_str_str(char* source, const char* insert, int32_t index) {
  int source_len = strlen(source);
  int insert_len = strlen(insert);

  if (index < 0 || index > source_len) {
    LF_ERROR("Index for inserting out of bounds\n");
    return;
  }

  memmove(source + index + insert_len, source + index, source_len - index + 1);

  memcpy(source + index, insert, insert_len);
}

void substr_str(const char* str, int start_index, int end_index, char* substring) {
  int substring_length = end_index - start_index + 1; 
  strncpy(substring, str + start_index, substring_length);
  substring[substring_length] = '\0';
}

int map_vals(int value, int from_min, int from_max, int to_min, int to_max) {
  return (value - from_min) * (to_max - to_min) / (from_max - from_min) + to_min;
}