#pragma once

#include "khg_dbm/rownodes.h"

typedef struct{
    char* name;
    char** field_names;
    char* field_types;
    rownode* root;
    rownode* end;
    size_t row_begin;
} table;

table* create_table(char* name, char*** field_names, char** field_types);
rownode* get_rows(table** in_table);
void tbl_insert(table** in_table, row** new_row);

size_t pack_table(table** in_table, size_t row_addr, char** buf);
table* unpack_table(char** buf);
size_t table_size(table** in_table);
