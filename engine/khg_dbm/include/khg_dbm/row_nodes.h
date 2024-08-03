#pragma once
#include "khg_dbm/rows.h"

typedef struct dbm_row_node{
  row *data;
  struct dbm_row_node *next;
} dbm_row_node;

dbm_row_node *dbm_create_row_node(row **row_val);

void dbm_add_node(dbm_row_node **parent_node, dbm_row_node **child_node);
dbm_row_node* dbm_add_row(dbm_row_node **parent_node, row **new_row);

void dbm_print_row_nodes(dbm_row_node *root);

size_t dbm_pack_row_nodes(dbm_row_node *root, char **buf);
void dbm_unpack_row_nodes(char *types, dbm_row_node **root, char **buf, size_t buf_sz);

