#include "khg_dbm/util.h"
#include "khg_dbm/tables.h"
#include <string.h>
#include <stdlib.h>

table *create_table(char *name, char ***field_names, char **field_types) {
  table *new_table = (table *)malloc(sizeof(table));
  new_table->name = name;
  new_table->field_names = *field_names;
  new_table->field_types = *field_types;
  new_table->root = NULL;
  new_table->end = NULL;
  new_table->row_begin = 0;
  return new_table;
}

dbm_row_node *get_rows(table **in_table) {
  return (*in_table)->root;
}

void tbl_insert(table **in_table, row **new_row) {
  if((*in_table)->end == NULL){
    dbm_row_node *new_rownode = dbm_create_row_node(new_row);
    (*in_table)->root = new_rownode;
    (*in_table)->end = new_rownode;
  } 
  else {
    (*in_table)->end = dbm_add_row(&(*in_table)->end, new_row);
  }
}

size_t table_size(table **in_table) {
  size_t ret_size = 0;
  for(int i = 0; i < strlen((*in_table)->field_types); i++) {
    ret_size += sizeof(size_t) + (strlen((*in_table)->field_names[i]) + 1) * sizeof(char);
  }
  ret_size += sizeof(size_t) + (strlen((*in_table)->name) + 1) * sizeof(char);
  ret_size += sizeof(size_t) + (strlen((*in_table)->field_types) + 1) * sizeof(char);
  ret_size += sizeof(size_t);
  ret_size += sizeof(size_t);
  return ret_size;
}

size_t pack_table(table **in_table, size_t row_addr, char **buf) {
  size_t tbl_size = table_size(in_table);
  *buf = (char *)malloc(tbl_size);
  off_t pos = 0;
  memcpy(*buf + pos, &tbl_size, sizeof(tbl_size));
  pos+= sizeof(tbl_size);
  pos+= pack_string(*buf + pos, (*in_table)->name);
  pos+= pack_string(*buf + pos, (*in_table)->field_types);
  memcpy(*buf + pos, &row_addr, sizeof(row_addr));
  pos+=sizeof(row_addr);
  for(int i = 0; i < strlen((*in_table)->field_types); i++) {
    pos+= pack_string(*buf + pos, (*in_table)->field_names[i]);
  }
  return pos;
}

table *unpack_table(char **buf) {
  table *new_table = malloc(sizeof(table));
  off_t pos = 0;
  pos += unpack_string(*buf + pos, &new_table->name);
  pos += unpack_string(*buf + pos, &new_table->field_types);
  memcpy(&new_table->row_begin, *buf + pos, sizeof(size_t));
  pos += sizeof(size_t);
  new_table->field_names = (char **)malloc(sizeof(char *) * strlen(new_table->field_types));
  for(int i = 0; i < strlen(new_table->field_types); i++) {
    pos += unpack_string(*buf + pos, &new_table->field_names[i]);
  }
  return new_table;
}

