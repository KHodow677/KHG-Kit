#include "khg_dbm/util.h"
#include "khg_dbm/database.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

database *create_database(char *name) {
  database *new_db = (database *)malloc(sizeof(database));
  mstrcpy(&new_db->name, &name);
  new_db->root = NULL;
  new_db->end = NULL;
  return new_db;
}

size_t pack_db(database **db, char **buf) {
  return pack_string(*buf, (*db)->name);
}

size_t unpack_db(database **db, char *buf) {
  char *tmp_name;
  size_t read = unpack_string(buf, &tmp_name);
  *db = create_database(tmp_name);
  return read;
}

tablenode *add_tablenode(tablenode **parent, table **table) {
  tablenode *new_node = (tablenode *)malloc(sizeof(tablenode));
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

off_t write_tables(database **db, FILE *fp) {
  off_t pos = 0;
  size_t size = 0;
  char *buf;
  for (tablenode* it = (*db)->root; it != NULL; it = it->next) {
    if (it->table != NULL){
      size = pack_table(&it->table, it->table->row_begin, &buf);
      pos += size;
      write_buffer(fp, buf, size);
      free(buf);
    }
  }
  return pos;
}

off_t read_tables(database **db, FILE *fp) {
  off_t pos = 0;
  size_t size = 0;
  char *buf;
  while (fread(&size, sizeof(size), 1, fp) > 0) {
    pos += sizeof(size);
    buf = malloc(size);
    if (fread(buf, sizeof(char), size, fp) > 0) {
      table *new_table = unpack_table(&buf);
      free(buf);
      db_insert_table(db, &new_table);
      pos+=size;
    }
  }
  return pos;
}

void read_rows(database **db, FILE *fp) {
  for(tablenode *it = (*db)->root; it != NULL; it = it->next) {
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
      unpack_rownodes(it->table->field_types, &it->table->root, &buf, buf_sz);
      for (rownode* i = it->table->root; i != NULL; i = i->next) {
        it->table->end = i;
      }
      free(buf);
    }
  }
}

off_t write_rows(database **db, FILE *fp) {
  off_t pos = 0;
  size_t size = 0;
  char *buf;
  for (tablenode *it = (*db)->root; it != NULL; it = it->next) {
    if (it->table!= NULL) {
      size = pack_rownodes(it->table->root, &buf);
      it->table->row_begin = pos;
      pos+= size;
      write_buffer(fp, buf, size);
      free(buf);
    }
  }
  return pos;
}

void write_files(database **db) {
  char file_name[255];
  snprintf(file_name, 255, "%s.db",(*db)->name);
  FILE *binary_file = fopen(file_name, "wb");
  write_rows(db, binary_file);
  fclose(binary_file);
  snprintf(file_name, 255, "%s.meta",(*db)->name);
  FILE *meta_file = fopen(file_name, "wb");
  write_tables(db, meta_file);
  fclose(meta_file);
}

void read_files(database **db) {
  char file_name[255];
  snprintf(file_name, 255, "%s.meta",(*db)->name);
  FILE *meta_file = fopen(file_name, "rb");
  read_tables(db, meta_file);
  fclose(meta_file);
  snprintf(file_name, 255, "%s.db",(*db)->name);
  FILE *binary_file = fopen(file_name, "rb");
  read_rows(db, binary_file);
  fclose(binary_file);
}

void db_insert_table(database **db, table **table) {
  if ((*db)->end != NULL) {
    (*db)->end = add_tablenode(&(*db)->end, table);
  } 
  else {
    (*db)->end = add_tablenode(&(*db)->root, table);
  }
}
