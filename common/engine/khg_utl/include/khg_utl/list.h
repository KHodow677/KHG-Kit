#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef bool (*utl_condition_function)(void *);
typedef int (*utl_compare_function)(const void *, const void *);

typedef struct utl_node utl_node;
struct utl_node {
  void *value;
  utl_node *next;
  utl_node *prev;  
};

typedef struct utl_list {
  utl_node *head;
  utl_node *tail;
  unsigned int size;
  unsigned int item_size;
  utl_condition_function condition;
  utl_compare_function compare;
} utl_list;

utl_list *utl_list_create(unsigned int item_size, utl_compare_function compare);
unsigned int utl_list_length(const utl_list *list);

void *utl_list_front(const utl_list *list);
void *utl_list_back(const utl_list *list);
void *utl_list_insert(utl_list *list, unsigned int index, void *value);
void *utl_list_erase(utl_list *list, unsigned int index);

void utl_list_resize(utl_list *list, unsigned int new_size, void *default_value);
void utl_list_swap(utl_list *list1, utl_list *list2);
void utl_list_reverse(utl_list *list);
void utl_list_sort(utl_list* list);
void utl_list_push_front(utl_list *list, void *value);
void utl_list_push_back(utl_list *list, void *value);
void utl_list_pop_front(utl_list *list);
void utl_list_pop_back(utl_list *list);
void utl_list_clear(utl_list *list);
void utl_list_assign(utl_list *list, void *values, unsigned int num_values);
void utl_list_emplace_front(utl_list *list, void *value);
void utl_list_emplace_back(utl_list *list, void *value);
void utl_list_splice(utl_list *dest, utl_list *src, utl_node *pos);
void utl_list_remove(utl_list *list, void *value);
void utl_list_remove_if(utl_list *list, utl_condition_function cond);
void utl_list_unique(utl_list *list);
void utl_list_merge(utl_list *list1, utl_list *list2);
void utl_list_deallocate(utl_list *list);

utl_node *utl_list_begin(const utl_list *list);
utl_node *utl_list_end(const utl_list *list);
utl_node *utl_list_rbegin(const utl_list *list);
utl_node *utl_list_rend(const utl_list *list); 

const utl_node *utl_list_cbegin(const utl_list *list);
const utl_node *utl_list_cend(const utl_list* list);
const utl_node *utl_list_crbegin(const utl_list* list);
const utl_node *utl_list_crend(const utl_list* list);

bool utl_list_is_less(const utl_list *list1, const utl_list *list2);
bool utl_list_is_greater(const utl_list *list1, const utl_list *list2);
bool utl_list_is_equal(const utl_list *list1, const utl_list *list2);
bool utl_list_is_less_or_equal(const utl_list *list1, const utl_list *list2);
bool utl_list_is_greater_or_equal(const utl_list *list1, const utl_list *list2);
bool utl_list_is_not_equal(const utl_list *list1, const utl_list *list2);
bool utl_list_empty(const utl_list *list);

