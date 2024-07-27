#pragma once

#include <stdlib.h>

typedef struct net_list_node {
  struct net_list_node *next;
  struct net_list_node *previous;
} net_list_node;

typedef net_list_node *net_list_iterator;

typedef struct net_list {
  net_list_node sentinel;
} net_list;

extern void net_list_clear(net_list *);

extern net_list_iterator net_list_insert(net_list_iterator, void *);
extern void *net_list_remove(net_list_iterator);
extern net_list_iterator net_list_move(net_list_iterator, void *, void *);

extern size_t net_list_size(net_list *);

#define net_list_begin(list) ((list)->sentinel.next)
#define net_list_end(list) (&(list)->sentinel)

#define net_list_empty(list) (net_list_begin(list) == net_list_end(list))

#define net_list_next(iterator) ((iterator)->next)
#define net_list_previous(iterator) ((iterator)->previous)

#define net_list_front(list) ((void *)(list)->sentinel.next)
#define net_list_back(list) ((void *)(list)->sentinel.previous)
