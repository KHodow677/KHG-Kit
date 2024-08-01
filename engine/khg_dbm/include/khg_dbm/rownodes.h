#pragma once
#include "khg_dbm/rows.h"

typedef struct _rownode{
  row *data;
  struct _rownode *next;
} rownode;

rownode *create_rownode(row **row_val);

void add_node(rownode **parent_node, rownode **child_node);
rownode* add_row(rownode **parent_node, row **new_row);

void print_rownodes(rownode *root);

size_t pack_rownodes(rownode *root, char **buf);
void unpack_rownodes(char *types, rownode **root, char **buf, size_t buf_sz);
