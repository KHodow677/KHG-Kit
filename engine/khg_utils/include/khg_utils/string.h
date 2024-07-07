#pragma once

#include "stdlib.h"

typedef char *string;

typedef struct {
  size_t cap;
  size_t len;
  char data[];
} _string;

string _string_create();
string _string_create_from_string(string _str);
int _string_free(string _str);
_string* _string_struct(string _str);
_string* _string_realloc(_string* _str, size_t _data_size);
int _string_calc_capacity(size_t* _cap, size_t _dest);
size_t _string_length(string _str);
size_t _string_capacity(string _str);
int _string_empty(string _str);
int _string_compare(string str1, string str2);
int _string_inc(string* _str, size_t _count);
char _string_get(string _str, size_t _index);
int _string_set(string _str, size_t _index, char _chr);
int _string_add(string* _dst, char* _src);
int _string_add_char(string* _dst, char _chr);
int _string_insert(string* _dst, size_t _index, char* _src);
int _string_insert_char(string* _dst, size_t _index, char _chr);
int _string_delete(string* _str, size_t _index, size_t _num_chars);
int _string_clear(string* _str);
int _string_reverse(string _str);
int _string_fill(string _str, char _chr);
int _string_fill_range(string _str, size_t _index, size_t _num_chars, char _chr);
string _string_substr(string _str, size_t _index, size_t _num_chars);
string _string_clone(string _str);
int _string_find_from(string _str, char* _substr, size_t _index);
int _string_find_char_from(string _str, char _chr, size_t _index);
int _string_replace(string* _str, char* _substr, char* _new_substr, int _first);
size_t _string_count(string _str, char* _substr);
size_t _string_count_char(string _str, char _chr);
int _string_resize(string* _str, size_t _num_chars);
int _string_reserve(string* _str, size_t _num_chars);
int _string_shrink(string* _str);

#define str_create() _string_create()
#define str_create_from_str(str) _string_create_from_string(str)
#define str_free(str) _string_free(str)
#define str_length(str) _string_length(str)
#define str_size(str) _string_length(str)
#define str_capacity(str) _string_capacity(str)
#define str_empty(str) _string_empty(str)
#define str_compare(str1, str2) _string_compare(str1, str2)
#define str_get(str, index) _string_get(str, index)
#define str_set(str, index, chr) _string_set(str, index, chr)
#define str_add(dst, src) _string_add(&dst, src)
#define str_add_char(dst, chr) _string_add_char(&dst, chr)
#define str_insert(dst, index, src) _string_insert(&dst, index, src)
#define str_insert_char(dst, index, chr) _string_insert_char(&dst, index, chr)
#define str_delete(str, index, num_chars) _string_delete(&str, index, num_chars)
#define str_clear(str) _string_clear(&str)
#define str_reverse(str) _string_reverse(str)
#define str_fill(str, chr) _string_fill(str, chr)
#define str_fill_range(str, index, num_chars, chr) _string_fill_range(str, index, num_chars, chr)
#define str_clone(str) _string_clone(str)
#define str_substr(str, index, num_chars) _string_substr(str, index, num_chars)
#define str_find(str, substr) _string_find_from(str, substr, 0)
#define str_find_char(str, chr) _string_find_char_from(str, chr, 0)
#define str_find_from(str, substr, index) _string_find_from(str, substr, index)
#define str_find_char_from(str, chr, index) _string_find_char_from(str, chr, index)
#define str_replace_all(str, substr, new_substr) _string_replace(&str, substr, new_substr, 0)
#define str_replace_first(str, substr, new_substr) _string_replace(&str, substr, new_substr, 1)
#define str_count(str, substr) _string_count(str, substr)
#define str_count_char(str, chr) _string_count_char(str, chr)
#define str_resize(str, num_chars) _string_resize(&str, num_chars)
#define str_reserve(str, num_chars) _string_reserve(&str, num_chars)
#define str_shrink(str) _string_shrink(&str)

