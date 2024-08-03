#pragma once

#include "khg_dbm/row_nodes.h"
#include "khg_dbm/parser.h"
#include "khg_dbm/database.h"

typedef struct dbm_query_node {
  dbm_row_node *data;
  struct dbm_query_node *next;
} dbm_query_node;

dbm_query_node *dbm_add_query_node(dbm_query_node **parent, dbm_row_node **child);

table *dbm_get_table(dbm_database **db, dbm_arg_node *arg_table);

dbm_query_node *dbm_exc_select(dbm_database **db, dbm_arg_node *table, dbm_arg_node *fields, dbm_arg_node *condition);
size_t dbm_exc_delete(dbm_database **db, dbm_arg_node *table, dbm_arg_node *condition);
void dbm_exc_insert(dbm_database **db, dbm_arg_node *table, dbm_arg_node *values);
size_t dbm_exc_update(dbm_database **db, dbm_arg_node *table, dbm_arg_node *updates, dbm_arg_node *condition);
size_t dbm_exc_create(dbm_database **db, dbm_arg_node *table, dbm_arg_node *fields);
size_t dbm_exc_drop(dbm_database **db, dbm_arg_node *table);

void dbm_print_query_nodes(dbm_query_node *root);
void dbm_print_tbl_headers(dbm_database **db, dbm_arg_node *table);

