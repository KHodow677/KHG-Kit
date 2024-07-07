#include "khg_utils/string.h"
#include <stdlib.h>
#include <string.h>

#define _STRING_MAX_CAPACITY (size_t)-1

string _string_create(){
  _string *str = (_string*)malloc(sizeof(_string) + 1);
  if(str == NULL) {
    return NULL;
  }
  str->cap = 1;
  str->len = 0;
  str->data[0] = '\0';
  return (string)(&str->data);
}

string _string_create_from_string(string _str){
  size_t len = strlen(_str);
  size_t cap = 0;
  if(!_string_calc_capacity(&cap, len + 1)) {
    return NULL;
  }
  _string *str = (_string *)malloc(sizeof(_string) + cap);
  if(str == NULL) {
    return NULL;
  }
  str->cap = cap;
  str->len = len;
  for(size_t i = 0; i < len; ++i) {
    str->data[i] = _str[i];
  }
  str->data[len] = '\0';
  return (string)(&str->data);
}

int _string_free(string _str){
  if(_str == NULL) {
    return 0;
  }
  free(_string_struct(_str));
  return 1;
}

_string *_string_struct(string _str){
  if(_str == NULL) {
    return NULL;
  }
  return ((_string*)_str) - 1;
}

_string *_string_realloc(_string *_str, size_t _data_size){
  if(_str == NULL) {
    return NULL;
  }
  _string *new_str = (_string*)realloc(_str, sizeof(_string) + _data_size);
  if(new_str == NULL) {
    return NULL;
  }
  return new_str;
}

int _string_calc_capacity(size_t *_cap, size_t _dest){
  (*_cap) = 0;
  if(_dest == 0) {
    return 1;
  }
  size_t new_cap = 1;
  size_t prev_cap;
  while(new_cap < _dest){
    prev_cap = new_cap;
    new_cap += (new_cap > 1) ? (new_cap / 2) : 1;
    if(new_cap < prev_cap){
      new_cap = _STRING_MAX_CAPACITY;
      break;
    }
  }
  if(new_cap < _dest) {
    return 0;
  }
  (*_cap) = new_cap;
  return 1;
}

size_t _string_length(string _str){
  if(_str == NULL) {
    return 0;
  }
  return _string_struct(_str)->len;
}

size_t _string_capacity(string _str){
  if(_str == NULL) {
    return 0;
  }
  return _string_struct(_str)->cap;
}

int _string_empty(string _str){
  if(_str == NULL) {
    return 0;
  }
  return (_string_struct(_str)->len == 0);
}

int _string_compare(string str1, string str2){
  if(str1 == NULL || str2 == NULL) {
    return 0;
  }
  return (strcmp(str1, str2) == 0);
}

int _string_inc(string *_str, size_t _count){
  if(_str == NULL || _count == 0) {
    return 0;
  }
  if((*_str) == NULL) {
    return 0;
  }
  _string *str = _string_struct(*_str);
  size_t new_len = str->len + _count;
  if(new_len < str->len) {
    return 0;
  }
  if(new_len + 1 > str->cap){
    size_t new_cap = 0;
    if(!_string_calc_capacity(&new_cap, new_len + 1)) {
      return 0;
    }
    _string *new_str = _string_realloc(str, new_cap);
    if(new_str == NULL) {
      return 0;
    }
    str = new_str;
    str->cap = new_cap;
    *_str = (string)(&str->data);
  }
  str->len = new_len;
  return 1;
}

char _string_get(string _str, size_t _index){
  if(_str == NULL) {
    return '\0';
  }
  _string *str = _string_struct(_str);
  if(_index >= str->len) {
    return '\0';
  }
  return _str[_index];
}

int _string_set(string _str, size_t _index, char _chr){
  if(_str == NULL || _chr == '\0') {
    return 0;
  }
  _string *str = _string_struct(_str);
  if(_index >= str->len) {
    return 0;
  }
  _str[_index] = _chr;
  return 1;
}

