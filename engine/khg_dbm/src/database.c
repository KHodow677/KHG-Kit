#include "khg_dbm/util.h"
#include "khg_dbm/database.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

dbm_database *dbm_create_database(char *name) {
  dbm_database *new_db = (dbm_database *)malloc(sizeof(dbm_database));
  mstrcpy(&new_db->name, &name);
  new_db->root = NULL;
  new_db->end = NULL;
  return new_db;
}

size_t dbm_pack_db(dbm_database **db, char **buf) {
  return pack_string(*buf, (*db)->name);
}

size_t dbm_unpack_db(dbm_database **db, char *buf) {
  char *tmp_name;
  size_t read = unpack_string(buf, &tmp_name);
  *db = dbm_create_database(tmp_name);
  return read;
}

dbm_tablenode *dbm_add_tablenode(dbm_tablenode **parent, table **table) {
  dbm_tablenode *new_node = (dbm_tablenode *)malloc(sizeof(dbm_tablenode));
  new_node->name = (*table)->name;
  new_node->table = *table;
  new_node->next = NULL;
  if (*parent == NULL) {
    *parent = new_node;
  } else {
    (*parent)->next = new_node;
  }
  return new_node;
}

off_t dbm_write_tables(dbm_database **db, FILE *fp) {
  off_t pos = 0;
  size_t size = 0;
  char *buf;
  for (dbm_tablenode* it = (*db)->root; it != NULL; it = it->next) {
    if (it->table != NULL){
      size = pack_table(&it->table, it->table->row_begin, &buf);
      pos += size;
      write_buffer(fp, buf, size);
      free(buf);
    }
  }
  return pos;
}

off_t dbm_read_tables(dbm_database **db, FILE *fp) {
  off_t pos = 0;
  size_t size = 0;
  char *buf;
  while (fread(&size, sizeof(size), 1, fp) > 0) {
    pos += sizeof(size);
    buf = malloc(size);
    if (fread(buf, sizeof(char), size, fp) > 0) {
      table *new_table = unpack_table(&buf);
      free(buf);
      dbm_db_insert_table(db, &new_table);
      pos+=size;
    }
  }
  return pos;
}

void dbm_read_rows(dbm_database **db, FILE *fp) {
  for(dbm_tablenode *it = (*db)->root; it != NULL; it = it->next) {
    size_t read_begin = it->table->row_begin;
    size_t read_end = 0;
    if (it->next == NULL) {
      read_end = get_file_size(fp);
    } 
    else {
      read_end = it->next->table->row_begin;
    }
    if (fseek(fp, read_begin, SEEK_SET) != -1) {
      char *buf;
      size_t buf_sz = read_buffer(fp, &buf, read_end - read_begin);
      dbm_unpack_row_nodes(it->table->field_types, &it->table->root, &buf, buf_sz);
      for (dbm_row_node* i = it->table->root; i != NULL; i = i->next) {
        it->table->end = i;
      }
      free(buf);
    }
  }
}

off_t dbm_write_rows(dbm_database **db, FILE *fp) {
  off_t pos = 0;
  size_t size = 0;
  char *buf;
  for (dbm_tablenode *it = (*db)->root; it != NULL; it = it->next) {
    if (it->table!= NULL) {
      size = dbm_pack_row_nodes(it->table->root, &buf);
      it->table->row_begin = pos;
      pos+= size;
      write_buffer(fp, buf, size);
      free(buf);
    }
  }
  return pos;
}

void dbm_write_files(dbm_database **db) {
  char file_name[255];
  snprintf(file_name, 255, "%s.db",(*db)->name);
  FILE *binary_file = fopen(file_name, "wb");
  dbm_write_rows(db, binary_file);
  fclose(binary_file);
  snprintf(file_name, 255, "%s.meta",(*db)->name);
  FILE *meta_file = fopen(file_name, "wb");
  dbm_write_tables(db, meta_file);
  fclose(meta_file);
}

void dbm_read_files(dbm_database **db) {
  char file_name[255];
  snprintf(file_name, 255, "%s.meta",(*db)->name);
  FILE *meta_file = fopen(file_name, "rb");
  dbm_read_tables(db, meta_file);
  fclose(meta_file);
  snprintf(file_name, 255, "%s.db",(*db)->name);
  FILE *binary_file = fopen(file_name, "rb");
  dbm_read_rows(db, binary_file);
  fclose(binary_file);
}

void dbm_db_insert_table(dbm_database **db, table **table) {
  if ((*db)->end != NULL) {
    (*db)->end = dbm_add_tablenode(&(*db)->end, table);
  } 
  else {
    (*db)->end = dbm_add_tablenode(&(*db)->root, table);
  }
}

