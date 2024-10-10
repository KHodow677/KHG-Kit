#pragma once

#include "khg_dbm/btree.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct MiniDbHeader {
  size_t data_size;
  int64_t row_count;
  int64_t free_count;
} MiniDbHeader;

typedef struct MiniDbIndex {
  BTree search;
  BTree freelist;
  FILE *fd;
} MiniDbIndex;

typedef struct MiniDb {
  MiniDbHeader header;
  MiniDbIndex index;
  FILE *fd;
} MiniDb;

typedef struct MiniDbInfo {
  size_t data_size;
  int64_t row_count;
  int64_t free_count;
} MiniDbInfo;

typedef enum MiniDbState {
  MINIDB_OK,
  MINIDB_ERROR,
  MINIDB_ERROR_MALLOC_FAIL,
  MINIDB_ERROR_CANNOT_OPEN_FILE,
  MINIDB_ERROR_NULL_POINTER,
  MINIDB_ERROR_ROW_NOT_FOUND,
  MINIDB_ERROR_DUPLICATED_KEY_VIOLATION,
} MiniDbState;

const char *minidb_error_get_str(MiniDbState value);

MiniDbState minidb_create(MiniDb **db, const char *path, size_t data_size);
MiniDbState minidb_open(MiniDb **db, const char *path);
void minidb_close(MiniDb **db);

void minidb_get_info(const MiniDb *db, MiniDbInfo *result);

MiniDbState minidb_select(const MiniDb *db, int64_t key, void *result);
MiniDbState minidb_select_all(const MiniDb *db, void (*callback)(int64_t, void *));

MiniDbState minidb_insert(MiniDb *db, int64_t key, void *data);
MiniDbState minidb_update(MiniDb *db, int64_t key, void *data);
MiniDbState minidb_delete(MiniDb *db, int64_t key);
