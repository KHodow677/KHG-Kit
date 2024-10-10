#include "khg_dbm/btree.h"
#include "khg_dbm/index.h"
#include <stdbool.h>
#include <stdlib.h>

void minidb_index_init(dbm_index *index) {
  btree_init(&index->search);
  btree_init(&index->freelist);
  index->fd = NULL;
}

dbm_db_state minidb_index_open(dbm_index *index, const char *path, int64_t row_count, int64_t freelist_count) {
  bool is_new_file = row_count == INT64_C(0) && freelist_count == INT64_C(0);
  FILE *fd = fopen(path, is_new_file ? "w+" : "r+");
  if (!fd) {
    return DBM_ERROR_CANNOT_OPEN_FILE;
  }
  index->fd = fd;
  if (!is_new_file) {
    int64_t i;
    dbm_btree_node node;
    fseek(fd, 0, SEEK_SET);
    for (i = 0; i < row_count; i++) {
      fread(&node.key, sizeof(node.key), 1, fd);
      fread(&node.value, sizeof(node.value), 1, fd);
      btree_insert(&index->search, node.key, node.value);
    }
    for (i = 0; i < freelist_count; i++) {
      fread(&node.key, sizeof(node.key), 1, fd);
      fread(&node.value, sizeof(node.value), 1, fd);
      btree_insert(&index->freelist, node.key, node.value);
    }
  }
  return DBM_OK;
}

void minidb_index_close(dbm_index *index) {
  minidb_index_write(index);
  fflush(index->fd);
  fclose(index->fd);
  btree_destroy(&index->search);
  btree_destroy(&index->freelist);
}

static void minidb_index_write_node_recursive(const dbm_btree_node *node, FILE *fd) {
  if (node) {
    minidb_index_write_node_recursive(node->left, fd);
    fwrite(&node->key, sizeof(node->key), 1, fd);
    fwrite(&node->value, sizeof(node->value), 1, fd);
    minidb_index_write_node_recursive(node->right, fd);
  }
}

void minidb_index_write(const dbm_index *index) {
  fseek(index->fd, 0, SEEK_SET);
  minidb_index_write_node_recursive(index->search.root, index->fd);
  minidb_index_write_node_recursive(index->freelist.root, index->fd);
  fflush(index->fd);
}
