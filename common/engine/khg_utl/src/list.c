#include "khg_utl/list.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>
#include <string.h>

utl_list *utl_list_create(unsigned int item_size, utl_compare_function compare) {
  if (item_size == 0) {
    utl_error_func("Item size must be greater than 0", utl_user_defined_data);
    exit(-1);
  }
  utl_list *list = malloc(sizeof(utl_list));
  if (!list) {
    utl_error_func("Cannot allocate memory for list", utl_user_defined_data);
    exit(-1);
  }
  list->head = list->tail = NULL;
  list->size = 0;
  list->item_size = item_size;
  list->compare = compare; 
  return list;
}

void *utl_list_front(const utl_list *list) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_error_func);
    return NULL;
  }
  if (list->head == NULL) {
    utl_error_func("The list is empty", utl_user_defined_data);
    return NULL;
  }
  return list->head->value;
}

void *utl_list_back(const utl_list *list) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return NULL;
  }
  if (list->tail == NULL) {
    utl_error_func("The list is empty", utl_user_defined_data);
    return NULL;
  }
  return list->tail->value;
}

void *utl_list_insert(utl_list *list, unsigned int index, void *value) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return NULL;
  }
  if (index > list->size) {
    utl_error_func("Index out of bounds", utl_user_defined_data);
    return NULL;
  }
  if (index == 0) {
    utl_list_push_front(list, value);
    return list->head->value;
  } 
  else if (index == list->size) {
    utl_list_push_back(list, value);
    return list->tail->value;
  }
  utl_node *new_node = malloc(sizeof(utl_node));
  if (!new_node) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  new_node->value = malloc(list->item_size);
  if (!new_node->value) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    free(new_node);
    return NULL;
  }
  memcpy(new_node->value, value, list->item_size);
  utl_node *current = list->head;
  for (unsigned int i = 0; i < index - 1; ++i) {
    current = current->next;
  }
  new_node->next = current->next;
  new_node->prev = current;
  if (new_node->next != NULL) {
    new_node->next->prev = new_node;  
  }
  current->next = new_node;
  list->size++;
  return new_node->value;
}

void *utl_list_erase(utl_list *list, unsigned int index) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return NULL;
  }
  if (index >= list->size) {
    utl_error_func("Index out of bounds", utl_user_defined_data);
    return NULL;
  }
  utl_node *node_to_remove;
  if (index == 0) {
    node_to_remove = list->head;
    list->head = node_to_remove->next;
    if (list->head) {
      list->head->prev = NULL;
    }
  } 
  else {
    utl_node *current = list->head;
    for (unsigned int i = 0; i < index - 1; ++i) {
      current = current->next;
    }
    node_to_remove = current->next;
    current->next = node_to_remove->next;
    if (node_to_remove->next) {
      node_to_remove->next->prev = current;
    }
  }
  if (node_to_remove == list->tail) {
    list->tail = node_to_remove->prev;
  }
  void *removed_value = node_to_remove->value;
  free(node_to_remove);
  list->size--;
  if (list->size == 0) {
    list->head = list->tail = NULL;
  }
  return removed_value;
}

void utl_list_resize(utl_list *list, unsigned int new_size, void *default_value) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return;
  }
  while (list->size > new_size) {
    utl_list_pop_back(list);
  }
  while (list->size < new_size) {
    void *new_value = malloc(list->item_size);
    if (!new_value) {
      utl_error_func("Memory allocation failed", utl_user_defined_data);
      return;
    }
    if (default_value != NULL) {
      memcpy(new_value, default_value, list->item_size);
    } 
    else {
      memset(new_value, 0, list->item_size); 
    }
    utl_list_push_back(list, new_value);
    free(new_value); 
  }
}

void utl_list_swap(utl_list *list1, utl_list *list2) {
  if (list1 == NULL || list2 == NULL) {
    utl_error_func("Null pointer provided for one or both lists", utl_user_defined_data);
    return;
  }
  utl_node *temp_head = list1->head; 
  list1->head = list2->head;
  list2->head = temp_head;
  utl_node *temp_tail = list1->tail; 
  list1->tail = list2->tail;
  list2->tail = temp_tail;
  unsigned int temp_size = list1->size; 
  list1->size = list2->size;
  list2->size = temp_size;
  unsigned int temp_item_size = list1->item_size; 
  list1->item_size = list2->item_size;
  list2->item_size = temp_item_size;
}

