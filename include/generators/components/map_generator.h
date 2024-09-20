#pragma once

#include "khg_utl/vector.h"
#define MAX_PATH 260

int get_csv_rows(char *path);
int get_csv_cols(char *path);

void load_map(const char *filepath, utl_vector **map);
void free_map(utl_vector **map);

