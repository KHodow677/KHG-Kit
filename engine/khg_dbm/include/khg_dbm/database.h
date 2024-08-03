#pragma once

#include "khg_dbm/tables.h"
#include <stdio.h>

typedef struct _tablenode{
  char *name;
  table *table;
  struct _tablenode *next;
} dbm_tablenode;

typedef struct _database{
  char *name;
  dbm_tablenode *root;
  dbm_tablenode *end;
} dbm_database;

dbm_database *dbm_create_database(char *name);
size_t dbm_pack_db(dbm_database **db, char **buf);
size_t dbm_unpack_db(dbm_database **db, char *buf);
dbm_tablenode *dbm_add_tablenode(dbm_tablenode **parent, table **table);
off_t dbm_write_tables(dbm_database **db, FILE *fp);
off_t dbm_read_tables(dbm_database **db, FILE *fp);
void dbm_read_rows(dbm_database **db, FILE *fp);
off_t dbm_write_rows(dbm_database **db, FILE *fp);
void dbm_write_files(dbm_database **db);
void dbm_read_files(dbm_database **db);
void dbm_db_insert_table(dbm_database **db, table **table);

