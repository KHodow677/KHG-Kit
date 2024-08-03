#pragma once

#include "khg_dbm/lexer.h"
#include "khg_dbm/database.h"

typedef struct dbm_arg_node {
  char *key;
  char *value;
  struct dbm_arg_node *next;
} dbm_arg_node;

void dbm_print_args(dbm_arg_node *args);
void dbm_parse_select(dbm_database **db, dbm_token *root);
void dbm_parse_delete(dbm_database **db, dbm_token *root);
void dbm_parse_insert(dbm_database **db, dbm_token *root);
void dbm_parse_update(dbm_database **db, dbm_token *root);
void dbm_parse_create(dbm_database **db, dbm_token *root);
void dbm_parse_drop(dbm_database **db, dbm_token *root);
void dbm_parse_query(dbm_database **db, dbm_token *root);
void dbm_interactive(dbm_database **db, char *query);
dbm_arg_node *dbm_add_arg(dbm_arg_node **parent, char *key, char *value);
dbm_arg_node *dbm_parse_k_till(dbm_token **from, char *value);
dbm_arg_node *dbm_parse_kv_till(dbm_token **from, char *value);

