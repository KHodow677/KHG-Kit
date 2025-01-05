#include "khg_dbm/btree.h"
#include <assert.h>
#include <stdlib.h>

static dbm_btree_node *dbm_node_create(long long key, long long value) {
  dbm_btree_node *node = malloc(sizeof(dbm_btree_node));
  if (node) {
    node->key = key;
    node->value = value;
    node->left = NULL;
    node->right = NULL;
  }
  return node;
}

static void dbm_btree_node_destroy_recursive(dbm_btree_node *node) {
  if (node) {
    dbm_btree_node_destroy_recursive(node->left);
    dbm_btree_node_destroy_recursive(node->right);
    free(node);
  }
}

static dbm_btree_node *dbm_btree_node_search_recursive(dbm_btree_node *node, long long key, dbm_btree_node **parent) {
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

static dbm_btree_node *dbm_btree_node_insert_recursive(dbm_btree_node *parent, long long key, long long value) {
  dbm_btree_node **child = key < parent->key ? &parent->left : &parent->right;
  if (!*child) {
    *child = dbm_node_create(key, value);
    return *child;
  } else {
    return dbm_btree_node_insert_recursive(*child, key, value);
  }
}

static dbm_btree_node *dbm_btree_node_search_successor(dbm_btree_node *node) {
  while (node->left) {
    node = node->left;
  }
  return node;
}

static bool dbm_btree_node_remove_recursive(dbm_btree_node **root, long long key, long long *value) {
  dbm_btree_node *parent = NULL;
  dbm_btree_node *current = dbm_btree_node_search_recursive(*root, key, &parent);
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
    dbm_btree_node *successor = dbm_btree_node_search_successor(current->right);
    long long subkey = successor->key;
    long long old_val = successor->value;
    dbm_btree_node_remove_recursive(root, subkey, NULL);
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

void dbm_btree_init(dbm_btree *tree) {
  tree->size = 0;
  tree->root = NULL;
}

void dbm_btree_destroy(dbm_btree *tree) {
  if (tree->size > 0) {
    dbm_btree_node_destroy_recursive(tree->root);
    tree->size = 0;
    tree->root = NULL;
  }
}

bool dbm_btree_contains(const dbm_btree *tree, long long key) {
  dbm_btree_node *current = tree->root;
  while (current) {
    if (current->key == key) {
      return true;
    }
    current = key < current->key ? current->left : current->right;
  }

  return false;
}

dbm_btree_node *dbm_btree_search(const dbm_btree *tree, long long key) {
  if (tree->size > 0) {
    return dbm_btree_node_search_recursive(tree->root, key, NULL);
  }
  return NULL;
}

dbm_btree_node *dbm_btree_insert(dbm_btree *tree, long long key, long long value) {
  dbm_btree_node *new_node = NULL;
  if (tree->size == 0) {
    assert(tree->root == NULL);
    new_node = dbm_node_create(key, value);
    tree->root = new_node;
  } 
  else {
    new_node = dbm_btree_node_insert_recursive(tree->root, key, value);
  }
  if (new_node) {
    tree->size++;
  }
  return new_node;
}

bool dbm_btree_remove(dbm_btree *tree, long long key, long long *address) {
  if (tree->size != 0) {
    if (dbm_btree_node_remove_recursive(&tree->root, key, address)) {
      tree->size--;
      assert(tree->size >= 0);
      return true;
    }
  }
  return false;
}

