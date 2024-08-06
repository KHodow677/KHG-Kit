#include "khg_dbm/rows.h"
#include "khg_dbm/util.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

size_t dbm_rowlen(row **in_row) {
	return strlen((*in_row)->types);
}

row *dbm_create_row(char *types) {
  if(dbm_verify_types(types)) {
		row *new_row = (row *)malloc(sizeof(row));
		new_row->columns = (dbm_generic **)malloc(sizeof(dbm_generic *) * strlen(types));
		new_row->types = types;
		for(int i = 0; i < dbm_rowlen(&new_row); i++) {
			dbm_create_def_generic(&new_row->columns[i], dbm_get_type(new_row->types[i]));
		}
		return new_row;
	} 
  else {
		return NULL;
	}
}

dbm_i dbm_r_get_type(row **in_row, int index) {
	return (*in_row)->columns[index]->type;
}

int dbm_r_get_int(row **in_row, int index) {
	return dbm_get_int(&(*in_row)->columns[index]);
}

void dbm_r_set_int(row **in_row, int index, int value) {
	dbm_set_int(&(*in_row)->columns[index], &value);
}

char dbm_r_get_char(row **in_row, int index) {
	return dbm_get_char(&(*in_row)->columns[index]);
}

void dbm_r_set_char(row **in_row, int index, char value) {
	dbm_set_int(&(*in_row)->columns[index], &value);
}

char *dbm_r_get_str(row **in_row, int index) {
	return dbm_get_str(&(*in_row)->columns[index]);
}

void dbm_r_set_str(row **in_row, int index, char *value) {
	dbm_set_str(&(*in_row)->columns[index], value);
}

void dbm_print_row(row **in_row){
	for(int i = 0; i < dbm_rowlen(in_row); i++) {
		switch(dbm_r_get_type(in_row, i)) {
			case int_t:
				printf("%i\t", dbm_r_get_int(in_row, i));
				break;
			case char_t:
				printf("%c\t", dbm_r_get_char(in_row, i));
				break;
			case str_t:
				printf("%s\t", dbm_r_get_str(in_row, i));
				break;
			default:
				printf("NULL\t");
				break;
		}
	}
	printf("\n");
}

size_t dbm_pack_row(row **in_row, char **buf) {
	*buf = malloc(dbm_get_size(in_row));
	size_t pos = 0;
	int value_i;
	char value_c;
	char *value_s;
	for(int i = 0; i < dbm_rowlen(in_row); i++) {
		switch(dbm_r_get_type(in_row, i)) {
			case int_t:
				value_i = dbm_r_get_int(in_row, i);
				memcpy(*buf + pos, &value_i, sizeof(int));
				pos += sizeof(int);
				break;
			case char_t:
				value_c = dbm_r_get_char(in_row, i);
				memcpy(*buf + pos, &value_c, sizeof(char));
				pos += sizeof(char);
				break;
			case str_t:
				value_s = dbm_r_get_str(in_row, i);
				pos += pack_string(*buf + pos, value_s);
        free(value_s);
				break;
			default:
				break;
		}
	}
	return pos;
}

row *dbm_unpack_row(char *types, char **buf) {
	row *new_row = dbm_create_row(types);
	size_t pos = 0;
	int value_i;
	char value_c;
	size_t len;
	char *value_s;
	for (int i = 0; i < dbm_rowlen(&new_row); i++) {
		switch(dbm_r_get_type(&new_row, i)) {
			case int_t:
				value_i = 0;
				memcpy(&value_i, *buf + pos, sizeof(int));
				dbm_r_set_int(&new_row, i, value_i);
				pos += sizeof(int);
				break;
			case char_t:
				value_c = 'a';
				memcpy(&value_c, *buf + pos, sizeof(char));
				dbm_r_set_char(&new_row, i, value_c);
				pos += sizeof(char);
				break;
			case str_t:
				memcpy(&len, *buf + pos, sizeof(size_t));
				pos+= sizeof(size_t);
				value_s = (char *) malloc(len);
				memcpy(value_s, *buf + pos, len);
				dbm_r_set_str(&new_row, i, value_s);
				free(value_s);
				pos+= len;
				break;
			default:
				break;
		}
	}
	return new_row;
}

size_t dbm_get_size(row **in_row){
	size_t rowsize = 0;
	for(int i = 0; i < dbm_rowlen(in_row); i++) {
		switch(dbm_r_get_type(in_row, i)) {
			case int_t:
				rowsize += sizeof(int);
				break;
			case char_t:
				rowsize += sizeof(char);
				break;
			case str_t:
				rowsize += sizeof(size_t) + sizeof(char) * (strlen(dbm_r_get_str(in_row, i)) + 1);
				break;
			default:
				break;
		}
	}
	return rowsize;
}