int _string_add(string *_dst, char *_src){
  if(_dst == NULL) {
    return 0;
  }
  if((*_dst) == NULL) {
    return 0;
  }
  if((*_src) == '\0') {
    return 1;
  }
  _string *str = _string_struct(*_dst);
  size_t src_len = strlen(_src);
  size_t old_len = str->len;
  if(!_string_inc(_dst, src_len)) {
    return 0;
  }
  str = _string_struct(*_dst);
  size_t end = old_len + src_len;
  for(size_t i = old_len; i < end; ++i) {
    str->data[i] = _src[i - old_len];
  }
  str->data[str->len] = '\0';
  return 1;
}

int _string_add_char(string *_dst, char _chr){
  if(_dst == NULL) {
    return 0;
  }
  if((*_dst) == NULL) {
    return 0;
  }
  if(_chr == '\0') {
    return 0;
  }
  _string *str = _string_struct(*_dst);
  if(!_string_inc(_dst, 1)) {
    return 0;
  }
  str = _string_struct(*_dst);
  str->data[str->len - 1] = _chr;
  str->data[str->len] = '\0';
  return 1;
}

int _string_insert(string *_dst, size_t _index, char *_src){
  if(_dst == NULL) {
    return 0;
  }
  if((*_dst) == NULL) {
    return 0;
  }
  _string *str = _string_struct(*_dst);
  if(_index > str->len) {
    return 0;
  }
  if((*_src) == '\0') {
    return 1;
  }
  size_t old_len = str->len;
  size_t src_len = strlen(_src);
  if(!_string_inc(_dst, src_len)) {
    return 0;
  }
  str = _string_struct(*_dst);
  if(_index < old_len) {
    memmove(str->data + _index + src_len, str->data + _index, (old_len - _index));
  }
  size_t end = _index + src_len;
  for(size_t i = _index; i < end; ++i) {
    str->data[i] = _src[i - _index];
  }
  str->data[str->len] = '\0';
  return 1;
}

int _string_insert_char(string *_dst, size_t _index, char _chr){
  if(_dst == NULL) {
    return 0;
  }
  if((*_dst) == NULL) {
    return 0;
  }
  if(_chr == '\0') {
    return 0;
  }
  _string *str = _string_struct(*_dst);
  if(_index > str->len) {
    return 0;
  }
  size_t old_len = str->len;
  if(!_string_inc(_dst, 1)) {
    return 0;
  }
  str = _string_struct(*_dst);
  if(_index < old_len) {
    memmove(str->data + _index + 1, str->data + _index, (old_len - _index));
  }
  str->data[_index] = _chr;
  str->data[str->len] = '\0';
  return 1;
}

int _string_delete(string *_str, size_t _index, size_t _num_chars){
  if(_str == NULL || _num_chars == 0) {
    return 0;
  }
  if((*_str) == NULL) {
    return 0;
  }
  _string *str = _string_struct(*_str);
  if(_index >= str->len) {
    return 0;
  }
  if(_index + _num_chars - 1 < _index) {
    return 0;
  }
  if(_index + _num_chars - 1 >= str->len) {
    _num_chars = (str->len - _index);
  }
  size_t new_len = str->len - _num_chars;
  size_t old_cap = str->cap;
  size_t new_cap = old_cap;
  if(!_string_calc_capacity(&new_cap, new_len + 1)) {
    return 0;
  }
  if(_index + _num_chars - 1 < str->len - 1) {
    memmove(str->data + _index, str->data + _index + _num_chars, str->len - (_index + _num_chars));
  }
  if(old_cap != new_cap){
    _string *new_str = _string_realloc(str, new_cap);
    if(new_str == NULL) {
      return 0;
    }
    str = new_str;
    *_str = (string)(&str->data);
  }
  str->len = new_len;
  str->cap = new_cap;
  str->data[new_len] = '\0';
  return 1;
}