void utl_list_reverse(utl_list *list) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return;
  }
  if (list->head == NULL) {
    utl_error_func("Trying to reverse an empty list", utl_user_defined_data);
    return;
  }
  utl_node *current = list->head;
  utl_node *temp = NULL;
  while (current != NULL) {
    temp = current->prev;
    current->prev = current->next;
    current->next = temp;
    current = current->prev; 
  }
  temp = list->head; 
  list->head = list->tail;
  list->tail = temp;
}

void utl_list_sort(utl_list *list) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return;
  }
  if (list->size < 2 || list->compare == NULL) {
    return;
  }
  bool swapped;
  do {
    swapped = false;
    utl_node *current = list->head;
    while (current != NULL && current->next != NULL) {
      if (list->compare(current->value, current->next->value) > 0) {
        void *temp = current->value;
        current->value = current->next->value;
        current->next->value = temp;
        swapped = true;
      }
      current = current->next;
    }
  } while (swapped);
}

void utl_list_push_front(utl_list *list, void *value) {
  if (list == NULL || value == NULL) {
    utl_error_func("Null pointer provided for list or value", utl_user_defined_data);
    return;
  }
  utl_node *new_node = malloc(sizeof(utl_node));
  if (!new_node) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return;
  }
  new_node->value = malloc(list->item_size);
  if (!new_node->value) {
    free(new_node);
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return;
  }
  memcpy(new_node->value, value, list->item_size);
  new_node->next = list->head;
  new_node->prev = NULL;
  if (list->head != NULL) { 
    list->head->prev = new_node;
  }
  list->head = new_node;
  if (list->tail == NULL) { 
    list->tail = new_node;
  }
  list->size++;
}

void utl_list_push_back(utl_list *list, void *value) {
  if (list == NULL || value == NULL) {
    utl_error_func("Null pointer provided for list or value", utl_user_defined_data);
    return;
  }
  utl_node *new_node = malloc(sizeof(utl_node));
  if (!new_node) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return;
  }
  new_node->value = malloc(list->item_size);
  if (!new_node->value) {
    free(new_node);
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return;  
  }
  memcpy(new_node->value, value, list->item_size);
  new_node->next = NULL; 
  new_node->prev = list->tail;  
  if (list->tail != NULL) {
    list->tail->next = new_node;  
  }
  list->tail = new_node;  
  if (list->head == NULL) {
    list->head = new_node;  
  }
  list->size++;
}

void utl_list_pop_front(utl_list *list) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return;
  }
  if (list->head == NULL) {
    utl_error_func("Trying to pop from an empty list", utl_user_defined_data);
    return;  
  }
  utl_node *temp = list->head;
  list->head = list->head->next;  
  if (list->head != NULL) { 
    list->head->prev = NULL;  
  }
  else {
    list->tail = NULL;  
  }
  free(temp->value);  
  free(temp);
  list->size--;
}

void utl_list_pop_back(utl_list *list) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return;
  }
  if (list->head == NULL) {
    utl_error_func("Trying to pop from an empty list", utl_user_defined_data);
    return;  
  }
  if (list->head == list->tail) {
    free(list->head->value);
    free(list->head);
    list->head = list->tail = NULL;
  } 
  else {
    utl_node *last_node = list->tail;
    list->tail = last_node->prev;
    list->tail->next = NULL;
    free(last_node->value);
    free(last_node);
  }
  list->size--;
}

void utl_list_clear(utl_list *list) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return;
  }
  utl_node *current = list->head;
  while (current != NULL) {
    utl_node *next = current->next;
    free(current->value);
    free(current);
    current = next;
  }
  list->head = list->tail = NULL;
  list->size = 0;
}

bool utl_list_empty(const utl_list *list) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return true; 
  }
  return list->head == NULL;
}

unsigned int utl_list_length(const utl_list *list) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return 0;
  }
  return list->size;
}

void utl_list_deallocate(utl_list *list) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return;
  }
  utl_list_clear(list);
  free(list);
}

utl_node *utl_list_begin(const utl_list *list) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return NULL;
  }
  return list->head;
}

utl_node *utl_list_end(const utl_list *list) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return NULL;
  }
  return NULL; 
}

utl_node *utl_list_rbegin(const utl_list *list) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return NULL;
  }
  return list->tail;
}

utl_node *utl_list_rend(const utl_list *list) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return NULL;
  }
  return NULL;
}

const utl_node *utl_list_cbegin(const utl_list *list) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return NULL;
  }
  return list->head;
}

const utl_node *utl_list_cend(const utl_list* list) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return NULL;
  }
  return NULL;
}

const utl_node *utl_list_crbegin(const utl_list* list) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return NULL;
  }
  return list->tail;
}

