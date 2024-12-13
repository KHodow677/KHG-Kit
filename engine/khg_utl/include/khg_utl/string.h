#pragma once

#include <stddef.h>
#include <stdbool.h>

extern const char *UTL_STRING_ASCII_LETTERS;
extern const char *UTL_STRING_ASCII_LOWERCASE;
extern const char *UTL_STRING_ASCII_UPPERCASE;
extern const char *UTL_STRING_DIGITS;
extern const char *UTL_STRING_HEXDIGITS;
extern const char *UTL_STRING_WHITESPACE;
extern const char *UTL_STRING_PUNCTUATION;

typedef struct string utl_string;
typedef struct memory_pool_string {
  void *pool;
  size_t pool_size;
  size_t used;
} utl_memory_pool_string;

struct string {
  char *data_str;
  size_t size;
  size_t capacity_size;
  utl_memory_pool_string *pool;
};

char utl_string_at(utl_string *str, size_t index);
float utl_string_to_float(utl_string *str);
double utl_string_to_double(utl_string *str);
wchar_t *utl_string_to_unicode(const char *str);

utl_string *utl_string_create(const char *initial_str);
utl_string *utl_string_create_with_pool(size_t size);
utl_string *utl_string_substr(utl_string *str, size_t pos, size_t len);
utl_string **utl_string_split(utl_string *str, const char *delimiter, int *count);
utl_string *utl_string_join(utl_string **strings, int count, const char *delimiter);
utl_string *utl_string_from_int(int value);
utl_string *utl_string_from_float(float value);
utl_string *utl_string_from_double(double value);
utl_string **utl_string_tokenize(utl_string *str, const char *delimiters, int *count);
utl_string *utl_string_from_unicode(const wchar_t *wstr);
utl_string **utl_string_create_from_initializer(size_t count, ...);
utl_string *utl_string_to_hex(utl_string *str);
utl_string *utl_string_from_hex(utl_string *hex_str);
utl_string *utl_string_base64_encode(const utl_string *input);
utl_string *utl_string_base64_decode(const utl_string *input);
utl_string *utl_string_repeat(const utl_string *str, size_t count);
utl_string *utl_string_join_variadic(size_t count, ...);

bool utl_string_is_equal(utl_string *str1, utl_string *str2);
bool utl_string_is_less(utl_string *str1, utl_string *str2);
bool utl_string_is_greater(utl_string *str1, utl_string *str2);
bool utl_string_is_less_or_equal(utl_string *str1, utl_string *str2);
bool utl_string_is_greater_or_equal(utl_string *str1, utl_string *str2);
bool utl_string_is_not_equal(utl_string *str1, utl_string *str2);
bool utl_string_is_alpha(utl_string *str);
bool utl_string_is_digit(utl_string *str);
bool utl_string_is_lower(utl_string *str);
bool utl_string_is_upper(utl_string *str);
bool utl_string_empty(utl_string *str);
bool utl_string_contains(utl_string *str, const char *substr);
bool utl_string_set_pool_size(utl_string *str, size_t newSize);
bool utl_string_starts_with(const utl_string *str, const char *substr);
bool utl_string_ends_with(const utl_string *str, const char *substr);
bool utl_string_to_bool_from_cstr(const char *boolstr);

int utl_string_compare(const utl_string *str1, const utl_string *str2);
int utl_string_find(utl_string *str, const char *buffer, size_t pos);
int utl_string_rfind(utl_string *str, const char *buffer, size_t pos);
int utl_string_find_first_of(utl_string *str, const char *buffer, size_t pos);
int utl_string_find_last_of(utl_string *str, const char *buffer, size_t pos);
int utl_string_find_first_not_of(utl_string *str, const char *buffer, size_t pos);
int utl_string_find_last_not_of(utl_string *str, const char *buffer, size_t pos);
int utl_string_compare_ignore_case(utl_string *str1, utl_string *str2);
int utl_string_to_int(utl_string *str);
int utl_string_strcmp(const char *str1, const char *str2);

void utl_string_reverse(utl_string *str);
void utl_string_resize(utl_string *str, size_t newSize);
void utl_string_shrink_to_fit(utl_string *str);
void utl_string_append(utl_string *str, const char *str_item);
void utl_string_push_back(utl_string *str, const char ch_item); 
void utl_string_assign(utl_string *str, const char *new_str);
void utl_string_insert(utl_string *str, size_t pos, const char *str_item);
void utl_string_erase(utl_string *str, size_t pos, size_t len);
void utl_string_replace(utl_string *str1, const char *old_str, const char *new_str);
void utl_string_swap(utl_string *str1, utl_string *str2);
void utl_string_pop_back(utl_string *str);
void utl_string_deallocate(utl_string *str);
void utl_string_clear(utl_string *str);

char *utl_string_to_upper(utl_string *str);
char *utl_string_to_lower(utl_string *str);
char *utl_string_begin(utl_string *str);
char *utl_string_end(utl_string *str);
char *utl_string_rbegin(utl_string *str);
char *utl_string_rend(utl_string *str);
char *utl_string_back(utl_string *str);
char *utl_string_front(utl_string *str);
char *utl_string_strdup(const char *s);
char *utl_string_from_int_cstr(int value);

size_t utl_string_length_cstr(const char *str);
size_t utl_string_length_utf8(const char *str);
size_t utl_string_length(utl_string *str);
size_t utl_string_capacity(utl_string *str);
size_t utl_string_max_size(utl_string *str);
size_t utl_string_copy(utl_string *str, char *buffer, size_t pos, size_t len);
size_t utl_string_count(utl_string *str, const char *substr);
size_t utl_string_utf8_char_len(char c);

const char* utl_string_data(utl_string *str);
const char* utl_string_c_str(const utl_string *str);
const char* utl_string_cbegin(utl_string *str);
const char* utl_string_cend(utl_string *str);
const char* utl_string_crbegin(utl_string *str);
const char* utl_string_crend(utl_string *str);

void utl_string_deallocate(utl_string *str);
void utl_string_concatenate(utl_string *str1, const utl_string *str2);
void utl_string_trim_left(utl_string *str);
void utl_string_trim_right(utl_string *str);
void utl_string_trim(utl_string *str);
void utl_string_replace_all(utl_string *str, const char *old_str, const char *new_str);
void utl_string_pad_left(utl_string *str, size_t total_length, char pad_char);
void utl_string_pad_right(utl_string *str, size_t total_length, char pad_char);
void utl_string_trim_characters(utl_string *str, const char *chars);
void utl_string_shuffle(utl_string *str);
void utl_string_to_title(utl_string *str);
void utl_string_to_capitalize(utl_string *str);
void utl_string_to_casefold(utl_string *str);
void utl_string_format(utl_string *str, const char *format, ...);
void utl_string_remove(utl_string *str, const char *substr);
void utl_string_remove_range(utl_string *str, size_t start_pos, size_t end_pos);
void utl_string_swap_case(utl_string *str);

