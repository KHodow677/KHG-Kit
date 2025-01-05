#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct btree_node {
  long long key;
  long long value;
  struct btree_node *left;
  struct btree_node *right;
} dbm_btree_node;

typedef struct dbm_btree {
  long long size;
  dbm_btree_node *root;
} dbm_btree;

void dbm_btree_init(dbm_btree *tree);
void dbm_btree_destroy(dbm_btree *tree);

bool dbm_btree_contains(const dbm_btree *tree, long long key);
dbm_btree_node *dbm_btree_search(const dbm_btree *tree, long long key);
dbm_btree_node *dbm_btree_insert(dbm_btree *tree, long long key, long long value);
bool dbm_btree_remove(dbm_btree *tree, long long key, long long *old_value);