const utl_node *utl_list_crend(const utl_list* list) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return NULL;
  }
  return NULL;
}

void utl_list_assign(utl_list *list, void *values, unsigned int num_values) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return;
  }
  if (values == NULL && num_values > 0) {
    utl_error_func("Null pointer provided for values", utl_user_defined_data);
    return;
  }
  utl_list_clear(list);
  for (unsigned int i = 0; i < num_values; ++i) {
    void *current_value = (char *)values + i * list->item_size;
    utl_list_push_back(list, current_value);
  }
}

void utl_list_emplace_front(utl_list *list, void *value) {
  if (list == NULL || value == NULL) {
    utl_error_func("Null pointer provided for list or value", utl_user_defined_data);
    return;
  }
  utl_node *new_node = malloc(sizeof(utl_node));
  if (!new_node) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return;
  }
  new_node->value = value;
  new_node->next = list->head;
  new_node->prev = NULL;
  if (list->head != NULL) { 
    list->head->prev = new_node;
  }
  list->head = new_node;
  if (list->tail == NULL) { 
    list->tail = new_node;
  }
  list->size++;
}

void utl_list_emplace_back(utl_list *list, void *value) {
  if (list == NULL || value == NULL) {
    utl_error_func("Null pointer provided for list or value", utl_user_defined_data);
    return;
  }
  utl_node *new_node = malloc(sizeof(utl_node));
  if (!new_node) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return;
  }
  new_node->value = value;
  new_node->next = NULL;
  new_node->prev = list->tail;
  if (list->tail != NULL) { 
    list->tail->next = new_node;
  }
  else {
    list->head = new_node;
  }
  list->tail = new_node;
  list->size++;
}

void utl_list_splice(utl_list *dest, utl_list *src, utl_node *pos) {
  if (dest == NULL || src == NULL) {
    utl_error_func("Null pointer provided for one or both lists", utl_user_defined_data);
    return;
  }
  if (dest == src) {
    utl_error_func("Cannot splice a list into itself", utl_user_defined_data);
    return;
  }
  if (src->head == NULL) {
    utl_error_func("Source list is empty, nothing to splice", utl_user_defined_data);
    return;
  }
  utl_node *src_first = src->head;
  utl_node *src_last = src->tail;
  if (pos == NULL) {
    if (dest->tail != NULL) { 
      dest->tail->next = src_first;
    }
    else { 
      dest->head = src_first;
    }
    src_first->prev = dest->tail;
    dest->tail = src_last;
  } 
  else {
    if (pos->prev != NULL) { 
      pos->prev->next = src_first;
    }
    else {
      dest->head = src_first;
    }
    src_first->prev = pos->prev;
    src_last->next = pos;
    pos->prev = src_last;
  }
  dest->size += src->size;
  src->head = src->tail = NULL;
  src->size = 0;
}

void utl_list_remove(utl_list *list, void *value) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return;
  }
  if (value == NULL) {
    utl_error_func("Null value provided for comparison", utl_user_defined_data);
    return;
  }
  if (list->compare == NULL) {
    utl_error_func("Null compare function provided", utl_user_defined_data);
    return;
  }
  utl_node *current = list->head;
  while (current != NULL) {
    utl_node *next = current->next;
    if (list->compare(current->value, value) == 0) {
      if (current->prev) {
        current->prev->next = next;
      }
      else {
        list->head = next;
      }
      if (next) { 
        next->prev = current->prev;
      }
      else { 
        list->tail = current->prev;
      }
      free(current->value);
      free(current);
      list->size--;
    }
    current = next;
  }
}

void utl_list_remove_if(utl_list *list, utl_condition_function cond) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return;
  }
  if (cond == NULL) {
    utl_error_func("Null condition function provided", utl_user_defined_data);
    return;
  }
  utl_node *current = list->head;
  while (current != NULL) {
    utl_node *next = current->next;
    if (cond(current->value)) {
      if (current->prev) {
        current->prev->next = next;
      }
      else { 
        list->head = next;
      }
      if (next) { 
        next->prev = current->prev;
      }
      else {
        list->tail = current->prev;
      }
      free(current->value);
      free(current);
      list->size--;
    }
    current = next;
  }
}

