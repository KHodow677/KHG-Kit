#include "khg_utils/error_func.h"
#include "khg_utils/map.h"
#include <stdlib.h>
#include <string.h>

#define RED 1
#define BLACK 0

struct map {
  map_node *root;
  compare_func_map compFunc;
  value_dealloc_func deallocKey;
  value_dealloc_func deallocValue;
  size_t size;
};


void map_iterator_increment(map_iterator *it) {
  if (it == NULL) {
    error_func("Null iterator provided in map_iterator_increment", user_defined_data);
    return;
  }
  if (it->node == NULL) {
    error_func("Iterator's node is null in map_iterator_increment", user_defined_data);
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

void map_iterator_decrement(map_iterator *it) {
  if (it == NULL) {
    error_func("Null iterator provided in map_iterator_decrement", user_defined_data);
    return;
  }
  if (it->node == NULL) {
    error_func("Iterator's node is null in map_iterator_decrement", user_defined_data);
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

static map_node *create_node(key_type key, value_type value) {
  map_node *node = (map_node *)malloc(sizeof(map_node));
  if (!node) {
    error_func("Cannot allocate memory for node in create_node", user_defined_data);
    return NULL;
  }
  node->key = key;
  node->value = value;
  node->left = node->right = node->parent = NULL;
  node->color = RED;
  return node;
}

static void map_left_rotate(map *map, map_node *x) {
  if (map == NULL || x == NULL) {
    error_func("Null pointer provided to map_left_rotate", user_defined_data);
    return;
  }
  map_node *y = x->right;
  if (y == NULL) {
    error_func("Right child is null in map_left_rotate", user_defined_data);
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

static void map_right_rotate(map *map, map_node *y) {
    if (map == NULL || y == NULL) {
      error_func("Null pointer provided to map_right_rotate", user_defined_data);
      return;
    }
    map_node *x = y->left;
    if (x == NULL) {
      error_func("Left child is null in map_right_rotate", user_defined_data);
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

static void map_transplant(map *map, map_node *u, map_node *v) {
  if (map == NULL || u == NULL) {
    error_func("Null pointer provided to map_transplant", user_defined_data);
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

static map_node *map_minimum(map_node *node) {
  if (node == NULL) {
    error_func("Null pointer provided to map_minimum", user_defined_data);
    return NULL;
  }
  while (node->left != NULL) {
    node = node->left;
  }
  return node;
}

static void map_erase_fixup(map *map, map_node *x) {
  while (x != map->root && (x == NULL || x->color == BLACK)) {
    if (x == x->parent->left) {
      map_node *w = x->parent->right;
      if (w->color == RED) {
        w->color = BLACK;
        x->parent->color = RED;
        map_left_rotate(map, x->parent);
        w = x->parent->right;
      }
      if ((w->left == NULL || w->left->color == BLACK) && (w->right == NULL || w->right->color == BLACK)) {
        w->color = RED;
        x = x->parent;
      } 
      else {
        if (w->right == NULL || w->right->color == BLACK) {
          if (w->left != NULL) {
            w->left->color = BLACK;
          }
          w->color = RED;
          map_right_rotate(map, w);
          w = x->parent->right;
        }
        w->color = x->parent->color;
        x->parent->color = BLACK;
        if (w->right != NULL) { 
          w->right->color = BLACK;
        }
        map_left_rotate(map, x->parent);
        x = map->root;
      }
    } 
    else {
      map_node *w = x->parent->left;
      if (w->color == RED) {
        w->color = BLACK;
        x->parent->color = RED;
        map_right_rotate(map, x->parent);
        w = x->parent->left;
      }
      if ((w->right == NULL || w->right->color == BLACK) && (w->left == NULL || w->left->color == BLACK)) {
        w->color = RED;
        x = x->parent;
      } 
      else {
        if (w->left == NULL || w->left->color == BLACK) {
          if (w->right != NULL) {
            w->right->color = BLACK;
          }
          w->color = RED;
          map_left_rotate(map, w);
          w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = BLACK;
        if (w->left != NULL) { 
          w->left->color = BLACK;
        }
        map_right_rotate(map, x->parent);
        x = map->root;
      }
    }
  }
  if (x != NULL) { 
      x->color = BLACK;
  }
}

static void map_free_nodes(map_node *node, value_dealloc_func deallocKey, value_dealloc_func deallocValue) {
  if (node == NULL) {
    error_func("Node param is null and invalid in map_free_nodes", user_defined_data);
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

static void map_insert_fixup(map *map, map_node *newNode) {
  if (map == NULL || newNode == NULL) {
    error_func("Null pointer provided to map_insert_fixup", user_defined_data);
    return;
  }
  while (newNode != map->root && newNode->parent->color == RED) {
    if (newNode->parent == newNode->parent->parent->left) {
      map_node *uncle = newNode->parent->parent->right;
      if (uncle && uncle->color == RED) {
        newNode->parent->color = BLACK;
        uncle->color = BLACK;
        newNode->parent->parent->color = RED;
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
      map_node *uncle = newNode->parent->parent->left;
      if (uncle && uncle->color == RED) {
        newNode->parent->color = BLACK;
        uncle->color = BLACK;
        newNode->parent->parent->color = RED;
        newNode = newNode->parent->parent;
      } 
      else {
        if (newNode == newNode->parent->left) {
          newNode = newNode->parent;
          map_right_rotate(map, newNode);
        }
        newNode->parent->color = BLACK;
        newNode->parent->parent->color = RED;
        map_left_rotate(map, newNode->parent->parent);
        break;
      }
    }
  }
  map->root->color = BLACK;
}

map *map_create(compare_func_map comp, value_dealloc_func deallocKey, value_dealloc_func deallocValue) {
  if (!comp) {
    error_func("Compare function is null, cannot create map", user_defined_data);
    exit(-1);
  }
  map *m = (map *)malloc(sizeof(map));
  if (!m) {
    error_func("Cannot allocate memory for map", user_defined_data);
    exit(-1);
  }
  m->root = NULL;
  m->compFunc = comp;
  m->deallocKey = deallocKey;
  m->deallocValue = deallocValue;
  m->size = 0;
  return m;
}

void map_deallocate(map *map) {
  if (!map){
    error_func("Map is null or empty there is nothing to deallocate in map_deallocate", user_defined_data);
    return;
  }
  map_free_nodes(map->root, map->deallocKey, map->deallocValue);
  free(map);
}

bool map_empty(const map *map) {
  if (map == NULL) {
    error_func("Null pointer provided for map in map_empty", user_defined_data);
    return true;
  }
  return map->size == 0;
}

size_t map_size(const map *map) {
  if (map == NULL) {
    error_func("Null pointer provided for map in map_size", user_defined_data);
    return 0;
  }
  return map->size;
}

size_t map_max_size(const map *map) {
  if (map == NULL) {
    error_func("Null pointer provided for map in map_max_size", user_defined_data);
    return 0;
  }
  return (size_t)(~((size_t)0)) / sizeof(map_node);
}

bool map_insert(map *map, key_type key, value_type value) {
    if (map == NULL || key == NULL) {
      error_func("Null pointer provided for map or key in map_insert", user_defined_data);
      return false;
    }
    map_node **curr = &map->root;
    map_node *parent = NULL;
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
    map_node *newNode = create_node(key, value);
    if (!newNode) {
      error_func("Node creation failed in map_insert", user_defined_data);
      return false;
    }
    *curr = newNode;
    newNode->parent = parent;
    map->size++;
    map_insert_fixup(map, newNode);
    return true;
}

value_type map_at(const map *map, key_type key) {
  if (map == NULL || key == NULL) {
    error_func("Null pointer provided for map or key in map_at", user_defined_data);
    return NULL;
  }
  map_node *curr = map->root;
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

void map_clear(map *map) {
  if (map == NULL) {
    error_func("Null pointer provided for map in map_clear", user_defined_data);
    return;
  }
  map_free_nodes(map->root, map->deallocKey, map->deallocValue);
  map->root = NULL;
  map->size = 0;
}

void map_swap(map *map1, map *map2) {
  if (map1 == NULL || map2 == NULL) {
    error_func("Null pointer provided for one or both maps in map_swap", user_defined_data);
    return;
  }
  map_node *tempRoot = map1->root;
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

size_t map_count(const map *map, key_type key) {
  if (map == NULL) {
    error_func("Null pointer provided for map in map_count", user_defined_data);
    return 0;
  }
  map_node* current = map->root;
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

bool map_emplace(map *map, key_type key, value_type value) {
    if (map == NULL || key == NULL) {
      error_func("Null pointer provided for map or key in map_emplace", user_defined_data);
      return false;
    }
    map_node **curr = &map->root;
    map_node *parent = NULL;
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
    map_node *newNode = create_node(key, value);
    if (!newNode) {
      return false;
    }
    *curr = newNode;
    newNode->parent = parent;
    map->size++;
    map_insert_fixup(map, newNode);
    return true;
}

compare_func_map map_key_comp(const map *map) {
  if (map == NULL) {
    error_func("Null pointer provided for map in map_key_comp", user_defined_data);
    return NULL;
  }
  return map->compFunc;
}

bool map_emplace_hint(map *map, map_iterator hint, key_type key, value_type value) {
    if (map == NULL || !key) {
      error_func("Map or key is null cannot emplace in map_emplace_hint", user_defined_data);
      exit(-1);
    }
    map_node *newNode = create_node(key, value);
    if (newNode == NULL) {
      error_func("Unable to crate new Node in map_emplace_hint", user_defined_data);
      return false;
    }
    if (map->root == NULL) {
      map->root = newNode;
      map->root->color = BLACK;
      map->size++;
      return true;
    }
    if (hint.node != NULL) {
      map_node *curr = hint.node;
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
    map_node **curr = &map->root;
    map_node *parent = NULL;
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

bool map_erase(map *map, key_type key) {
  if (map == NULL || map->root == NULL) {
    error_func("Map or map root is null in map_erase", user_defined_data);
    return false;
  }
  map_node *z = map->root;
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
    error_func("Key not found in map_erase", user_defined_data);
    return false;
  }
  map_node *y = z;
  int y_original_color = y->color;
  map_node *x;
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
  if (y_original_color == BLACK) { 
    map_erase_fixup(map, x);
  }
  map->size--;
  return true;
}

map_iterator map_find(const map *map, key_type key) {
  map_iterator iterator = {0};
  if (map == NULL || key == NULL) {
    error_func("Null pointer provided for map or key in map_find", user_defined_data);
    return iterator;
  }
  map_node *current = map->root;
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

map_iterator map_begin(const map *map) {
  map_iterator iterator = {0};
  if (map == NULL) {
    error_func("Null pointer provided for map in map_begin", user_defined_data);
    return iterator;
  }
  map_node *current = map->root;
  while (current->left != NULL) {
    current = current->left;
  }
  iterator.node =current;
  return iterator;
}

map_iterator map_end(const map *map) {
  if (map == NULL) {
    error_func("Null pointer provided for map in map_end", user_defined_data);
    return (map_iterator){0};
  }
  return (map_iterator){0};
}

map_iterator map_rbegin(const map *map) {
  map_iterator iterator = {0};
  if (map == NULL) {
    error_func("Null pointer provided for map in map_rbegin", user_defined_data);
    return iterator;
  }
  map_node *current = map->root;
  while (current->right != NULL) { 
    current = current->right;
  }
  iterator.node = current;
  return iterator;
}

map_iterator map_rend(const map *map) {
  if (map == NULL) {
    error_func("Null pointer provided for map in map_rend", user_defined_data);
    return (map_iterator){0};
  }
  return (map_iterator){0};
}

map_iterator map_cbegin(const map *map) {
  map_iterator iterator = {0};
  if (map == NULL) {
    error_func("Null pointer provided for map in map_cbegin", user_defined_data);
    return iterator;
  }
  if (map != NULL && map->root != NULL) {
    map_node *current = map->root;
    while (current->left != NULL) {
      current = current->left;
    }
    iterator.node = current;
  }
  return iterator;
}

map_iterator map_cend(const map* map) {
  if (map == NULL) {
    error_func("Null pointer provided for map in map_cend", user_defined_data);
    return (map_iterator){0};
  }
  return (map_iterator){0};
}

map_iterator map_crbegin(const map *map) {
  map_iterator iterator = {0};
  if (map == NULL) {
    error_func("Null pointer provided for map in map_crbegin", user_defined_data);
    return iterator;
  }
  if (map != NULL && map->root != NULL) {
    map_node *current = map->root;
    while (current->right != NULL) {
      current = current->right;
    }
    iterator.node = current;
  }
  return iterator;
}

map_iterator map_crend(const map *map) {
  if (map == NULL) {
    error_func("Null pointer provided for map in map_crend", user_defined_data);
    return (map_iterator){0};
  }
  return (map_iterator){0};
}

map_iterator map_lower_bound(const map *map, key_type key) {
  map_iterator iterator = {0};
  if (map == NULL || key == NULL) {
    error_func("Null pointer provided for map or key in map_lower_bound", user_defined_data);
    return iterator;
  }
  map_node *current = map->root;
  map_node *last = NULL;
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

map_iterator map_upper_bound(const map* map, key_type key) {
  map_iterator iterator = {0};
  if (map == NULL || key == NULL) {
    error_func("Null pointer provided for map or key in map_upper_bound", user_defined_data);
    return iterator;
  }
  map_node *current = map->root;
  map_node *last = NULL;
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

map_iterator_pair map_equal_range(const map *map, key_type key) {
  map_iterator_pair iteratorPair = {{0}, {0}};
  if (map == NULL) {
    error_func("Map object is Null in map_equal_range", user_defined_data);
    return iteratorPair;
  }
  if (key == NULL) {
    error_func("KeyType value is Null in map_equal_range", user_defined_data);
    return iteratorPair;
  }
  iteratorPair.first = map_lower_bound(map, key);
  iteratorPair.second = map_upper_bound(map, key);
  return iteratorPair;
}

key_type map_node_get_key(map_node *node) {
  if (!node) {
    error_func("MapNode object is Null and Invalid in map_node_get_key", user_defined_data);
    return NULL;
  }
  return node->key;
}

value_type map_node_get_value(map_node *node) {
  if (!node) {
    error_func("MapNode object is Null and Invalid in map_node_get_value", user_defined_data);
    return NULL;
  }
  return node->value;
}

map *map_copy(const map *src) {
  if (src == NULL){
    error_func("Map Object is NULL and Invalid in map_copy", user_defined_data);
    return NULL;
  }
  map *newMap = map_create(src->compFunc, src->deallocKey, src->deallocValue);
  if (!newMap) {
    error_func("Can not Allocate memory for newMap in map_copy", user_defined_data);    
    return NULL;
  }
  for (map_iterator it = map_begin(src); it.node != map_end(src).node; map_iterator_increment(&it)) { 
    map_insert(newMap, map_node_get_key(it.node), map_node_get_value(it.node));
  }
  return newMap;
}
