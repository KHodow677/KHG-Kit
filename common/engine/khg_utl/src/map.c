#include "khg_utl/map.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>
#include <string.h>

#define UTL_RED 1
#define UTL_BLACK 0

struct utl_map {
  utl_map_node *root;
  compare_func_map compFunc;
  value_dealloc_func deallocKey;
  value_dealloc_func deallocValue;
  size_t size;
};


void utl_map_iterator_increment(utl_map_iterator *it) {
  if (it == NULL) {
    utl_error_func("Null iterator provided in map_iterator_increment", utl_user_defined_data);
    return;
  }
  if (it->node == NULL) {
    utl_error_func("Iterator's node is null in map_iterator_increment", utl_user_defined_data);
    return;
  }
  if (it->node->right != NULL) {
    it->node = it->node->right;
    
    while (it->node->left != NULL) {
      it->node = it->node->left;
    }
  } 
  else {
    while (it->node->parent != NULL && it->node == it->node->parent->right) { 
      it->node = it->node->parent;
    }
    it->node = it->node->parent;
  }
}

void utl_map_iterator_decrement(utl_map_iterator *it) {
  if (it == NULL) {
    utl_error_func("Null iterator provided in map_iterator_decrement", utl_user_defined_data);
    return;
  }
  if (it->node == NULL) {
    utl_error_func("Iterator's node is null in map_iterator_decrement", utl_user_defined_data);
    return;
  }
  if (it->node->left != NULL) {
    it->node = it->node->left;
    while (it->node->right != NULL) { 
      it->node = it->node->right;
    }
  } 
  else {
    while (it->node->parent != NULL && it->node == it->node->parent->left) { 
      it->node = it->node->parent;
    }
    it->node = it->node->parent;
  }
}

static utl_map_node *create_node(utl_key_type key, utl_value_type value) {
  utl_map_node *node = (utl_map_node *)malloc(sizeof(utl_map_node));
  if (!node) {
    utl_error_func("Cannot allocate memory for node in create_node", utl_user_defined_data);
    return NULL;
  }
  node->key = key;
  node->value = value;
  node->left = node->right = node->parent = NULL;
  node->color = UTL_RED;
  return node;
}

