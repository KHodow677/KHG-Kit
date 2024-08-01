#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "khg_dbm/util.h"
#include "khg_dbm/query.h"
#include "khg_dbm/parser.h"

void dbm_print_args(ArgNode *args) {
  for (ArgNode *it = args; it != NULL; it= it->next) {
    printf("%s: %s\n", it->key, it->value);
  }
}

void dbm_interactive(database **db, char *query) {
  Token *query_tok = tokenize(query);
  dbm_parse_query(db, query_tok);
}

void dbm_parse_select(database **db, Token *root) {
  ArgNode *fields = dbm_parse_k_till(&root, "from");
  ArgNode *table = dbm_parse_k_till(&root, "where");
  ArgNode *conditions = dbm_parse_kv_till(&root, "\\EOF\\");
  if (table != NULL && fields != NULL && conditions != NULL) {
    QueryNode *results = exc_select(db, table, fields, conditions);
    if (results != NULL) {
      print_tbl_headers(db, table);
      print_queryNodes(results);
    } 
    else {
      printf("------");
    }
  }
}

void dbm_parse_delete(database **db, Token *root) {
  Token *expect_from = root->next;
  ArgNode *table = NULL;
  ArgNode *conditions = NULL;
  if (streq(expect_from->value, "from")) {
    table = dbm_parse_k_till(&expect_from, "where");
    conditions = dbm_parse_kv_till(&expect_from, "\\EOF\\");
  }
  if (table != NULL && conditions != NULL) {
    exc_delete(db, table, conditions);
  }
}

void dbm_parse_insert(database **db, Token *root) {
  Token *iter = root->next;
  ArgNode *table = dbm_parse_k_till(&iter, "(");
  ArgNode *values = dbm_parse_kv_till(&iter, ")");
  if (table != NULL && values != NULL) {
    exc_insert(db, table, values);
  }
}

void dbm_parse_update(database **db, Token *root) {
  ArgNode *table = dbm_parse_k_till(&root, "set");
  ArgNode *updates = dbm_parse_kv_till(&root, "where");
  ArgNode *conditions = dbm_parse_kv_till(&root, "\\EOF\\");
  if (table != NULL && updates != NULL && conditions != NULL) {
    exc_update(db, table, updates, conditions);
  }
}

void dbm_parse_create(database **db, Token *root) {
  ArgNode *table = dbm_parse_k_till(&root, "fields");
  ArgNode *fields = dbm_parse_kv_till(&root, "\\EOF\\");
  if (table != NULL && fields != NULL) {
    exc_create(db, table, fields);
  }
}

void dbm_parse_drop(database **db, Token *root) {
  ArgNode *table = dbm_parse_k_till(&root, "\\EOF\\");
  if (table != NULL) {
    exc_drop(db, table);
  }
}

void dbm_parse_query(database **db, Token *root) {
  if(root->type == KEYWORD) {
    if (streq(root->value, "select")) {
      dbm_parse_select(db, root);
    }
    if (streq(root->value, "delete")) {
      return dbm_parse_delete(db, root);
    }
    if (streq(root->value, "update")) {
      return dbm_parse_update(db, root);
    }
    if (streq(root->value, "insert")) {
      return dbm_parse_insert(db, root);
    }
    if (streq(root->value, "create")) {
      return dbm_parse_create(db, root);
    }
    if (streq(root->value, "drop")) {
      return dbm_parse_drop(db, root);
    }
  }
}

ArgNode *dbm_add_arg(ArgNode **parent, char *key, char *value) {
  ArgNode *newArg = (ArgNode *)malloc(sizeof(ArgNode));
  mstrcpy(&newArg->key, &key);
  mstrcpy(&newArg->value, &value);
  if (*parent == NULL) {
    *parent = newArg;
  } 
  else {
    (*parent)->next = newArg;
  }
  newArg->next = NULL;
  return newArg;
}

ArgNode *dbm_parse_k_till(Token **from, char *value) {
  ArgNode *keys = NULL;
  ArgNode *end = NULL;
  Token *it = *from;
  for (; it != NULL && !streq(it->value, value); it = it->next) {
    if (it->type == IDENTIFIER) {
      if (keys == NULL) {
        end = dbm_add_arg(&keys, it->value, "");
      } 
      else {
        end = dbm_add_arg(&end, it->value, "");
      }
    }
  }
  *from = it;
  return keys;
}

ArgNode *dbm_parse_kv_till(Token **from, char *value) {
  ArgNode *keys = NULL;
  ArgNode *end = NULL;
  Token *it = *from;
  for (; it != NULL && !streq(it->value, value); it = it->next) {
    if (it->type == IDENTIFIER) {
      char *key = it->value;
      Token *op = it->next;
      if (op != NULL && op->type == OPERATOR && streq(op->value, "=")) {
        Token *tk_value = op->next;
        if (tk_value->type == VALUE) {
          it = tk_value;
          if (keys == NULL) {
            end = dbm_add_arg(&keys, key, tk_value->value);
          } 
          else {
            end = dbm_add_arg(&end,  key, tk_value->value);
          }
        }
        if (tk_value->type == KEYWORD) {
          it = tk_value;
          if (streq(tk_value->value, "int") || streq(tk_value->value, "char") || streq(tk_value->value, "str")) {
            if (keys == NULL) {
              end = dbm_add_arg(&keys, key, tk_value->value);
            }
            else {
              end = dbm_add_arg(&end,  key, tk_value->value);
            }
          }
        }
      }
    }
  }
  *from = it;
  return keys;
}
