#pragma once

#include <stdbool.h>

bool is_number(const char *str);

bool print_buffer(const char *buffer, const int length, void *user_data);
bool ignore_buffer(const char *buffer, int length, void *user_data);

