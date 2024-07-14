#pragma once

#include <stddef.h>
#include <stdbool.h>

extern const char *STRING_ASCII_LETTERS;
extern const char *STRING_ASCII_LOWERCASE;
extern const char *STRING_ASCII_UPPERCASE;
extern const char *STRING_DIGITS;
extern const char *STRING_HEXDIGITS;
extern const char *STRING_WHITESPACE;
extern const char *STRING_PUNCTUATION;

typedef struct string string;
typedef struct memory_pool_string {
  void *pool;
  size_t poolSize;
  size_t used;
} memory_pool_string;

struct string {
  char *dataStr;
  size_t size;
  size_t capacitySize;
  memory_pool_string *pool;
};

char string_at(string *str, size_t index);
float string_to_float(string *str);
double string_to_double(string *str);
wchar_t *string_to_unicode(const char *str);

string *string_create(const char *initialStr);
string *string_create_with_pool(size_t size);
string *string_substr(string *str, size_t pos, size_t len);
string **string_split(string *str, const char *delimiter, int *count);
string *string_join(string **strings, int count, const char *delimiter);
string *string_from_int(int value);
string *string_from_float(float value);
string *string_from_double(double value);
string **string_tokenize(string *str, const char *delimiters, int *count);
string *string_from_unicode(const wchar_t *wstr);
string **string_create_from_initializer(size_t count, ...);
string *string_to_hex(string *str);
string *string_from_hex(string *hexStr);
string *string_base64_encode(const string *input);
string *string_base64_decode(const string *input);
string *string_repeat(const string *str, size_t count);
string *string_join_variadic(size_t count, ...);

bool string_is_equal(string *str1, string *str2);
bool string_is_less(string *str1, string *str2);
bool string_is_greater(string *str1, string *str2);
bool string_is_less_or_equal(string *str1, string *str2);
bool string_is_greater_or_equal(string *str1, string *str2);
bool string_is_not_equal(string *str1, string *str2);
bool string_is_alpha(string *str);
bool string_is_digit(string *str);
bool string_is_lower(string *str);
bool string_is_upper(string *str);
bool string_empty(string *str);
bool string_contains(string *str, const char *substr);
bool string_set_pool_size(string *str, size_t newSize);
bool string_starts_with(const string *str, const char *substr);
bool string_ends_with(const string *str, const char *substr);
bool string_to_bool_from_cstr(const char *boolstr);

int string_compare(const string *str1, const string *str2);
int string_find(string *str, const char *buffer, size_t pos);
int string_rfind(string *str, const char *buffer, size_t pos);
int string_find_first_of(string *str, const char *buffer, size_t pos);
int string_find_last_of(string *str, const char *buffer, size_t pos);
int string_find_first_not_of(string *str, const char *buffer, size_t pos);
int string_find_last_not_of(string *str, const char *buffer, size_t pos);
int string_compare_ignore_case(string *str1, string *str2);
int string_to_int(string *str);
int string_strcmp(const char *str1, const char *str2);

void string_reverse(string *str);
void string_resize(string *str, size_t newSize);
void string_shrink_to_fit(string *str);
void string_append(string *str, const char *strItem);
void string_push_back(string *str, const char chItem); 
void string_assign(string *str, const char *newStr);
void string_insert(string *str, size_t pos, const char *strItem);
void string_erase(string *str, size_t pos, size_t len);
void string_replace(string *str1, const char *oldStr, const char *newStr);
void string_swap(string *str1, string *str2);
void string_pop_back(string *str);
void string_deallocate(string *str);
void string_clear(string *str);

char* string_to_upper(string *str);
char* string_to_lower(string *str);
char* string_begin(string *str);
char* string_end(string *str);
char* string_rbegin(string *str);
char* string_rend(string *str);
char* string_back(string *str);
char* string_front(string *str);
char* string_strdup(const char *s);
char* string_from_int_cstr(int value);

size_t string_length_cstr(const char *str);
size_t string_length_utf8(const char *str);
size_t string_length(string *str);
size_t string_capacity(string *str);
size_t string_max_size(string *str);
size_t string_copy(string *str, char *buffer, size_t pos, size_t len);
size_t string_count(string *str, const char *substr);
size_t string_utf8_char_len(char c);

const char* string_data(string *str);
const char* string_c_str(const string *str);
const char* string_cbegin(string *str);
const char* string_cend(string *str);
const char* string_crbegin(string *str);
const char* string_crend(string *str);

void string_deallocate(string *str);
void string_concatenate(string *str1, const string *str2);
void string_trim_left(string *str);
void string_trim_right(string *str);
void string_trim(string *str);
void string_replace_all(string *str, const char *oldStr, const char *newStr);
void string_pad_left(string *str, size_t totalLength, char padChar);
void string_pad_right(string *str, size_t totalLength, char padChar);
void string_trim_characters(string *str, const char *chars);
void string_shuffle(string *str);
void string_to_title(string *str);
void string_to_capitalize(string *str);
void string_to_casefold(string *str);
void string_remove(string *str, const char *substr);
void string_remove_range(string *str, size_t startPos, size_t endPos);
void string_swap_case(string *str);

