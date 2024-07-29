#pragma once
#include <stdint.h>

void                     remove_i_str(char *str, int32_t index);
void                     remove_substr_str(char *str, int start_index, int end_index);
void                     insert_i_str(char *str, char ch, int32_t index);
void                     insert_str_str(char *source, const char *insert, int32_t index);
void                     substr_str(const char* str, int start_index, int end_index, char* substring);
int                      map_vals(int value, int from_min, int from_max, int to_min, int to_max);
