#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct BTreeNode {
  int64_t key;
  int64_t value;
  struct BTreeNode *left;
  struct BTreeNode *right;
} BTreeNode;

typedef struct BTree {
  int64_t size;
  BTreeNode *root;
} BTree;

void btree_init(BTree *tree);
void btree_destroy(BTree *tree);

bool btree_contains(const BTree *tree, int64_t key);
BTreeNode *btree_search(const BTree *tree, int64_t key);
BTreeNode *btree_insert(BTree *tree, int64_t key, int64_t value);
bool btree_remove(BTree *tree, int64_t key, int64_t *old_value);
