#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct btree_node {
  int64_t key;
  int64_t value;
  struct btree_node *left;
  struct btree_node *right;
} dbm_btree_node;

typedef struct dbm_btree {
  int64_t size;
  dbm_btree_node *root;
} dbm_btree;

void btree_init(dbm_btree *tree);
void btree_destroy(dbm_btree *tree);

bool btree_contains(const dbm_btree *tree, int64_t key);
dbm_btree_node *btree_search(const dbm_btree *tree, int64_t key);
dbm_btree_node *btree_insert(dbm_btree *tree, int64_t key, int64_t value);
bool btree_remove(dbm_btree *tree, int64_t key, int64_t *old_value);