int _string_clear(string *_str){
  if(_str == NULL) {
    return 0;
  }
  if((*_str) == NULL) {
    return 0;
  }
  _string *str = _string_struct(*_str);
  if(str->cap <= 1) {
    return 1;
  }
  _string *new_str = _string_realloc(str, 1);
  if(new_str == NULL) {
    return 0;
  }
  new_str->len = 0;
  new_str->cap = 1;
  new_str->data[0] = '\0';
  *_str = (string)(&new_str->data);
  return 1;
}

int _string_reverse(string _str){
  if(_str == NULL) {
    return 0;
  }
  _string *str = _string_struct(_str);
  size_t middle = (str->len / 2) + ((str->len & 0x1) == 1);
  char temp;
  for(size_t i = 0; i < middle; ++i){
    temp = str->data[i];
    str->data[i] = str->data[str->len - i - 1];
    str->data[str->len - i - 1] = temp;
  }
  return 1;
}

int _string_fill(string _str, char _chr){
  if(_str == NULL || _chr == '\0') {
    return 0;
  }
  _string *str = _string_struct(_str);
  for(size_t i = 0; i < str->len; ++i) {
    _str[i] = _chr;
  }
  return 1;
}

int _string_fill_range(string _str, size_t _index, size_t _num_chars, char _chr){
  if(_str == NULL || _chr == '\0') {
    return 0;
  }
  _string *str = _string_struct(_str);
  for(size_t i = _index; i < _index + _num_chars; ++i){
    _str[i] = _chr;
    if(i >= str->len - 1) break;
  }
  return 1;
}

string _string_clone(string _str){
  if(_str == NULL) {
    return NULL;
  }
  _string *str = _string_struct(_str);
  _string *new_str = (_string*)malloc(sizeof(_string) + str->cap);
  if(new_str == NULL) {
    return NULL;
  }
  new_str->cap = str->cap;
  new_str->len = str->len;
  for(size_t i = 0; i < new_str->len + 1; ++i) {
    new_str->data[i] = _str[i];
  }
  return (string)(&new_str->data);
}

string _string_substr(string _str, size_t _index, size_t _num_chars){
  if(_str == NULL || _num_chars == 0) {
    return NULL;
  }
  _string *str = _string_struct(_str);
  if(_index >= str->len) {
    return NULL;
  }
  if(str->len - _index < _num_chars) {
    _num_chars = (str->len - _index);
  }
  _string *substr = (_string*)malloc(sizeof(_string) + _num_chars + 1);
  if(substr == NULL) {
    return NULL;
  }
  substr->cap = _num_chars + 1;
  substr->len = _num_chars;
  for(size_t i = 0; i < _num_chars; ++i) {
    substr->data[i] = _str[i + _index];
  }
  substr->data[_num_chars] = '\0';
  return (string)(&substr->data);
}

int _string_find_from(string _str, char *_substr, size_t _index){
  if(_str == NULL || _substr == NULL) {
    return -1;
  }
  _string *str = _string_struct(_str);
  size_t sub_len = strlen(_substr);
  if(sub_len == 0 || _index >= str->len) {
    return -1;
  }
  for(size_t i = _index; i < str->len; ++i){
    for(size_t j = 0; j < sub_len; ++j){
      if(_str[i + j] != _substr[j]) {
        break;
      }
      if(j == sub_len - 1) {
        return i;
      }
    }
  }
  return -1;
}

int _string_find_char_from(string _str, char _chr, size_t _index){
  if(_str == NULL || _chr == '\0') {
    return -1;
  }
  _string *str = _string_struct(_str);
  for(size_t i = _index; i < str->len; ++i){
    if(_str[i] == _chr) {
      return i;
    }
  }
  return -1;
}

