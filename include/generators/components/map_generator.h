#pragma once

#define MAX_PATH 260

int get_csv_rows(char *path);
int get_csv_cols(char *path);

void load_map(const char *filepath);
void free_map(void);

