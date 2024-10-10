#include "khg_dbm/btree.h"
#include <assert.h>
#include <stdlib.h>

static BTreeNode *node_create(int64_t key, int64_t value) {
  BTreeNode *node = malloc(sizeof(BTreeNode));
  if (node) {
    node->key = key;
    node->value = value;
    node->left = NULL;
    node->right = NULL;
  }
  return node;
}

void btree_init(BTree *tree) {
  tree->size = 0;
  tree->root = NULL;
}

static void btree_node_destroy_recursive(BTreeNode *node) {
  if (node) {
    btree_node_destroy_recursive(node->left);
    btree_node_destroy_recursive(node->right);
    free(node);
  }
}

void btree_destroy(BTree *tree) {
  if (tree->size > 0) {
    btree_node_destroy_recursive(tree->root);
    tree->size = 0;
    tree->root = NULL;
  }
}

bool btree_contains(const BTree *tree, int64_t key) {
  BTreeNode *current = tree->root;
  while (current) {
    if (current->key == key) {
      return true;
    }
    current = key < current->key ? current->left : current->right;
  }

  return false;
}

static BTreeNode *btree_node_search_recursive(BTreeNode *node, int64_t key, BTreeNode **parent) {
  BTreeNode *last = NULL;
  BTreeNode *current = node;
  while (current != NULL && current->key != key) {
    last = current;
    if (key < current->key) {
      current = current->left;
    } 
    else {
      current = current->right;
    }
  }
  if (parent) {
    *parent = last;
  }
  return current;
}

BTreeNode *btree_search(const BTree *tree, int64_t key) {
  if (tree->size > 0) {
    return btree_node_search_recursive(tree->root, key, NULL);
  }
  return NULL;
}

static BTreeNode *btree_node_insert_recursive(BTreeNode *parent, int64_t key, int64_t value) {
  BTreeNode **child = key < parent->key ? &parent->left : &parent->right;
  if (!*child) {
    *child = node_create(key, value);
    return *child;
  } else {
    return btree_node_insert_recursive(*child, key, value);
  }
}

BTreeNode *btree_insert(BTree *tree, int64_t key, int64_t value) {
  BTreeNode *new_node = NULL;
  if (tree->size == 0) {
    assert(tree->root == NULL);
    new_node = node_create(key, value);
    tree->root = new_node;
  } 
  else {
    new_node = btree_node_insert_recursive(tree->root, key, value);
  }
  if (new_node) {
    tree->size++;
  }
  return new_node;
}

static BTreeNode *btree_node_search_successor(BTreeNode *node) {
  while (node->left) {
    node = node->left;
  }
  return node;
}

static bool btree_node_remove_recursive(BTreeNode **root, int64_t key, int64_t *value) {
  BTreeNode *parent = NULL;
  BTreeNode *current = btree_node_search_recursive(*root, key, &parent);
  if (!current) {
    return false;
  }
  if (value) {
    *value = current->value;
  }
  if ((!((current)->left) && !((current)->right))) {
    if (current != *root) {
      if (current == parent->left) {
        parent->left = NULL;
      } 
      else {
        parent->right = NULL;
      }
    } 
    else {
      *root = NULL;
    }
    free(current);
  } 
  else if (((current)->left) && ((current)->right)) {
    BTreeNode *successor = btree_node_search_successor(current->right);
    int64_t subkey = successor->key;
    int64_t old_val = successor->value;
    btree_node_remove_recursive(root, subkey, NULL);
    current->key = subkey;
    current->value = old_val;
  } 
  else {
    BTreeNode *child = current->left ? current->left : current->right;
    if (current != *root) {
      if (current == parent->left) {
        parent->left = child;
      } 
      else {
        parent->right = child;
      }
    } 
    else {
      *root = child;
    }
    free(current);
  }
  return true;
}

bool btree_remove(BTree *tree, int64_t key, int64_t *address) {
  if (tree->size != 0) {
    if (btree_node_remove_recursive(&tree->root, key, address)) {
      tree->size--;
      assert(tree->size >= 0);
      return true;
    }
  }
  return false;
}
