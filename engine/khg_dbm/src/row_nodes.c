#include <stdlib.h>
#include <string.h>
#include "khg_dbm/row_nodes.h"

dbm_row_node *dbm_create_row_node(row **row_val) {
	dbm_row_node *new_node = (dbm_row_node *)malloc(sizeof(dbm_row_node));
	new_node->data = *row_val;
	new_node->next = NULL;
	return new_node;
}

void dbm_add_node(dbm_row_node **parent_node, dbm_row_node **child_node) {
	(*parent_node)->next = *child_node;
}

dbm_row_node *dbm_add_row(dbm_row_node **parent_node, row **new_row) {
	dbm_row_node *new_node = dbm_create_row_node(new_row);
	dbm_add_node(parent_node, &new_node);
  return new_node;
}

void dbm_print_row_nodes(dbm_row_node *root) {
	dbm_row_node *it = root;
	while(it!=NULL) {
		print_row(&it->data);
		it = it->next;
	}
}

size_t dbm_pack_row_nodes(dbm_row_node *root, char **buf) {
	size_t row_sz = 0;
	size_t pos = 0;
	char *rownode_buf = malloc(sizeof(char));
	char *row_buf;
	dbm_row_node *it = root;
	while (it!=NULL) {
    if (it->data !=NULL) {
      row_sz = pack_row(&it->data, &row_buf);
      rownode_buf = (char *)realloc(rownode_buf, pos + row_sz + sizeof(size_t));
      memcpy(rownode_buf + pos, &row_sz, sizeof(size_t));
      pos+=sizeof(size_t);
      memcpy(rownode_buf + pos, row_buf, row_sz);
      pos+=row_sz;
      free(row_buf);
    }
		it = it-> next;
	}
	(*buf) = rownode_buf;
	return pos;
}

void dbm_unpack_row_nodes(char *types, dbm_row_node **root, char **buf, size_t buf_sz) {
	size_t pos = 0;
	size_t row_sz = 0;
	char *row_buf;
  *root = NULL;
	dbm_row_node *it;
	while(pos < buf_sz) {
		memcpy(&row_sz, (*buf) + pos, sizeof(size_t));
		pos+= sizeof(size_t);
		row_buf = (char *)malloc(sizeof(char) * row_sz);
		memcpy(row_buf, (*buf) + pos, row_sz);
		pos+= row_sz;
		row* cur_row = unpack_row(types, &row_buf);
		free(row_buf);
    if(*root == NULL) {
			*root = dbm_create_row_node(&cur_row);
			it = *root;
		} 
    else {
			dbm_add_row(&it, &cur_row);
			it = it->next;
		}
	}
}