int _string_replace(string *_str, char *_substr, char *_new_substr, int _first){
  if(_str == NULL || _substr == NULL) {
    return 0;
  }
  if((*_str) == NULL || (*_substr) == '\0') {
    return 0;
  }
  size_t sub_len = strlen(_substr);
  size_t new_sub_len = _new_substr == NULL ? 0 : strlen(_new_substr);
  size_t count = _string_count(*_str, _substr);
  if(count == 0) {
    return 1;
  }
  if(_first) {
    count = 1;
  }
  _string *str = _string_struct(*_str);
  size_t old_len = str->len;
  size_t new_len = old_len - (count  *sub_len) + (count  *new_sub_len);
  if(new_len > str->len){
    _string *new_str = _string_realloc(str, new_len + 1);
    if(new_str == NULL) {
      return 0;
    }
    str = new_str;
    str->cap = new_len + 1;
  }
  str->len = str->len;
  size_t offset = 0;
  size_t index;
  int pos;
  while((pos = _string_find_from(str->data, _substr, offset)) >= 0){
    index = (size_t)pos;
    memmove(str->data + index + new_sub_len, str->data + index + sub_len, str->len - (index + sub_len) + 1);
    str->len = str->len - sub_len + new_sub_len;
    for(size_t i = 0; i < new_sub_len; ++i) {
      str->data[index + i] = _new_substr[i];
    }
    offset = index + new_sub_len;
    if(_first) {
      break;
    }
  }
  if(str->len < old_len){
    _string *new_str = _string_realloc(str, new_len + 1);
    if(new_str == NULL) {
      return 0;
    }
    str = new_str;
    str->cap = new_len + 1;
  }
  *_str = (string)(&str->data);
  return 1;
}

size_t _string_count(string _str, char *_substr){
  if(_str == NULL || _substr == NULL) {
    return 0;
  }
  _string *str = _string_struct(_str);
  size_t sub_len = strlen(_substr);
  if(sub_len == 0) {
    return 0;
  }
  size_t count = 0;
  for(size_t i = 0; i < str->len; ++i){
    for(size_t j = 0; j < sub_len; ++j){
      if(_str[i + j] != _substr[j]) {
        break;
      }
      if(j == sub_len - 1) {
        ++count;
      }
    }
  }
  return count;
}

size_t _string_count_char(string _str, char _chr){
  if(_str == NULL || _chr == '\0') {
    return 0;
  }
  _string *str = _string_struct(_str);
  size_t count = 0;
  for(size_t i = 0; i < str->len; ++i){
    if(_str[i] == _chr) {
      ++count;
    }
  }
  return count;
}

int _string_resize(string *_str, size_t _num_chars){
  if(_str == NULL) {
    return 0;
  }
  if((*_str) == NULL) {
    return 0;
  }
  _string *str = _string_struct(*_str);
  if(str->len == _num_chars) {
    return 1;
  }
  size_t new_cap = 0;
  if(!_string_calc_capacity(&new_cap, _num_chars + 1)) {
    return 0;
  }
  if(new_cap != str->cap){
    _string *new_str = _string_realloc(str, new_cap);
    if(new_str == NULL) {
      return 0;
    }
    str = new_str;
    *_str = (string)(&str->data);
  }
  str->cap = new_cap;
  str->len = _num_chars;
  str->data[_num_chars] = '\0';
  return 1;
}

int _string_reserve(string *_str, size_t _num_chars){
  if(_str == NULL) {
    return 0;
  }
  if((*_str) == NULL) {
    return 0;
  }
  _string *str = _string_struct(*_str);
  if(str->cap >= _num_chars + 1) {
    return 1;
  }
  _string *new_str = _string_realloc(str, _num_chars + 1);
  if(new_str == NULL) {
    return 0;
  }
  new_str->cap = _num_chars + 1;
  *_str = (string)(&new_str->data);
  return 1;
}

int _string_shrink(string *_str){
  if(_str == NULL) {
    return 0;
  }
  if((*_str) == NULL) {
    return 0;
  }
  _string *str = _string_struct(*_str);
  if(str->cap == str->len + 1) {
    return 1;
  }
  _string *new_str = _string_realloc(str, str->len + 1);
  if(new_str == NULL) {
    return 0;
  }
  new_str->cap = new_str->len + 1;
  *_str = (string)(&new_str->data);
  return 1;
}
