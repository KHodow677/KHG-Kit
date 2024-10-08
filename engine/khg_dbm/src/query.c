#include "khg_dbm/util.h"
#include "khg_dbm/database.h"
#include "khg_dbm/query.h"
#include <stdlib.h>
#include <string.h>

dbm_query_node *dbm_add_query_node(dbm_query_node **parent, dbm_row_node **child) {
  dbm_query_node *new_node = (dbm_query_node *)malloc(sizeof(dbm_query_node));
  new_node->data = *child;
  new_node->next = NULL;
  if (*parent == NULL) {
    *parent = new_node;
  } 
  else {
    (*parent)->next = new_node;
  }
  return new_node;
}

table *dbm_get_table(dbm_database **db, dbm_arg_node *arg_table) {
  if (arg_table != NULL) {
    for (dbm_tablenode *it = (*db)->root; it != NULL; it = it->next) {
      if (streq(it->name, arg_table->key) && it->table != NULL) {
        return it->table;
      }
    }
  }
  return NULL;
}

int map_name_to_col(table **table, char *col) {
  for (int i = 0; i < strlen((*table)->field_types); i++) {
    if (streq((*table)->field_names[i], col)) {
      return i;
    }
  }
  return -1;
}

dbm_query_node *dbm_exc_select(dbm_database **db, dbm_arg_node *arg_table, dbm_arg_node *fields, dbm_arg_node *condition) {
  table *in_table = dbm_get_table(db, arg_table);
  dbm_query_node *results = NULL;
  dbm_query_node *last = NULL;
  int col = map_name_to_col(&in_table, condition->key);
  dbm_i type = dbm_get_type(in_table->field_types[col]);
  dbm_generic *cond = (dbm_generic *)malloc(sizeof(dbm_generic));
  dbm_create_generic_str(&cond, type, condition->value);
  if (in_table == NULL) {
    return NULL;
  }
  for (dbm_row_node* it = in_table->root; it != NULL; it = it->next) {
    if (it->data != NULL) {
      if(dbm_geneq(it->data->columns[col], cond)) {
        if(last == NULL) {
          last = dbm_add_query_node(&results, &it);
        } 
        else {
          last = dbm_add_query_node(&last, &it);
        }
      }
    }
  }
  return results;
}

size_t create_cols(dbm_arg_node *fields, char ***field_names, char **out_field_types) {
  char *field_types = (char *)malloc(255);
  off_t pos = 0;
  for (dbm_arg_node *it = fields; it != NULL; it = it->next) {
    if (streq(it->value, "int")) {
      field_types[pos] = 'i';
      pos++;
    }
    if (streq(it->value, "char")) {
      field_types[pos] = 'c';
      pos++;
    }
    if (streq(it->value, "str")) {
      field_types[pos] = 's';
      pos++;
    }
  }
  field_types[pos] = '\0';
  field_types = realloc(field_types, pos);
  *out_field_types = field_types;
  *field_names = (char **)malloc(sizeof(char*) * pos);
  pos = 0;
  for (dbm_arg_node *it = fields; it != NULL; it = it->next) {
    (*field_names)[pos] = it->key;
    pos++;
  }
  return pos;
}

size_t dbm_exc_create(dbm_database **db, dbm_arg_node *arg_table, dbm_arg_node *fields) {
  char *table_name = arg_table->key;
  char **field_names;
  char *field_types;
  create_cols(fields, &field_names, &field_types);
  table *new_table = create_table(table_name, &field_names, &field_types);
  dbm_db_insert_table(db, &new_table);
  return 1;
}

size_t dbm_exc_update(dbm_database **db, dbm_arg_node *arg_table, dbm_arg_node *updates, dbm_arg_node *condition) {
  table *in_table = dbm_get_table(db, arg_table);
  if (in_table == NULL) {
    printf("Table doesn't exist\n");
    return 0;
  }
  dbm_query_node *results = dbm_exc_select(db, arg_table, NULL, condition);
  size_t pos = 0;
  for (dbm_query_node *it = results; it != NULL; it = it->next) {
    for (dbm_arg_node *arg = updates; arg != NULL; arg = arg->next) {
      int col = map_name_to_col(&in_table, arg->key);
      switch (dbm_r_get_type(&it->data->data, col)) {
        case int_t:
          dbm_r_set_int(&it->data->data, col, atoi(arg->value));
          break;
        case str_t:
          dbm_r_set_str(&it->data->data, col, arg->value);
          break;
        case char_t:
          dbm_r_set_char(&it->data->data, col, arg->value[0]);
          break;
        default:
          break;
      }
    }
    pos++;
  }
  return pos;
}

size_t dbm_exc_delete(dbm_database **db, dbm_arg_node *arg_table, dbm_arg_node *condition) {
  dbm_query_node *results = dbm_exc_select(db, arg_table, NULL, condition);
  size_t pos = 0;
  for (dbm_query_node *it = results; it != NULL; it = it->next){
    free(it->data->data);
    it->data->data = NULL;
    pos++;
  }
  return pos;
}

void dbm_exc_insert(dbm_database **db, dbm_arg_node *arg_table, dbm_arg_node *values) {
  table *in_table = dbm_get_table(db, arg_table);
  if (in_table == NULL) {
    printf("Table doesn't exist\n");
  }
  if (in_table != NULL) {
    row *new_row = dbm_create_row(in_table->field_types);
    for (dbm_arg_node *arg = values; arg != NULL; arg = arg->next){
      int col = map_name_to_col(&in_table, arg->key);
      switch (dbm_r_get_type(&new_row, col)) {
        case int_t:
          dbm_r_set_int(&new_row, col, atoi(arg->value));
          break;
        case str_t:
          dbm_r_set_str(&new_row, col, arg->value);
          break;
        case char_t:
          dbm_r_set_char(&new_row, col, arg->value[0]);
          break;
        default:
          break;
      }
    }
    tbl_insert(&in_table, &new_row);
  }
}

size_t dbm_exc_drop(dbm_database **db, dbm_arg_node *arg_table) {
  if(arg_table != NULL) {
    for(dbm_tablenode* it = (*db)->root; it != NULL; it = it->next) {
      if(streq(it->name, arg_table->key)) {
        free(it->table);
        it->table = NULL;
      }
    }
  }
  return 0;
}

void dbm_print_query_nodes(dbm_query_node *root) {
  if(root != NULL) {
    for(dbm_query_node *it = root; it != NULL; it = it->next) {
      dbm_print_row(&it->data->data);
    }
  }
}

void dbm_print_tbl_headers(dbm_database **db, dbm_arg_node *arg_table) {
  table *in_table = dbm_get_table(db, arg_table);
  for(size_t i =0; i < strlen(in_table->field_types); i++) {
    printf("%s\t", in_table->field_names[i]);
  }
  printf("\n");
}
