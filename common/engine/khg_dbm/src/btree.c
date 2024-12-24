#include "khg_dbm/btree.h"
#include <assert.h>
#include <stdlib.h>

static dbm_btree_node *node_create(int64_t key, int64_t value) {
  dbm_btree_node *node = malloc(sizeof(dbm_btree_node));
  if (node) {
    node->key = key;
    node->value = value;
    node->left = NULL;
    node->right = NULL;
  }
  return node;
}

void btree_init(dbm_btree *tree) {
  tree->size = 0;
  tree->root = NULL;
}

static void btree_node_destroy_recursive(dbm_btree_node *node) {
  if (node) {
    btree_node_destroy_recursive(node->left);
    btree_node_destroy_recursive(node->right);
    free(node);
  }
}

void btree_destroy(dbm_btree *tree) {
  if (tree->size > 0) {
    btree_node_destroy_recursive(tree->root);
    tree->size = 0;
    tree->root = NULL;
  }
}

bool btree_contains(const dbm_btree *tree, int64_t key) {
  dbm_btree_node *current = tree->root;
  while (current) {
    if (current->key == key) {
      return true;
    }
    current = key < current->key ? current->left : current->right;
  }

  return false;
}

static dbm_btree_node *btree_node_search_recursive(dbm_btree_node *node, int64_t key, dbm_btree_node **parent) {
  dbm_btree_node *last = NULL;
  dbm_btree_node *current = node;
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

dbm_btree_node *btree_search(const dbm_btree *tree, int64_t key) {
  if (tree->size > 0) {
    return btree_node_search_recursive(tree->root, key, NULL);
  }
  return NULL;
}

static dbm_btree_node *btree_node_insert_recursive(dbm_btree_node *parent, int64_t key, int64_t value) {
  dbm_btree_node **child = key < parent->key ? &parent->left : &parent->right;
  if (!*child) {
    *child = node_create(key, value);
    return *child;
  } else {
    return btree_node_insert_recursive(*child, key, value);
  }
}

dbm_btree_node *btree_insert(dbm_btree *tree, int64_t key, int64_t value) {
  dbm_btree_node *new_node = NULL;
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

static dbm_btree_node *btree_node_search_successor(dbm_btree_node *node) {
  while (node->left) {
    node = node->left;
  }
  return node;
}

static bool btree_node_remove_recursive(dbm_btree_node **root, int64_t key, int64_t *value) {
  dbm_btree_node *parent = NULL;
  dbm_btree_node *current = btree_node_search_recursive(*root, key, &parent);
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
    dbm_btree_node *successor = btree_node_search_successor(current->right);
    int64_t subkey = successor->key;
    int64_t old_val = successor->value;
    btree_node_remove_recursive(root, subkey, NULL);
    current->key = subkey;
    current->value = old_val;
  } 
  else {
    dbm_btree_node *child = current->left ? current->left : current->right;
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

bool btree_remove(dbm_btree *tree, int64_t key, int64_t *address) {
  if (tree->size != 0) {
    if (btree_node_remove_recursive(&tree->root, key, address)) {
      tree->size--;
      assert(tree->size >= 0);
      return true;
    }
  }
  return false;
}