void utl_list_unique(utl_list *list) {
  if (list == NULL) {
    utl_error_func("Null pointer provided for list", utl_user_defined_data);
    return;
  }
  if (list->compare == NULL) {
    utl_error_func("Null compare function provided", utl_user_defined_data);
    return;
  }
  if (list->size < 2) {
    utl_error_func("No action needed, list has less than two elements", utl_user_defined_data);
    return;
  }
  utl_node *current = list->head;
  while (current != NULL && current->next != NULL) {
    if (list->compare(current->value, current->next->value) == 0) {
      utl_node *duplicate = current->next;
      current->next = duplicate->next;
      if (duplicate->next) {
        duplicate->next->prev = current;
      } 
      else { 
        list->tail = current;
      }
      free(duplicate->value);
      free(duplicate);
      list->size--;
    } 
    else {
      current = current->next;
    }
  }
}

void utl_list_merge(utl_list *list1, utl_list *list2) {
  if (list1 == NULL || list2 == NULL) {
    utl_error_func("Null pointer provided for one or both lists in list_merge.", utl_user_defined_data);
    return;
  }
  if (list1 == list2 || list2->size == 0) {
    utl_error_func("No merge needed, lists are identical or second list is empty", utl_user_defined_data);
    return;
  }
  if (list1->size == 0) {
    list1->head = list2->head;
    list1->tail = list2->tail;
    list1->size = list2->size;
    list2->head = list2->tail = NULL;
    list2->size = 0;
    return;
  }
  utl_node *current1 = list1->head;
  utl_node *current2 = list2->head;
  while (current1 != NULL && current2 != NULL) {
    if (list1->compare && list1->compare(current1->value, current2->value) > 0) {
      utl_node *next2 = current2->next;
      current2->prev = current1->prev;
      current2->next = current1;
      if (current1->prev) {
        current1->prev->next = current2;
      } 
      else { 
        list1->head = current2;
      }
      current1->prev = current2;
      current2 = next2;
      list2->size--;
      list1->size++;
    } 
    else {
      current1 = current1->next;
    }
  }
  if (current2 != NULL) {
    list1->tail->next = current2;
    current2->prev = list1->tail;
    list1->tail = list2->tail;
    list1->size += list2->size;
  }
  list2->head = list2->tail = NULL;
  list2->size = 0;
}

bool utl_list_is_less(const utl_list *list1, const utl_list *list2) {
  if (list1 == NULL || list2 == NULL) {
    utl_error_func("Null pointer provided for one or both lists", utl_user_defined_data);
    return false;
  }
  if (list1->size != list2->size) {
    return list1->size < list2->size;
  }
  utl_node *node1 = list1->head;
  utl_node *node2 = list2->head;
  while (node1 != NULL && node2 != NULL) {
    int val1 = *(int *)(node1->value);
    int val2 = *(int *)(node2->value);
    if (val1 != val2) {
      return val1 < val2;
    }
    node1 = node1->next;
    node2 = node2->next;
  }
  return false;
}

bool utl_list_is_greater(const utl_list *list1, const utl_list *list2) {
  if (list1 == NULL || list2 == NULL) {
    utl_error_func("Null pointer provided for one or both lists", utl_user_defined_data);
    return false;
  }
  bool result = utl_list_is_less(list2, list1);
  return result;
}

bool utl_list_is_equal(const utl_list *list1, const utl_list *list2) {
  if (list1 == NULL || list2 == NULL) {
    utl_error_func("Null pointer provided for one or both lists", utl_user_defined_data);
    return false;
  }
  if (list1->size != list2->size) {
    return false;
  }
  utl_node *node1 = list1->head;
  utl_node *node2 = list2->head;
  while (node1 != NULL && node2 != NULL) {
    int cmp_result = list1->compare(node1->value, node2->value);
    if (cmp_result != 0) {
      return false;
    }
    node1 = node1->next;
    node2 = node2->next;
  }
  return true;
}

bool utl_list_is_less_or_equal(const utl_list *list1, const utl_list *list2) {
  if (list1 == NULL || list2 == NULL) {
    utl_error_func("Null pointer provided for one or both lists", utl_user_defined_data);
    return false;
  }
  bool result = utl_list_is_less(list1, list2) || utl_list_is_equal(list1, list2);
  return result;
}

bool utl_list_is_greater_or_equal(const utl_list *list1, const utl_list *list2) {
  if (list1 == NULL || list2 == NULL) {
    utl_error_func("Null pointer provided for one or both lists", utl_user_defined_data);
    return false;
  }
  bool result = utl_list_is_greater(list1, list2) || utl_list_is_equal(list1, list2);
  return result;
}

bool utl_list_is_not_equal(const utl_list *list1, const utl_list *list2) {
  if (list1 == NULL || list2 == NULL) {
    utl_error_func("Null pointer provided for one or both lists", utl_user_defined_data);
    return true; 
  }
  bool result = !utl_list_is_equal(list1, list2);
  return result;
}

