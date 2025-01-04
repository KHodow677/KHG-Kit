#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct btree_node {
  long key;
  long value;
  struct btree_node *left;
  struct btree_node *right;
} dbm_btree_node;

typedef struct dbm_btree {
  long size;
  dbm_btree_node *root;
} dbm_btree;

void btree_init(dbm_btree *tree);
void btree_destroy(dbm_btree *tree);

bool btree_contains(const dbm_btree *tree, long key);
dbm_btree_node *btree_search(const dbm_btree *tree, long key);
dbm_btree_node *btree_insert(dbm_btree *tree, long key, long value);
bool btree_remove(dbm_btree *tree, long key, long *old_value);

