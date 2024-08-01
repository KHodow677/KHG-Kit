#pragma once 

#include "khg_dbm/generics.h"
#include <stdlib.h>

typedef struct {
  char *types;
  generic **columns;
} row;

row *create_row(char *types);

size_t rowlen(row **in_row);

i_type r_get_type(row **in_row, int index);

int r_get_int(row **in_row, int index);
void r_set_int(row **in_row, int index, int value);

char r_get_char(row **in_row, int index);
void r_set_char(row **in_row, int index, char value);

char *r_get_str(row **in_row, int index);
void r_set_str(row **in_row, int index, char *value);

void print_row(row **in_row);

size_t pack_row(row **in_row, char **buf);
row* unpack_row(char *types, char **buf);

size_t get_size(row **in_row);
