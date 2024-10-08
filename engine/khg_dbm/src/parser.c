#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "khg_dbm/util.h"
#include "khg_dbm/query.h"
#include "khg_dbm/parser.h"

void dbm_print_args(dbm_arg_node *args) {
  for (dbm_arg_node *it = args; it != NULL; it= it->next) {
    printf("%s: %s\n", it->key, it->value);
  }
}

void dbm_interactive(dbm_database **db, char *query) {
  dbm_token *query_tok = dbm_tokenize(query);
  dbm_parse_query(db, query_tok);
}

void dbm_parse_select(dbm_database **db, dbm_token *root) {
  dbm_arg_node *fields = dbm_parse_k_till(&root, "from");
  dbm_arg_node *table = dbm_parse_k_till(&root, "where");
  dbm_arg_node *conditions = dbm_parse_kv_till(&root, "\\EOF\\");
  if (table != NULL && fields != NULL && conditions != NULL) {
    dbm_query_node *results = dbm_exc_select(db, table, fields, conditions);
    if (results != NULL) {
      dbm_print_tbl_headers(db, table);
      dbm_print_query_nodes(results);
    } 
    else {
      printf("------\n");
    }
  }
}

void dbm_parse_delete(dbm_database **db, dbm_token *root) {
  dbm_token *expect_from = root->next;
  dbm_arg_node *table = NULL;
  dbm_arg_node *conditions = NULL;
  if (streq(expect_from->value, "from")) {
    table = dbm_parse_k_till(&expect_from, "where");
    conditions = dbm_parse_kv_till(&expect_from, "\\EOF\\");
  }
  if (table != NULL && conditions != NULL) {
    dbm_exc_delete(db, table, conditions);
  }
}

void dbm_parse_insert(dbm_database **db, dbm_token *root) {
  dbm_token *iter = root->next;
  dbm_arg_node *table = dbm_parse_k_till(&iter, "(");
  dbm_arg_node *values = dbm_parse_kv_till(&iter, ")");
  if (table != NULL && values != NULL) {
    dbm_exc_insert(db, table, values);
  }
}

void dbm_parse_update(dbm_database **db, dbm_token *root) {
  dbm_arg_node *table = dbm_parse_k_till(&root, "set");
  dbm_arg_node *updates = dbm_parse_kv_till(&root, "where");
  dbm_arg_node *conditions = dbm_parse_kv_till(&root, "\\EOF\\");
  if (table != NULL && updates != NULL && conditions != NULL) {
    dbm_exc_update(db, table, updates, conditions);
  }
}

void dbm_parse_create(dbm_database **db, dbm_token *root) {
  dbm_arg_node *table = dbm_parse_k_till(&root, "fields");
  dbm_arg_node *fields = dbm_parse_kv_till(&root, "\\EOF\\");
  if (table != NULL && fields != NULL) {
    dbm_exc_create(db, table, fields);
  }
}

void dbm_parse_drop(dbm_database **db, dbm_token *root) {
  dbm_arg_node *table = dbm_parse_k_till(&root, "\\EOF\\");
  if (table != NULL) {
    dbm_exc_drop(db, table);
  }
}

void dbm_parse_query(dbm_database **db, dbm_token *root) {
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

dbm_arg_node *dbm_add_arg(dbm_arg_node **parent, char *key, char *value) {
  dbm_arg_node *newArg = (dbm_arg_node *)malloc(sizeof(dbm_arg_node));
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

dbm_arg_node *dbm_parse_k_till(dbm_token **from, char *value) {
  dbm_arg_node *keys = NULL;
  dbm_arg_node *end = NULL;
  dbm_token *it = *from;
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

dbm_arg_node *dbm_parse_kv_till(dbm_token **from, char *value) {
  dbm_arg_node *keys = NULL;
  dbm_arg_node *end = NULL;
  dbm_token *it = *from;
  for (; it != NULL && !streq(it->value, value); it = it->next) {
    if (it->type == IDENTIFIER) {
      char *key = it->value;
      dbm_token *op = it->next;
      if (op != NULL && op->type == OPERATOR && streq(op->value, "=")) {
        dbm_token *tk_value = op->next;
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