static void map_left_rotate(utl_map *map, utl_map_node *x) {
  if (map == NULL || x == NULL) {
    utl_error_func("Null pointer provided to map_left_rotate", utl_user_defined_data);
    return;
  }
  utl_map_node *y = x->right;
  if (y == NULL) {
    utl_error_func("Right child is null in map_left_rotate", utl_user_defined_data);
    return;
  }
  x->right = y->left;
  if (y->left != NULL) {
    y->left->parent = x;
  }
  y->parent = x->parent;
  if (x->parent == NULL) { 
    map->root = y;
  }
  else if (x == x->parent->left) { 
    x->parent->left = y;
  }
  else {
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
}

static void map_right_rotate(utl_map *map, utl_map_node *y) {
    if (map == NULL || y == NULL) {
      utl_error_func("Null pointer provided to map_right_rotate", utl_user_defined_data);
      return;
    }
    utl_map_node *x = y->left;
    if (x == NULL) {
      utl_error_func("Left child is null in map_right_rotate", utl_user_defined_data);
      return;
    }
    y->left = x->right;
    if (x->right != NULL) {
      x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == NULL) { 
      map->root = x;
    }
    else if (y == y->parent->right) { 
      y->parent->right = x;
    }
    else {
      y->parent->left = x;
    }
    x->right = y;
    y->parent = x;
}

static void map_transplant(utl_map *map, utl_map_node *u, utl_map_node *v) {
  if (map == NULL || u == NULL) {
    utl_error_func("Null pointer provided to map_transplant", utl_user_defined_data);
    return;
  }
  if (u->parent == NULL) {
    map->root = v;
  }
  else if (u == u->parent->left) {
    u->parent->left = v;
  }
  else {
    u->parent->right = v;
  }
  if (v != NULL) { 
    v->parent = u->parent;
  }
}

static utl_map_node *map_minimum(utl_map_node *node) {
  if (node == NULL) {
    utl_error_func("Null pointer provided to map_minimum", utl_user_defined_data);
    return NULL;
  }
  while (node->left != NULL) {
    node = node->left;
  }
  return node;
}

static void map_erase_fixup(utl_map *map, utl_map_node *x) {
  while (x != map->root && (x == NULL || x->color == UTL_BLACK)) {
    if (x == x->parent->left) {
      utl_map_node *w = x->parent->right;
      if (w->color == UTL_RED) {
        w->color = UTL_BLACK;
        x->parent->color = UTL_RED;
        map_left_rotate(map, x->parent);
        w = x->parent->right;
      }
      if ((w->left == NULL || w->left->color == UTL_BLACK) && (w->right == NULL || w->right->color == UTL_BLACK)) {
        w->color = UTL_RED;
        x = x->parent;
      } 
      else {
        if (w->right == NULL || w->right->color == UTL_BLACK) {
          if (w->left != NULL) {
            w->left->color = UTL_BLACK;
          }
          w->color = UTL_RED;
          map_right_rotate(map, w);
          w = x->parent->right;
        }
        w->color = x->parent->color;
        x->parent->color = UTL_BLACK;
        if (w->right != NULL) { 
          w->right->color = UTL_BLACK;
        }
        map_left_rotate(map, x->parent);
        x = map->root;
      }
    } 
    else {
      utl_map_node *w = x->parent->left;
      if (w->color == UTL_RED) {
        w->color = UTL_BLACK;
        x->parent->color = UTL_RED;
        map_right_rotate(map, x->parent);
        w = x->parent->left;
      }
      if ((w->right == NULL || w->right->color == UTL_BLACK) && (w->left == NULL || w->left->color == UTL_BLACK)) {
        w->color = UTL_RED;
        x = x->parent;
      } 
      else {
        if (w->left == NULL || w->left->color == UTL_BLACK) {
          if (w->right != NULL) {
            w->right->color = UTL_BLACK;
          }
          w->color = UTL_RED;
          map_left_rotate(map, w);
          w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = UTL_BLACK;
        if (w->left != NULL) { 
          w->left->color = UTL_BLACK;
        }
        map_right_rotate(map, x->parent);
        x = map->root;
      }
    }
  }
  if (x != NULL) { 
    x->color = UTL_BLACK;
  }
}

static void map_free_nodes(utl_map_node *node, value_dealloc_func deallocKey, value_dealloc_func deallocValue) {
  if (node == NULL) {
    return;
  }
  map_free_nodes(node->left, deallocKey, deallocValue);
  map_free_nodes(node->right, deallocKey, deallocValue);
  if (deallocKey) { 
    deallocKey(node->key);
  }
  if (deallocValue) {
    deallocValue(node->value);
  }
  free(node);
}

static void map_insert_fixup(utl_map *map, utl_map_node *newNode) {
  if (map == NULL || newNode == NULL) {
    utl_error_func("Null pointer provided to map_insert_fixup", utl_user_defined_data);
    return;
  }
  while (newNode != map->root && newNode->parent->color == UTL_RED) {
    if (newNode->parent == newNode->parent->parent->left) {
      utl_map_node *uncle = newNode->parent->parent->right;
      if (uncle && uncle->color == UTL_RED) {
        newNode->parent->color = UTL_BLACK;
        uncle->color = UTL_BLACK;
        newNode->parent->parent->color = UTL_RED;
        newNode = newNode->parent->parent;
      } 
      else {
        if (newNode == newNode->parent->right) { 
          newNode = newNode->parent;
        }
        break;
      }
    } 
    else {
      utl_map_node *uncle = newNode->parent->parent->left;
      if (uncle && uncle->color == UTL_RED) {
        newNode->parent->color = UTL_BLACK;
        uncle->color = UTL_BLACK;
        newNode->parent->parent->color = UTL_RED;
        newNode = newNode->parent->parent;
      } 
      else {
        if (newNode == newNode->parent->left) {
          newNode = newNode->parent;
          map_right_rotate(map, newNode);
        }
        newNode->parent->color = UTL_BLACK;
        newNode->parent->parent->color = UTL_RED;
        map_left_rotate(map, newNode->parent->parent);
        break;
      }
    }
  }
  map->root->color = UTL_BLACK;
}

utl_map *utl_map_create(compare_func_map comp, value_dealloc_func deallocKey, value_dealloc_func deallocValue) {
  if (!comp) {
    utl_error_func("Compare function is null, cannot create map", utl_user_defined_data);
    exit(-1);
  }
  utl_map *m = (utl_map *)malloc(sizeof(utl_map));
  if (!m) {
    utl_error_func("Cannot allocate memory for map", utl_user_defined_data);
    exit(-1);
  }
  m->root = NULL;
  m->compFunc = comp;
  m->deallocKey = deallocKey;
  m->deallocValue = deallocValue;
  m->size = 0;
  return m;
}

void utl_map_deallocate(utl_map *map) {
  if (!map){
    utl_error_func("Map is null or empty there is nothing to deallocate in map_deallocate", utl_user_defined_data);
    return;
  }
  map_free_nodes(map->root, map->deallocKey, map->deallocValue);
  free(map);
}

bool utl_map_empty(const utl_map *map) {
  if (map == NULL) {
    utl_error_func("Null pointer provided for map in map_empty", utl_user_defined_data);
    return true;
  }
  return map->size == 0;
}

size_t utl_map_size(const utl_map *map) {
  if (map == NULL) {
    utl_error_func("Null pointer provided for map in map_size", utl_user_defined_data);
    return 0;
  }
  return map->size;
}

size_t utl_map_max_size(const utl_map *map) {
  if (map == NULL) {
    utl_error_func("Null pointer provided for map in map_max_size", utl_user_defined_data);
    return 0;
  }
  return (size_t)(~((size_t)0)) / sizeof(utl_map_node);
}

bool utl_map_insert(utl_map *map, utl_key_type key, utl_value_type value) {
    if (map == NULL || key == NULL) {
      utl_error_func("Null pointer provided for map or key in map_insert", utl_user_defined_data);
      return false;
    }
    utl_map_node **curr = &map->root;
    utl_map_node *parent = NULL;
    while (*curr) {
      parent = *curr;
      int cmp = map->compFunc(key, (*curr)->key);
      if (cmp == 0) {
        if (map->deallocValue) {
          map->deallocValue((*curr)->value);
        }
        (*curr)->value = value;
        return true;
      }
      if (cmp < 0) {
          curr = &(*curr)->left;
      } 
      else {
          curr = &(*curr)->right;
      }
    }
    utl_map_node *newNode = create_node(key, value);
    if (!newNode) {
      utl_error_func("Node creation failed in map_insert", utl_user_defined_data);
      return false;
    }
    *curr = newNode;
    newNode->parent = parent;
    map->size++;
    map_insert_fixup(map, newNode);
    return true;
}

utl_value_type utl_map_at(const utl_map *map, utl_key_type key) {
  if (map == NULL || key == NULL) {
    utl_error_func("Null pointer provided for map or key in map_at", utl_user_defined_data);
    return NULL;
  }
  utl_map_node *curr = map->root;
  while (curr) {
    int cmp = map->compFunc(key, curr->key);
    if (cmp == 0) { 
      return curr->value;
    }
    if (cmp < 0) { 
      curr = curr->left;
    }
    else { 
      curr = curr->right;
    }
  }
  return NULL;
}

void utl_map_clear(utl_map *map) {
  if (map == NULL) {
    utl_error_func("Null pointer provided for map in map_clear", utl_user_defined_data);
    return;
  }
  map_free_nodes(map->root, map->deallocKey, map->deallocValue);
  map->root = NULL;
  map->size = 0;
}

void utl_map_swap(utl_map *map1, utl_map *map2) {
  if (map1 == NULL || map2 == NULL) {
    utl_error_func("Null pointer provided for one or both maps in map_swap", utl_user_defined_data);
    return;
  }
  utl_map_node *tempRoot = map1->root;
  map1->root = map2->root;
  map2->root = tempRoot;
  size_t tempSize = map1->size;
  map1->size = map2->size;
  map2->size = tempSize;
  compare_func_map tempCompFunc = map1->compFunc;
  map1->compFunc = map2->compFunc;
  map2->compFunc = tempCompFunc;
  value_dealloc_func tempDeallocKey = map1->deallocKey;
  map1->deallocKey = map2->deallocKey;
  map2->deallocKey = tempDeallocKey;
  value_dealloc_func tempDeallocValue = map1->deallocValue;
  map1->deallocValue = map2->deallocValue;
  map2->deallocValue = tempDeallocValue;
}

size_t utl_map_count(const utl_map *map, utl_key_type key) {
  if (map == NULL) {
    utl_error_func("Null pointer provided for map in map_count", utl_user_defined_data);
    return 0;
  }
  utl_map_node* current = map->root;
  while (current != NULL) {
    int cmp = map->compFunc(key, current->key);
    if (cmp == 0) {
      return 1;
    }
    if (cmp < 0) { 
      current = current->left;
    }
    else {
      current = current->right;
    }
  }
  return 0;
}

bool utl_map_emplace(utl_map *map, utl_key_type key, utl_value_type value) {
  if (map == NULL || key == NULL) {
    utl_error_func("Null pointer provided for map or key in map_emplace", utl_user_defined_data);
    return false;
  }
  utl_map_node **curr = &map->root;
  utl_map_node *parent = NULL;
  while (*curr) {
    parent = *curr;
    int cmp = map->compFunc(key, (*curr)->key);
    if (cmp == 0) { 
      return false;
    }
    if (cmp < 0) { 
      curr = &(*curr)->left;
    }
    else { 
      curr = &(*curr)->right;
    }
  }
  utl_map_node *newNode = create_node(key, value);
  if (!newNode) {
    return false;
  }
  *curr = newNode;
  newNode->parent = parent;
  map->size++;
  map_insert_fixup(map, newNode);
  return true;
}

compare_func_map utl_map_key_comp(const utl_map *map) {
  if (map == NULL) {
    utl_error_func("Null pointer provided for map in map_key_comp", utl_user_defined_data);
    return NULL;
  }
  return map->compFunc;
}

bool utl_map_emplace_hint(utl_map *map, utl_map_iterator hint, utl_key_type key, utl_value_type value) {
  if (map == NULL || !key) {
    utl_error_func("Map or key is null cannot emplace in map_emplace_hint", utl_user_defined_data);
    exit(-1);
  }
  utl_map_node *newNode = create_node(key, value);
  if (newNode == NULL) {
    utl_error_func("Unable to crate new Node in map_emplace_hint", utl_user_defined_data);
    return false;
  }
  if (map->root == NULL) {
    map->root = newNode;
    map->root->color = UTL_BLACK;
    map->size++;
    return true;
  }
  if (hint.node != NULL) {
    utl_map_node *curr = hint.node;
    int cmp = map->compFunc(key, curr->key);
    if (cmp < 0) {
      if (curr->left == NULL) {
        curr->left = newNode;
        newNode->parent = curr;
        map->size++;
        map_insert_fixup(map, newNode);
        return true;
      }
    } 
    else if (cmp > 0) {
      if (curr->right == NULL) {
        curr->right = newNode;
        newNode->parent = curr;
        map->size++;
        map_insert_fixup(map, newNode);
        return true;
      }
    } 
    else {
      free(newNode);
      return false;
    }
  }
  utl_map_node **curr = &map->root;
  utl_map_node *parent = NULL;
  while (*curr) {
    parent = *curr;
    int cmp = map->compFunc(key, (*curr)->key);
    if (cmp == 0) {
      free(newNode);
      return false;
    }
    if (cmp < 0) {
      curr = &(*curr)->left;
    }
    else { 
      curr = &(*curr)->right;
    }
  }
  *curr = newNode;
  newNode->parent = parent;
  map->size++;
  map_insert_fixup(map, newNode);
  return true;
}

bool utl_map_erase(utl_map *map, utl_key_type key) {
  if (map == NULL || map->root == NULL) {
    utl_error_func("Map or map root is null in map_erase", utl_user_defined_data);
    return false;
  }
  utl_map_node *z = map->root;
  while (z != NULL) {
    int cmp = map->compFunc(key, z->key);
    if (cmp == 0) {
      break;
    }
    if (cmp < 0) { 
      z = z->left;
    }
    else {
      z = z->right;
    }
  }
  if (z == NULL) {
    utl_error_func("Key not found in map_erase", utl_user_defined_data);
    return false;
  }
  utl_map_node *y = z;
  int y_original_color = y->color;
  utl_map_node *x;
  if (z->left == NULL) {
    x = z->right;
    map_transplant(map, z, z->right);
  } 
  else if (z->right == NULL) {
    x = z->left;
    map_transplant(map, z, z->left);
  } 
  else {
    y = map_minimum(z->right);
    y_original_color = y->color;
    x = y->right;
    if (y->parent == z) {
      if (x != NULL) {
        x->parent = y;
      }
    } 
    else {
      map_transplant(map, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }
    map_transplant(map, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }
  if (map->deallocKey) {
    map->deallocKey(z->key);
  }
  if (map->deallocValue) {
    map->deallocValue(z->value);
  }
  free(z);
  if (y_original_color == UTL_BLACK) { 
    map_erase_fixup(map, x);
  }
  map->size--;
  return true;
}

utl_map_iterator utl_map_find(const utl_map *map, utl_key_type key) {
  utl_map_iterator iterator = {0};
  if (map == NULL || key == NULL) {
    utl_error_func("Null pointer provided for map or key in map_find", utl_user_defined_data);
    return iterator;
  }
  utl_map_node *current = map->root;
  while (current != NULL) {
    int cmp = map->compFunc(key, current->key);
    if (cmp == 0) {
      iterator.node = current;
      return iterator;
    }
    current = cmp < 0 ? current->left : current->right;
  }
  return iterator;
}

utl_map_iterator utl_map_begin(const utl_map *map) {
  utl_map_iterator iterator = {0};
  if (map == NULL) {
    utl_error_func("Null pointer provided for map in map_begin", utl_user_defined_data);
    return iterator;
  }
  utl_map_node *current = map->root;
  while (current->left != NULL) {
    current = current->left;
  }
  iterator.node =current;
  return iterator;
}

utl_map_iterator utl_map_end(const utl_map *map) {
  if (map == NULL) {
    utl_error_func("Null pointer provided for map in map_end", utl_user_defined_data);
    return (utl_map_iterator){0};
  }
  return (utl_map_iterator){0};
}

utl_map_iterator utl_map_rbegin(const utl_map *map) {
  utl_map_iterator iterator = {0};
  if (map == NULL) {
    utl_error_func("Null pointer provided for map in map_rbegin", utl_user_defined_data);
    return iterator;
  }
  utl_map_node *current = map->root;
  while (current->right != NULL) { 
    current = current->right;
  }
  iterator.node = current;
  return iterator;
}

utl_map_iterator utl_map_rend(const utl_map *map) {
  if (map == NULL) {
    utl_error_func("Null pointer provided for map in map_rend", utl_user_defined_data);
    return (utl_map_iterator){0};
  }
  return (utl_map_iterator){0};
}

utl_map_iterator utl_map_cbegin(const utl_map *map) {
  utl_map_iterator iterator = {0};
  if (map == NULL) {
    utl_error_func("Null pointer provided for map in map_cbegin", utl_user_defined_data);
    return iterator;
  }
  if (map != NULL && map->root != NULL) {
    utl_map_node *current = map->root;
    while (current->left != NULL) {
      current = current->left;
    }
    iterator.node = current;
  }
  return iterator;
}

utl_map_iterator utl_map_cend(const utl_map* map) {
  if (map == NULL) {
    utl_error_func("Null pointer provided for map in map_cend", utl_user_defined_data);
    return (utl_map_iterator){0};
  }
  return (utl_map_iterator){0};
}

utl_map_iterator utl_map_crbegin(const utl_map *map) {
  utl_map_iterator iterator = {0};
  if (map == NULL) {
    utl_error_func("Null pointer provided for map in map_crbegin", utl_user_defined_data);
    return iterator;
  }
  if (map != NULL && map->root != NULL) {
    utl_map_node *current = map->root;
    while (current->right != NULL) {
      current = current->right;
    }
    iterator.node = current;
  }
  return iterator;
}

utl_map_iterator utl_map_crend(const utl_map *map) {
  if (map == NULL) {
    utl_error_func("Null pointer provided for map in map_crend", utl_user_defined_data);
    return (utl_map_iterator){0};
  }
  return (utl_map_iterator){0};
}

utl_map_iterator utl_map_lower_bound(const utl_map *map, utl_key_type key) {
  utl_map_iterator iterator = {0};
  if (map == NULL || key == NULL) {
    utl_error_func("Null pointer provided for map or key in map_lower_bound", utl_user_defined_data);
    return iterator;
  }
  utl_map_node *current = map->root;
  utl_map_node *last = NULL;
  while (current != NULL) {
    if (map->compFunc(current->key, key) >= 0) {
      last = current;
      current = current->left;
    } 
    else {
      current = current->right;
    }
  }
  if (last != NULL) { 
    iterator.node = last;
  }
  return iterator;
}

utl_map_iterator utl_map_upper_bound(const utl_map* map, utl_key_type key) {
  utl_map_iterator iterator = {0};
  if (map == NULL || key == NULL) {
    utl_error_func("Null pointer provided for map or key in map_upper_bound", utl_user_defined_data);
    return iterator;
  }
  utl_map_node *current = map->root;
  utl_map_node *last = NULL;
  while (current != NULL) {
    if (map->compFunc(current->key, key) > 0) {
      last = current;
      current = current->left;
    } 
    else {
      current = current->right;
    }
  }
  if (last != NULL) { 
    iterator.node = last;
  }
  return iterator;
}

utl_map_iterator_pair utl_map_equal_range(const utl_map *map, utl_key_type key) {
  utl_map_iterator_pair iteratorPair = {{0}, {0}};
  if (map == NULL) {
    utl_error_func("Map object is Null in map_equal_range", utl_user_defined_data);
    return iteratorPair;
  }
  if (key == NULL) {
    utl_error_func("KeyType value is Null in map_equal_range", utl_user_defined_data);
    return iteratorPair;
  }
  iteratorPair.first = utl_map_lower_bound(map, key);
  iteratorPair.second = utl_map_upper_bound(map, key);
  return iteratorPair;
}

utl_key_type utl_map_node_get_key(utl_map_node *node) {
  if (!node) {
    utl_error_func("MapNode object is Null and Invalid in map_node_get_key", utl_user_defined_data);
    return NULL;
  }
  return node->key;
}

utl_value_type utl_map_node_get_value(utl_map_node *node) {
  if (!node) {
    utl_error_func("MapNode object is Null and Invalid in map_node_get_value", utl_user_defined_data);
    return NULL;
  }
  return node->value;
}

utl_map *utl_map_copy(const utl_map *src) {
  if (src == NULL){
    utl_error_func("Map Object is NULL and Invalid in map_copy", utl_user_defined_data);
    return NULL;
  }
  utl_map *newMap = utl_map_create(src->compFunc, src->deallocKey, src->deallocValue);
  if (!newMap) {
    utl_error_func("Can not Allocate memory for newMap in map_copy", utl_user_defined_data);    
    return NULL;
  }
  for (utl_map_iterator it = utl_map_begin(src); it.node != utl_map_end(src).node; utl_map_iterator_increment(&it)) { 
    utl_map_insert(newMap, utl_map_node_get_key(it.node), utl_map_node_get_value(it.node));
  }
  return newMap;
}

