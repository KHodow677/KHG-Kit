#pragma once

#include "khg_dbm/lexer.h"
#include "khg_dbm/database.h"

typedef struct _ArgNode {
  char *key;
  char *value;
  struct _ArgNode *next;
} ArgNode;

void dbm_print_args(ArgNode *args);
void dbm_parse_select(database **db, Token *root);
void dbm_parse_delete(database **db, Token *root);
void dbm_parse_insert(database **db, Token *root);
void dbm_parse_update(database **db, Token *root);
void dbm_parse_create(database **db, Token *root);
void dbm_parse_drop(database **db, Token *root);
void dbm_parse_query(database **db, Token *root);
void dbm_interactive(database **db, char *query);
ArgNode *dbm_add_arg(ArgNode **parent, char *key, char *value);
ArgNode *dbm_parse_k_till(Token **from, char *value);
ArgNode *dbm_parse_kv_till(Token **from, char *value);
