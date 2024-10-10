#include "khg_dbm/dbm.h"
#include "khg_dbm/index.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DBM_INDEX_SUFFIX "-index"
#define DBM_RETURN_CASE_AS_STRING(caseval) case caseval: return #caseval

const char *minidb_error_get_str(dbm_db_state value) {
  switch (value) {
    DBM_RETURN_CASE_AS_STRING(DBM_OK);
    DBM_RETURN_CASE_AS_STRING(DBM_ERROR);
    DBM_RETURN_CASE_AS_STRING(DBM_ERROR_ROW_NOT_FOUND);
    DBM_RETURN_CASE_AS_STRING(DBM_ERROR_DUPLICATED_KEY_VIOLATION);
    default: assert (0);
  }
}

static void minidb_build_index_file_path(const char *base_path, char *output, size_t output_size) {
  size_t len = strlen(base_path);
  size_t total_len = len + sizeof(DBM_INDEX_SUFFIX) - 1;
  if (total_len >= output_size) {
    total_len = output_size - 1;
  }
  memcpy(output, base_path, len);
  memcpy(output + len, DBM_INDEX_SUFFIX, sizeof(DBM_INDEX_SUFFIX) - 1);
  output[total_len] = '\0';
}

static void minidb_header_write(const dbm_db *mini) {
  fseek(mini->fd, 0, SEEK_SET);
  fwrite(&mini->header, sizeof(dbm_headers), 1, mini->fd);
  fflush(mini->fd);
}

static void minidb_initialize_empty(dbm_db *mini) {
  mini->header.data_size = UINT64_C(0);
  mini->header.row_count = INT64_C(0);
  mini->header.free_count = INT64_C(0);
  minidb_index_init(&mini->index);
}

dbm_db_state minidb_create(dbm_db **db, const char *path, size_t data_size) {
  *db = NULL;
  FILE *fd = fopen(path, "w+");
  if (!fd) {
    return DBM_ERROR_CANNOT_OPEN_FILE;
  }
  dbm_db *new_db = malloc(sizeof(dbm_db));
  if (!new_db) {
    fclose(fd);
    return DBM_ERROR_MALLOC_FAIL;
  }
  minidb_initialize_empty(new_db);
  new_db->header.data_size = data_size;
  new_db->fd = fd;
  char index_path[1024];
  minidb_build_index_file_path(path, index_path, sizeof(index_path));
  dbm_db_state state = minidb_index_open(&new_db->index, index_path, new_db->header.row_count, new_db->header.free_count);
  if (state != DBM_OK) {
    fclose(fd);
    free(new_db);
    return state;
  }
  minidb_header_write(new_db);
  *db = new_db;
  return DBM_OK;
}

dbm_db_state minidb_open(dbm_db **db, const char *path) {
  *db = NULL;
  FILE *fd = fopen(path, "r+");
  if (!fd) {
    return DBM_ERROR_CANNOT_OPEN_FILE;
  }
  dbm_db *new_db = malloc(sizeof(dbm_db));
  if (!new_db) {
    fclose(fd);
    return DBM_ERROR_MALLOC_FAIL;
  }
  minidb_initialize_empty(new_db);
  new_db->fd = fd;
  fread(&new_db->header, sizeof(dbm_headers), 1, fd);
  char index_path[1024];
  minidb_build_index_file_path(path, index_path, sizeof(index_path));
  dbm_db_state state = minidb_index_open(&new_db->index, index_path, new_db->header.row_count, new_db->header.free_count);
  if (state != DBM_OK) {
    fclose(fd);
    free(new_db);
  }
  *db = new_db;
  return DBM_OK;
}

void minidb_close(dbm_db **db) {
  if (db) {
    dbm_db *new_db = *db;
    minidb_index_close(&new_db->index);
    minidb_header_write(new_db);
    fflush(new_db->fd);
    fclose(new_db->fd);
    free(new_db);
    *db = NULL;
  }
}

void minidb_get_info(const dbm_db *db, dbm_db_info *result) {
  result->data_size = db->header.data_size;
  result->row_count = db->header.row_count;
  result->free_count = db->header.free_count;
}

dbm_db_state minidb_select(const dbm_db *db, int64_t key, void *result) {
  dbm_btree_node *node = btree_search(&db->index.search, key);
  if (!node) {
    return DBM_ERROR_ROW_NOT_FOUND;
  }
  fseek(db->fd, node->value, SEEK_SET);
  fread(result, db->header.data_size, 1, db->fd);
  return DBM_OK;
}

static void minidb_index_traverse(const dbm_db *db, dbm_btree_node *current, void *result, void (*callback)(int64_t, void *)) {
  if (current) {
    minidb_index_traverse(db, current->left, result, callback);
    minidb_select(db, current->key, result);
    callback(current->key, result);
    minidb_index_traverse(db, current->right, result, callback);
  }
}

dbm_db_state minidb_select_all(const dbm_db *db, void (*callback)(int64_t, void *)) {
  void *result = malloc(db->header.data_size);
  if (!result) {
    return DBM_ERROR_MALLOC_FAIL;
  }
  minidb_index_traverse(db, db->index.search.root, result, callback);
  free(result);
  return DBM_OK;
}

static const dbm_btree_node *minidb_freelist_find_node(const dbm_db *db) {
  const dbm_btree_node *node = db->index.freelist.root;
  if (node) {
    while (node->left) {
      node = node->left;
    }
  }
  return node;
}

dbm_db_state minidb_insert(dbm_db *db, int64_t key, void *data) {
  if (btree_contains(&db->index.search, key)) {
    return DBM_ERROR_DUPLICATED_KEY_VIOLATION;
  }
  const dbm_btree_node *free_node = minidb_freelist_find_node(db);
  int64_t address;
  if (!free_node) {
    address = sizeof(dbm_headers) + db->header.data_size * db->header.row_count;
  } 
  else {
    address = free_node->value;
    btree_remove(&db->index.freelist, free_node->key, NULL);
    db->header.free_count--;
    assert(db->header.free_count == db->index.freelist.size);
  }
  fseek(db->fd, address, SEEK_SET);
  fwrite(data, db->header.data_size, 1, db->fd);
  db->header.row_count++;
  btree_insert(&db->index.search, key, address);
  minidb_header_write(db);
  minidb_index_write(&db->index);
  return DBM_OK;
}

dbm_db_state minidb_update(dbm_db *db, int64_t key, void *data) {
  dbm_btree_node *node = btree_search(&db->index.search, key);
  if (!node) {
    return DBM_ERROR_ROW_NOT_FOUND;
  }
  fseek(db->fd, node->value, SEEK_SET);
  fwrite(data, db->header.data_size, 1, db->fd);
  fflush(db->fd);
  return DBM_OK;
}

dbm_db_state minidb_delete(dbm_db *db, int64_t key) {
  if (db->header.row_count > 0) {
    int64_t old_address;
    bool removed = btree_remove(&db->index.search, key, &old_address);
    if (removed) {
      db->header.row_count--;
      assert(db->header.row_count == db->index.search.size);
      btree_insert(&db->index.freelist, old_address, old_address);
      db->header.free_count++;
      assert(db->header.free_count == db->index.freelist.size);
      minidb_header_write(db);
      minidb_index_write(&db->index);
    }
  }
  return DBM_OK;
}
