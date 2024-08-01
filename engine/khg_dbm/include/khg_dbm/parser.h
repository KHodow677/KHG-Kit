#pragma once

#include "khg_dbm/lexer.h"
#include "khg_dbm/database.h"

typedef struct _ArgNode {
  char *key;
  char *value;
  struct _ArgNode *next;
} ArgNode;

void print_args(ArgNode *args);
void parse_select(database **db, Token *root);
void parse_delete(database **db, Token *root);
void parse_insert(database **db, Token *root);
void parse_update(database **db, Token *root);
void parse_create(database **db, Token *root);
void parse_drop(database **db, Token *root);
void parse_query(database **db, Token *root);
void interactive(database **db, char *query);
ArgNode* add_arg(ArgNode **parent, char *key, char *value);
ArgNode* parse_k_till(Token **from, char *value);
ArgNode* parse_kv_till(Token **from, char *value);
