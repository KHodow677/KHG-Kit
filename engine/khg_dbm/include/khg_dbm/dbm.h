#pragma once

#include "khg_dbm/btree.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct dbm_headers {
  size_t data_size;
  int64_t row_count;
  int64_t free_count;
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
  int64_t row_count;
  int64_t free_count;
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

const char *minidb_error_get_str(dbm_db_state value);

dbm_db_state minidb_create(dbm_db **db, const char *path, size_t data_size);
dbm_db_state minidb_open(dbm_db **db, const char *path);
void minidb_close(dbm_db **db);

void minidb_get_info(const dbm_db *db, dbm_db_info *result);

dbm_db_state minidb_select(const dbm_db *db, int64_t key, void *result);
dbm_db_state minidb_select_all(const dbm_db *db, void (*callback)(int64_t, void *));

dbm_db_state minidb_insert(dbm_db *db, int64_t key, void *data);
dbm_db_state minidb_update(dbm_db *db, int64_t key, void *data);
dbm_db_state minidb_delete(dbm_db *db, int64_t key);
