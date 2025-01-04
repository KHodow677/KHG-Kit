#pragma once

#include "khg_dbm/btree.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct dbm_headers {
  size_t data_size;
  long row_count;
  long free_count;
} dbm_headers;

typedef struct dbm_index {
  dbm_btree search;
  dbm_btree freelist;
  FILE *fd;
} dbm_index;

typedef struct dbm_db {
  dbm_headers header;
  dbm_index index;
  FILE *fd;
} dbm_db;

typedef struct dbm_db_info {
  size_t data_size;
  long row_count;
  long free_count;
} dbm_db_info;

typedef enum dbm_db_state {
  DBM_OK,
  DBM_ERROR,
  DBM_ERROR_MALLOC_FAIL,
  DBM_ERROR_CANNOT_OPEN_FILE,
  DBM_ERROR_NULL_POINTER,
  DBM_ERROR_ROW_NOT_FOUND,
  DBM_ERROR_DUPLICATED_KEY_VIOLATION,
} dbm_db_state;

const char *dbm_error_get_str(dbm_db_state value);

dbm_db_state dbm_create(dbm_db **db, const char *path, size_t data_size);
dbm_db_state dbm_open(dbm_db **db, const char *path);
void dbm_close(dbm_db **db);

void dbm_get_info(const dbm_db *db, dbm_db_info *result);

dbm_db_state dbm_select(const dbm_db *db, long key, void *result);
dbm_db_state dbm_select_all(const dbm_db *db, void (*callback)(long, void *));

dbm_db_state dbm_insert(dbm_db *db, long key, void *data);
dbm_db_state dbm_update(dbm_db *db, long key, void *data);
dbm_db_state dbm_delete(dbm_db *db, long key);

