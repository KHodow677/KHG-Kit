#pragma once 

#include "khg_dbm/generics.h"
#include <sys/types.h>

typedef struct {
  char *types;
  dbm_generic **columns;
} row;

size_t dbm_rowlen(row **in_row);
row *dbm_create_row(char *types);

dbm_i dbm_r_get_type(row **in_row, int index);

int dbm_r_get_int(row **in_row, int index);
void dbm_r_set_int(row **in_row, int index, int value);

char dbm_r_get_char(row **in_row, int index);
void dbm_r_set_char(row **in_row, int index, char value);

char *dbm_r_get_str(row **in_row, int index);
void dbm_r_set_str(row **in_row, int index, char *value);

void dbm_print_row(row **in_row);

size_t dbm_pack_row(row **in_row, char **buf);
row* dbm_unpack_row(char *types, char **buf);

size_t dbm_get_size(row **in_row);
