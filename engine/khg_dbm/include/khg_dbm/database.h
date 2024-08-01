#pragma once

#include "khg_dbm/tables.h"
#include <stdio.h>

typedef struct _tablenode{
  char *name;
  table *table;
  struct _tablenode *next;
} tablenode;

typedef struct _database{
  char *name;
  tablenode *root;
  tablenode *end;
} database;

database *dbm_create_database(char *name);
size_t dbm_pack_db(database **db, char **buf);
size_t dbm_unpack_db(database **db, char *buf);
tablenode *dbm_add_tablenode(tablenode **parent, table **table);
off_t dbm_write_tables(database **db, FILE *fp);
off_t dbm_read_tables(database **db, FILE *fp);
void dbm_read_rows(database **db, FILE *fp);
off_t dbm_write_rows(database **db, FILE *fp);
void dbm_write_files(database **db);
void dbm_read_files(database **db);
void dbm_db_insert_table(database **db, table **table);
