#include "khg_utl/string.h"
#include "khg_utl/error_func.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <wchar.h>

const char *STRING_ASCII_LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char *STRING_ASCII_LOWERCASE = "abcdefghijklmnopqrstuvwxyz";
const char *STRING_ASCII_UPPERCASE = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char *STRING_DIGITS = "0123456789";
const char *STRING_HEXDIGITS = "0123456789abcdefABCDEF";
const char *STRING_WHITESPACE = " \t\n\r\f\v";
const char *STRING_PUNCTUATION = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";


memory_pool_string *global_pool = NULL;
static memory_pool_string *memory_pool_create(size_t size);
static void *memory_pool_allocate(memory_pool_string *pool, size_t size);
static void memory_pool_destroy(memory_pool_string *pool);
bool memoryPoolCreated = false;

static const char *base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz" "0123456789+/";

static void init_global_memory_pool(size_t size) {
  if (global_pool == NULL) {
    global_pool = memory_pool_create(size);
    memoryPoolCreated = true;
  }   
}

static void destroy_global_memory_pool() {
  if (global_pool != NULL && memoryPoolCreated) {
    memory_pool_destroy(global_pool);
    global_pool = NULL;
  }
}

static memory_pool_string *memory_pool_create(size_t size) {
  memory_pool_string *pool = malloc(sizeof(memory_pool_string));
  if (pool) {
    pool->pool = malloc(size);
    if (!pool->pool) {
      free(pool);
      return NULL;
    }
    pool->poolSize = size;
    pool->used = 0;
  }
  return pool;
}

static void *memory_pool_allocate(memory_pool_string *pool, size_t size) {
  if (pool == NULL) {
    error_func("Memory pool is NULL in memory_pool_allocate", user_defined_data);
    return NULL;
  }
  if (pool->used + size > pool->poolSize) {
    error_func("Memory pool out of memory in memory_pool_allocate", user_defined_data);
    return NULL;
  }
  void *mem = (char *)pool->pool + pool->used;
  pool->used += size;
  return mem;
}

static void memory_pool_destroy(memory_pool_string *pool) {
  if (pool == NULL) {
    error_func("Attempt to destroy a NULL memory pool in memory_pool_destroy", user_defined_data);
    return;
  }
  free(pool->pool);
  free(pool);
}

string *string_create(const char *initialStr) {
  string *str = (string *)malloc(sizeof(string));
  if (!str) {
    error_func("Memory allocation failed for String object in string_create", user_defined_data);
    exit(-1);
  }
  size_t initialSize = initialStr ? strlen(initialStr) : 0;
  str->size = initialSize;
  str->capacitySize = 32 + initialSize;
  size_t initialPoolSize = 10000000;
  str->pool = memory_pool_create(initialPoolSize);
  if (!str->pool) {
    error_func("Memory pool creation failed in string_create", user_defined_data);
    free(str);
    exit(-1);
  }
  str->dataStr = memory_pool_allocate(str->pool, str->capacitySize);
  if (!str->dataStr) {
    error_func("Memory pool allocation failed in string_create", user_defined_data);
    memory_pool_destroy(str->pool);
    free(str);
    exit(-1);
  }
  if (initialStr) {
    strcpy(str->dataStr, initialStr);
  }
  return str;
}

string *string_create_with_pool(size_t size) {
  static int counter = 0;
  if (!counter) {
    init_global_memory_pool(size);
    counter++;
  }
  if (global_pool == NULL) {
    error_func("Failed to initialize global memory pool in string_create_with_pool", user_defined_data);
    exit(-1);
  }
  string *str = (string *)malloc(sizeof(string));
  if (!str) {
    error_func("Memory allocation failed for String object in string_create_with_pool", user_defined_data);
    exit(-1);
  }
  str->size = 0;
  str->capacitySize = 1;
  str->dataStr = NULL;
  str->pool = global_pool;
  return str;
}

string *string_substr(string *str, size_t pos, size_t len) {
  if (str == NULL) {
    error_func("The String object is NULL in string_substr", user_defined_data);
    return NULL;
  }
  if (pos >= str->size) {
    error_func("Position out of bounds in string_substr", user_defined_data);
    return NULL;
  }
  if (pos + len > str->size) { 
    len = str->size - pos;
  }
  string *substr = string_create(NULL);
  if (substr == NULL) { 
    error_func("Memory allocation failed for substring in string_substr", user_defined_data);
    return NULL;
  }
  substr->size = len;
  substr->capacitySize = len + 1;
  substr->dataStr = (char *)malloc(substr->capacitySize * sizeof(char));
  if (substr->dataStr == NULL) {
    error_func("Memory allocation failed for dataStr in substring in string_substr", user_defined_data);
    free(substr);
    return NULL;
  }
  strncpy(substr->dataStr, str->dataStr + pos, len);
  substr->dataStr[len] = '\0';
  return substr;
}

bool string_empty(string *str) {
  return (str == NULL) ? true : (str->size == 0);
}

bool string_contains(string *str, const char *substr) {
  if (str == NULL) {
    error_func("Error: The String object is NULL in string_contains", user_defined_data);
    return false;
  }
  if (substr == NULL) {
    error_func("The substring is NULL in string_contains", user_defined_data);
    return false;
  }
  return strstr(str->dataStr, substr) != NULL;
}

int string_compare(const string *str1, const string *str2) {
  if (str1 == NULL || str2 == NULL) {
    if (str1 == str2) {
      return 0;
    }
    error_func("One or both String objects are NULL in string_compare", user_defined_data);
    return (str1 == NULL) ? -1 : 1;
  }
  return strcmp(str1->dataStr, str2->dataStr);
}

bool string_is_equal(string *str1, string *str2) {
  return string_compare(str1, str2) == 0;
}

bool string_is_less(string *str1, string *str2) {
  return string_compare(str1, str2) < 0;
}

bool string_is_greater(string *str1, string *str2) {
  return string_compare(str1, str2) > 0;
}

bool string_is_less_or_equal(string *str1, string *str2) {
  return string_compare(str1, str2) <= 0;
}

bool string_is_greater_or_equal(string *str1, string *str2) {
  return string_compare(str1, str2) >= 0;
}

bool string_is_not_equal(string *str1, string *str2) {
  return string_compare(str1, str2) != 0;
}

bool string_is_alpha(string *str) {
  if (str != NULL){
    for (size_t index = 0; index < str->size; index++){
      if (!(str->dataStr[index] >= 'a' && str->dataStr[index] <= 'z') && 
        !(str->dataStr[index] >= 'A' && str->dataStr[index] <= 'Z')) {
        return false;
      }
    }
    return true;
  }
  return false;
}

bool string_is_digit(string *str) {
  if (str == NULL) {
    error_func("The String object is NULL in string_is_digit", user_defined_data);
    return false;
  }
  for (size_t index = 0; index < str->size; index++) {
    if (!(str->dataStr[index] >= '0' && str->dataStr[index] <= '9')) {
      return false;
    }
  }       
  return true;
}

bool string_is_upper(string *str) {
  if (str == NULL) {
    error_func("The String object is NULL in string_is_upper", user_defined_data);
    return false;
  }
  for (size_t index = 0; index < str->size; index++) {
    if (str->dataStr[index] >= 'a' && str->dataStr[index] <= 'z') {
      return false;
    }
  }
  return true;
}

bool string_is_lower(string *str) {
  if (str == NULL) {
    error_func("The String object is NULL in string_is_lower", user_defined_data);
    return false;
  }
  for (size_t index = 0; index < str->size; index++) {
    if (str->dataStr[index] >= 'A' && str->dataStr[index] <= 'Z') {
      return false;
    }
  }
  return true;
}

void string_reverse(string *str) {
  if (str != NULL && str->dataStr != NULL) {
    char *reverse = (char *)malloc(sizeof(char) * (str->size + 1));
    if (!reverse) {
      error_func("Error: Memory allocation failed in string_reverse", user_defined_data);
      return;
    }
    for (int index = str->size - 1, j = 0; index >= 0; index--, j++) {
      reverse[j] = str->dataStr[index];
    }
    reverse[str->size] = '\0';
    string_replace(str, string_c_str(str), reverse);
    free(reverse);
  }
  else {
    error_func("The String object or its data is NULL in string_reverse", user_defined_data);
    return;
  }
}

void string_resize(string *str, size_t newSize) {
  if (str == NULL) {
    error_func("The String object is NULL in string_resize", user_defined_data);
    return;
  }
  if (newSize < str->size) {
    str->size = newSize;
    str->dataStr[newSize] = '\0';
  } 
  else if (newSize > str->size) {
    if (newSize >= str->capacitySize) {
      size_t newCapacity = newSize + 1;
      char *newData = memory_pool_allocate(str->pool, newCapacity);
      if (!newData) {
        error_func("Memory allocation failed in string_resize", user_defined_data);
        return;
      }
      memcpy(newData, str->dataStr, str->size);
      str->dataStr = newData;
      str->capacitySize = newCapacity;
    }
    memset(str->dataStr + str->size, '\0', newSize - str->size);
    str->size = newSize;
  }
}

void string_shrink_to_fit(string *str) {
  if (str == NULL) {
    error_func("The String object is NULL in string_shrink_to_fit", user_defined_data);
    return;
  }
  if (str->size + 1 == str->capacitySize) {
    return;
  }
  if (str->dataStr != NULL) {
    size_t newCapacity = str->size + 1;
    char *newData = memory_pool_allocate(str->pool, newCapacity);
    if (newData == NULL) {
      error_func("Memory allocation failed in string_shrink_to_fit", user_defined_data);
      return;
    }
    memcpy(newData, str->dataStr, str->size);
    newData[str->size] = '\0';
    str->dataStr = newData;
    str->capacitySize = newCapacity;
  }
}

void string_append(string *str, const char *strItem) {
  if (str == NULL) {
    error_func("The String object is NULL in string_append", user_defined_data);
    return;
  }
  if (strItem == NULL) {
    error_func("The strItem is NULL in string_append", user_defined_data);
    return;
  }
  size_t strItemLength = strlen(strItem);
  if (strItemLength == 0) { 
    return;
  }
  if (str->size + strItemLength >= str->capacitySize) {
    size_t newCapacity = str->size + strItemLength + 1;
    char *newData = memory_pool_allocate(str->pool, newCapacity);
    if (!newData) {
      error_func("Memory allocation failed in string_append", user_defined_data);
      return;
    }
    memcpy(newData, str->dataStr, str->size);
    str->dataStr = newData;
    str->capacitySize = newCapacity;
  }
  strcpy(str->dataStr + str->size, strItem);
  str->size += strItemLength;
}

void string_push_back(string *str, char chItem) {
  if (str == NULL) {
    error_func("The String object is NULL in string_push_back", user_defined_data);
    return;
  }
  if (str->size + 1 >= str->capacitySize) {
    size_t newCapacity = str->capacitySize * 2;
    char *newData = memory_pool_allocate(str->pool, newCapacity);
    if (!newData) {
      error_func("Error: Memory allocation failed in string_push_back", user_defined_data);
      return;
    }
    if (str->dataStr) { 
      memcpy(newData, str->dataStr, str->size);
    }
    str->dataStr = newData;
    str->capacitySize = newCapacity;
  }
  str->dataStr[str->size] = chItem;
  str->size++;
  str->dataStr[str->size] = '\0';
}

void string_assign(string *str, const char *newStr) {
  if (str == NULL) {
    error_func("The String object is NULL in string_assign", user_defined_data);
    return;
  }
  if (newStr == NULL) {
    error_func("The newStr is NULL in string_assign", user_defined_data);
    return;
  }
  size_t newStrLength = strlen(newStr);
  if (newStrLength + 1 > str->capacitySize) {
    char *newData = memory_pool_allocate(str->pool, newStrLength + 1);
    if (!newData) {
      error_func("Memory allocation failed in string_assign", user_defined_data);
      return;
    }
    str->dataStr = newData;
    str->capacitySize = newStrLength + 1;
  }
  strcpy(str->dataStr, newStr);
  str->size = newStrLength;
}

void string_insert(string *str, size_t pos, const char *strItem) {
  if (str == NULL) {
    error_func("The String object is NULL in string_insert", user_defined_data);
    return;
  }
  if (strItem == NULL) {
    error_func("The strItem is NULL in string_insert", user_defined_data);
    return;
  }
  if (pos > str->size) {
    error_func("Position out of bounds in string_insert", user_defined_data);
    return;
  }
  size_t strItemLength = strlen(strItem);
  size_t newTotalLength = str->size + strItemLength;
  if (newTotalLength + 1 > str->capacitySize) {
    size_t newCapacity = newTotalLength + 1;
    char *newData = memory_pool_allocate(str->pool, newCapacity);
    if (!newData) {
      error_func("Memory allocation failed in string_insert", user_defined_data);
      return;
    }
    memcpy(newData, str->dataStr, pos);
    memcpy(newData + pos + strItemLength, str->dataStr + pos, str->size - pos);
    str->dataStr = newData;
    str->capacitySize = newCapacity;
  } 
  else { 
    memmove(str->dataStr + pos + strItemLength, str->dataStr + pos, str->size - pos);
  }
  memcpy(str->dataStr + pos, strItem, strItemLength);
  str->size = newTotalLength;
}

void string_erase(string *str, size_t pos, size_t len) {
  if (str == NULL) {
    error_func("The String object is NULL in string_erase", user_defined_data);
    return;
  }
  if (pos >= str->size) {
    error_func("Position out of bounds in string_erase", user_defined_data);
    return;
  }
  if (pos + len > str->size) { 
    len = str->size - pos;
  }
  memmove(str->dataStr + pos, str->dataStr + pos + len, str->size - pos - len + 1);
  str->size -= len;
}

void string_replace(string *str1, const char *oldStr, const char *newStr) {
  if (str1 == NULL) {
    error_func("The String object (str1) is NULL in string_replace", user_defined_data);
    return;
  }
  if (oldStr == NULL) {
    error_func("The oldStr is NULL in string_replace", user_defined_data);
    return;
  }
  if (newStr == NULL) {
    error_func("The newStr is NULL in string_replace", user_defined_data);
    return;
  }
  char *position = strstr(str1->dataStr, oldStr);
  if (position == NULL) { 
    error_func("OldStr not found in str1 in string_replace", user_defined_data);
    return;
  }
  size_t oldLen = strlen(oldStr);
  size_t newLen = strlen(newStr);
  size_t tailLen = strlen(position + oldLen);
  size_t newSize = (position - str1->dataStr) + newLen + tailLen;
  if (newSize + 1 > str1->capacitySize) {
    size_t newCapacity = newSize + 1;
    char *newData = memory_pool_allocate(str1->pool, newCapacity);
    if (!newData) {
        return;
    }
    memcpy(newData, str1->dataStr, position - str1->dataStr);
    memcpy(newData + (position - str1->dataStr) + newLen, position + oldLen, tailLen);
    str1->dataStr = newData;
    str1->capacitySize = newCapacity;
  } 
  else {
    memmove(position + newLen, position + oldLen, tailLen);
  }
  memcpy(position, newStr, newLen);
  str1->size = newSize;
}

void string_swap(string *str1, string *str2) {
  if (str1 == NULL || str2 == NULL) {
    error_func("One or both String objects are NULL in string_swap", user_defined_data);
    return;
  }
  string temp = *str1;
  *str1 = *str2;
  *str2 = temp;
}

void string_pop_back(string *str) {
  if (str == NULL) {
    error_func("The String object is NULL in string_pop_back", user_defined_data);
    return;
  }
  if (str->size == 0) {
    error_func("Attempt to pop back from an empty string in string_pop_back", user_defined_data);
    return;
  }
  str->dataStr[str->size - 1] = '\0';
  str->size--;
}

void string_deallocate(string *str) {
  if (str == NULL) {
    error_func("Attempt to deallocate a NULL String object in string_deallocate", user_defined_data);
    return;
  }
  if (str->pool != NULL) {
    memory_pool_destroy(str->pool);
    str->pool = NULL;
  }
  free(str);
  if (memoryPoolCreated) {
    destroy_global_memory_pool();
  }
}

char string_at(string *str, size_t index) {
  if (str == NULL) {
    error_func("The String object is NULL in string_at", user_defined_data);
    return '\0';
  }
  if (index >= str->size) {
    error_func("Index out of range in string_at", user_defined_data);
    return '\0';
  }
  return str->dataStr[index];
}

char *string_back(string *str) {
  if (str == NULL || str->size == 0) { 
    return NULL; 
  }
  return &str->dataStr[str->size - 1];
}

char *string_front(string *str) {
  if (str == NULL || str->size == 0) { 
    return NULL;  
  }
  return &str->dataStr[0];
}

size_t string_length(string *str) {
  if (str == NULL) {
    error_func("The String object is NULL in string_length", user_defined_data);
    return 0;
  }
  return str->size;
}

size_t string_capacity(string *str) {
  if (str == NULL) {
    error_func("The String object is NULL in string_capacity", user_defined_data);
    return 0;
  }
  return str->capacitySize;
}

size_t string_max_size(string *str) {
  if (str == NULL) {
    error_func("The String object is NULL in string_max_size", user_defined_data);
    return 0;
  }
  return (size_t)-1;
}


size_t string_copy(string *str, char *buffer, size_t pos, size_t len) {
  if (str == NULL || str->dataStr == NULL) {
    error_func("The String object or its data is NULL in string_copy", user_defined_data);
    return 0;
  }
  if (buffer == NULL) {
    error_func("The buffer is NULL in string_copy", user_defined_data);
    return 0;
  }
  if (pos >= str->size) {
    error_func("Position out of bounds in string_copy", user_defined_data);
    return 0;
  }
  size_t copyLen = len;
  if (pos + len > str->size || len == 0) { 
    copyLen = str->size - pos;
  }
  strncpy(buffer, str->dataStr + pos, copyLen);
  buffer[copyLen] = '\0';  
  return copyLen;
}

int string_find(string *str, const char *buffer, size_t pos) {
  if (str == NULL || str->dataStr == NULL) {
    error_func("The String object or its data is NULL in string_find", user_defined_data);
    return -1;
  }
  if (buffer == NULL) {
    error_func("The buffer is NULL in string_find", user_defined_data);
    return -1;
  }
  if (pos >= str->size) {
    error_func("Position out of bounds in string_find", user_defined_data);
    return -1;
  }
  const char *found = strstr(str->dataStr + pos, buffer);
  if (found == NULL) { 
    return -1;
  }
  return (int)(found - str->dataStr);
}

int string_rfind(string *str, const char *buffer, size_t pos) {
  if (str == NULL || str->dataStr == NULL) {
    error_func("The String object or its data is NULL in string_rfind", user_defined_data);
    return -1;
  }
  if (buffer == NULL) {
    error_func("The buffer is NULL in string_rfind", user_defined_data);
    return -1;
  }
  size_t bufferLen = strlen(buffer);
  if (bufferLen == 0) {
    error_func("The buffer is empty in string_rfind", user_defined_data);
    return -1;
  }
  if (pos < bufferLen - 1) {
    error_func("Position is too small in string_rfind", user_defined_data);
    return -1;
  }
  pos = (pos < str->size) ? pos : str->size - 1;
  for (int i = (int)pos; i >= 0; --i) {
    if (strncmp(str->dataStr + i, buffer, bufferLen) == 0) { 
      return i;
    }
  }
  return -1;
}

int string_find_first_of(string *str, const char *buffer, size_t pos) {
  if (str == NULL || str->dataStr == NULL) {
    error_func("The String object or its data is NULL in string_find_first_of", user_defined_data);
    return -1;
  }
  if (buffer == NULL) {
    error_func("The buffer is NULL in string_find_first_of", user_defined_data);
    return -1;
  }
  if (pos >= str->size) {
    error_func("Position out of bounds in string_find_first_of", user_defined_data);
    return -1;
  }
  const char *found = strstr(str->dataStr + pos, buffer);
  if (found != NULL) {
    return (int)(found - str->dataStr);
  }
  return -1;
}

int string_find_last_of(string *str, const char *buffer, size_t pos) {
  if (str == NULL || str->dataStr == NULL) {
    error_func("The String object or its data is NULL in string_find_last_of", user_defined_data);
    return -1;
  }
  if (buffer == NULL) {
    error_func("The buffer is NULL in string_find_last_of", user_defined_data);
    return -1;
  }
  if (pos >= str->size) {
    error_func("Position out of bounds in string_find_last_of", user_defined_data);
    return -1;
  }
  int lastFound = -1;
  const char *currentFound = strstr(str->dataStr, buffer);
  while (currentFound != NULL && (size_t)(currentFound - str->dataStr) <= pos) {
      lastFound = (int)(currentFound - str->dataStr);
      currentFound = strstr(currentFound + 1, buffer);
  }
  return lastFound;
}

int string_find_first_not_of(string *str, const char *buffer, size_t pos) {
  if (str == NULL || str->dataStr == NULL) {
    error_func("The String object or its data is NULL in string_find_first_not_of", user_defined_data);
    return -1;
  }
  if (buffer == NULL) {
    error_func("The buffer is NULL in string_find_first_not_of", user_defined_data);
    return -1;
  }
  if (pos >= str->size) {
    error_func("Position out of bounds in string_find_first_not_of", user_defined_data);
    return -1;
  }
  size_t bufferLen = strlen(buffer);
  if (bufferLen == 0) {
    return (int)pos;
  }
  for (size_t i = pos; i <= str->size - bufferLen; ++i) { 
    if (strncmp(str->dataStr + i, buffer, bufferLen) != 0) { 
      return (int)i;
    }
  }
  return -1;
}

int string_find_last_not_of(string *str, const char *buffer, size_t pos) {
  if (str == NULL || str->dataStr == NULL) {
    error_func("The String object or its data is NULL in string_find_last_not_of", user_defined_data);
    return -1;
  }
  if (buffer == NULL) {
    error_func("The buffer is NULL in string_find_last_not_of", user_defined_data);
    return -1;
  }
  size_t bufferLen = strlen(buffer);
  if (bufferLen == 0) {
    error_func("The buffer is empty in string_find_last_not_of", user_defined_data);
    return -1;
  }
  if (pos < bufferLen - 1) {
    error_func("Position is too small in string_find_last_not_of", user_defined_data);
    return -1;
  }
  pos = (pos < str->size - bufferLen) ? pos : str->size - bufferLen;
  for (int i = (int)pos; i >= 0; --i) {
    if (strncmp(str->dataStr + i, buffer, bufferLen) != 0) { 
      return i;
    }
  }
  return -1;
}

const char *string_data(string *str) {
  if (str == NULL || str->dataStr == NULL) {
    error_func("Invalid input or uninitialized String in string_data function", user_defined_data);
    return NULL;
  }
  return str->dataStr;
}

const char *string_c_str(const string *str) {
  if (str == NULL) { 
    error_func("Invalid input in string_c_str function", user_defined_data);
    return "";
  }
  if (str->dataStr == NULL) {
    error_func("Uninitialized String in string_c_str function", user_defined_data);
    return "";
  }
  return str->dataStr;
}

char *string_begin(string *str) {
  if (str == NULL) { 
    error_func("Invalid input in string_begin function", user_defined_data);
    return "";
  }
  if (str->dataStr == NULL) {
    error_func("Uninitialized String in string_begin function", user_defined_data);
    return "";
  }
  return str->dataStr;
}

char *string_end(string *str) {
  if (str == NULL || str->dataStr == NULL) { 
    error_func("Invalid input or uninitialized String in string_end function", user_defined_data);
    return NULL;
  }
  return str->dataStr + str->size;
}

char *string_rbegin(string *str) {
  if (str == NULL || str->dataStr == NULL || str->size == 0) {
    error_func("Invalid input, uninitialized, or empty String in string_rbegin function", user_defined_data);
    return NULL; 
  }
  return str->dataStr + str->size - 1;
}

char *string_rend(string *str) {
  if (str == NULL || str->dataStr == NULL) { 
    error_func("Invalid input or uninitialized String in string_rend function", user_defined_data);
    return NULL;
  }
  return str->dataStr - 1; 
}

const char *string_cbegin(string *str) {
  if (str == NULL || str->dataStr == NULL) { 
    error_func("Invalid input or uninitialized String in string_cbegin function", user_defined_data);
    return NULL;
  }
  return str->dataStr;
}

const char *string_cend(string *str) {
  if (str == NULL || str->dataStr == NULL) { 
    error_func("Invalid input or uninitialized String in string_cend function", user_defined_data);
    return NULL;
  }
  return str->dataStr + str->size;
}

const char *string_crbegin(string *str) {
  if (str == NULL || str->dataStr == NULL || str->size == 0) { 
    error_func("Invalid input, uninitialized, or empty String in string_crbegin function", user_defined_data);
    return NULL;
  }
  return str->dataStr + str->size - 1;
}

const char *string_crend(string *str) {
  if (str == NULL || str->dataStr == NULL) {
    error_func("Invalid input or uninitialized String in string_crend function", user_defined_data);
    return NULL;
  }
  return str->dataStr - 1;
}

void string_clear(string* str) {
  if (str != NULL) {
    str->size = 0;
    if (str->dataStr != NULL) { 
      str->dataStr[0] = '\0';
    }
  }
  error_func("String object is null no need to clear in string_clear", user_defined_data);
}

char *string_to_upper(string *str) {
  if (str != NULL) {
    char *upper = (char *)malloc(sizeof(char) * (str->size + 1));
    if (!upper) {
      error_func("Failed to allocate memory for string_to_upper function", user_defined_data);
      exit(-1);
    }
    for (size_t index = 0; index < str->size; index++) {
      if (isalpha(str->dataStr[index]) && (str->dataStr[index] >= 'a' && str->dataStr[index] <= 'z')) {
        upper[index] = toupper(str->dataStr[index]);
      }
      else { 
        upper[index] = str->dataStr[index];
      }
    }
    upper[str->size] = '\0';
    return upper;
  }
  error_func("Input 'str' is NULL in string_to_upper function", user_defined_data);
  return NULL;
}

char *string_to_lower(string *str) {
  if (str != NULL) {
    char *lower = (char*) malloc(sizeof(char) * (str->size + 1));
    if (!lower) {
      error_func("Failed to allocate memory for string_to_lower function", user_defined_data);
      exit(-1);
    }
    for (size_t index = 0; index < str->size; index++) {
      if (isalpha(str->dataStr[index]) && (str->dataStr[index] >= 'A' && str->dataStr[index] <= 'Z')) {
        lower[index] = tolower(str->dataStr[index]);
      }
      else { 
        lower[index] = str->dataStr[index];
      }
    }
    lower[str->size] = '\0';
    return lower;
  }
  error_func("Input 'str' is NULL in string_to_lower function", user_defined_data);
  return NULL;
}

bool string_set_pool_size(string *str, size_t newSize) {
  if (!str) {
    error_func("Invalid input - 'str' is NULL in string_set_pool_size", user_defined_data);
    return false;
  }
  if (newSize == 0) { 
    error_func("Invalid input - 'newSize' is zero in string_set_pool_size", user_defined_data);
    return false;
  }
  if (str->pool) {
    memory_pool_destroy(str->pool);
    str->pool = NULL;
  }
  str->pool = memory_pool_create(newSize);
  if (!str->pool) { 
    error_func("Failed to create a new memory pool in string_set_pool_size", user_defined_data);
    return false;
  }
  if (str->size > 0 && str->dataStr) {
    char* newData = memory_pool_allocate(str->pool, str->size + 1);
    if (!newData) {
      error_func("Failed to allocate memory for string data in the new pool in string_set_pool_size", user_defined_data);
      memory_pool_destroy(str->pool);
      str->pool = NULL;
      return false;
    }
    memcpy(newData, str->dataStr, str->size);
    newData[str->size] = '\0';
    str->dataStr = newData;
  }
  return true;
}

void string_concatenate(string *str1, const string *str2) {
  if (str1 == NULL) {
    error_func("Null String object 'str1' in string_concatenate", user_defined_data);
    return;
  }
  if (str2 == NULL) {
    error_func("Null String object 'str2' in string_concatenate", user_defined_data);
    return;
  }
  string_append(str1, str2->dataStr);
}

void string_trim_left(string *str) {
  if (str == NULL) {
    error_func("Null String object in string_trim_left", user_defined_data);
    return;
  }
  if (str->size == 0) {
    return;
  }
  size_t i = 0;
  while (i < str->size && isspace((unsigned char)str->dataStr[i])) {
    i++;
  }
  if (i > 0) {
    memmove(str->dataStr, str->dataStr + i, str->size - i);
    str->size -= i;
    str->dataStr[str->size] = '\0';
  }
}

void string_trim_right(string *str) {
  if (str == NULL) {
    error_func("Null String object in string_trim_right", user_defined_data);
    return;
  }
  if (str->size == 0) {
    return;
  }
  size_t i = str->size;
  while (i > 0 && isspace((unsigned char)str->dataStr[i - 1])) {
    i--;
  }
  if (i < str->size) {
    str->dataStr[i] = '\0';
    str->size = i;
  }
}

void string_trim(string *str) {
  if (str == NULL) {
    error_func("Null String object in string_trim", user_defined_data);
    return;
  }
  string_trim_left(str);
  string_trim_right(str);
}

string **string_split(string *str, const char *delimiter, int *count) {
  if (str == NULL) {
    error_func("Null String object in string_split", user_defined_data);
    return NULL;
  }
  if (delimiter == NULL) {
    error_func("Null delimiter in string_split", user_defined_data);
    return NULL;
  }
  size_t num_splits = 0;
  char *temp = string_strdup(str->dataStr);
  if (temp == NULL) {
    error_func("Memory allocation failed in string_split", user_defined_data);
    return NULL;
  }
  char *token = strtok(temp, delimiter);
  while (token != NULL) {
    num_splits++;
    token = strtok(NULL, delimiter);
  }
  free(temp);
  if (num_splits == 0) { 
    return NULL;
  }
  string **splits = malloc(sizeof(string*) * num_splits);
  if (splits == NULL) {
    error_func("Memory allocation failed for splits in string_split", user_defined_data);
    return NULL;
  }
  temp = string_strdup(str->dataStr);
  if (temp == NULL) {
    error_func("Memory allocation failed in string_split", user_defined_data);
    free(splits);
    return NULL;
  }
  token = strtok(temp, delimiter);
  size_t index = 0;
  while (token != NULL && index < num_splits) {
    splits[index] = string_create(token);
    if (splits[index] == NULL) {
      error_func("Failed to create string in string_split", user_defined_data);
      for (size_t i = 0; i < index; i++) {
        string_deallocate(splits[i]);
      }
      free(splits);
      free(temp);
      return NULL;
    }
    index++;
    token = strtok(NULL, delimiter);
  }
  free(temp);
  *count = num_splits;
  return splits;
}

string *string_join(string **strings, int count, const char *delimiter) {
  if (strings == NULL) {
    error_func("Null string array in string_join", user_defined_data);
    return NULL;
  }
  if (count <= 0) {
    error_func("Invalid count in string_join", user_defined_data);
    return NULL;
  }
  if (delimiter == NULL) {
    error_func("Null delimiter in string_join", user_defined_data);
    return NULL;
  }
  string *result = string_create("");
  if (result == NULL) {
    error_func("Memory allocation failed in string_join", user_defined_data);
    return NULL;
  }
  for (int i = 0; i < count; i++) {
    string_append(result, strings[i]->dataStr);
    if (i < count - 1) {
      string_append(result, delimiter);
    }
  }
  return result;
}

void string_replace_all(string *str, const char *oldStr, const char *newStr) {
  if (str == NULL) {
    error_func("Null String object in string_replace_all", user_defined_data);
    return;
  }
  if (oldStr == NULL || newStr == NULL) {
    error_func("Null substring in string_replace_all", user_defined_data);
    return;
  }
  string *temp = string_create("");
  if (temp == NULL) {
    error_func("Memory allocation failed in string_replace_all", user_defined_data);
    return;
  }
  char *start = str->dataStr;
  char *end;
  while ((end = strstr(start, oldStr)) != NULL) {
    *end = '\0';
    string_append(temp, start);
    string_append(temp, newStr);
    start = end + strlen(oldStr);
  }
  string_append(temp, start);
  string_assign(str, temp->dataStr);
  string_deallocate(temp);
}

int string_to_int(string *str) {
  if (str == NULL) {
    error_func("Null String object in string_to_int", user_defined_data);
    return 0;
  }
  if (string_empty(str)) {
    error_func("Empty string in string_to_int", user_defined_data);
    return 0;
  }
  return atoi(str->dataStr);
}

float string_to_float(string *str) {
  if (str == NULL) {
    error_func("Null String object in string_to_float", user_defined_data);
    return 0.0f;
  }
  if (string_empty(str)) {
    error_func("Empty string in string_to_float", user_defined_data);
    return 0.0f;
  }
  return atof(str->dataStr);
}

double string_to_double(string *str) {
  if (str == NULL) {
    error_func("Null String object in string_to_double", user_defined_data);
    return 0.0;
  }
  if (string_empty(str)) {
    error_func("Empty string in string_to_double", user_defined_data);
    return 0.0;
  }
  return strtod(str->dataStr, NULL);
}

void string_pad_left(string *str, size_t totalLength, char padChar) {
  if (str == NULL) {
    error_func("Null String object in string_pad_left", user_defined_data);
    return;
  }
  if (str->size >= totalLength) {
    error_func("Size of String object is bigger or equal that total length in string_pad_left", user_defined_data);
    return;
  }
  size_t padSize = totalLength - str->size;
  size_t newSize = str->size + padSize;
  char *newData = (char *)malloc(newSize + 1);
  if (newData == NULL) {
    error_func("Failed to allocate memory in string_pad_left", user_defined_data);
    return;
  }
  memset(newData, padChar, padSize);
  memcpy(newData + padSize, str->dataStr, str->size);
  newData[newSize] = '\0';
  free(str->dataStr);
  str->dataStr = newData;
  str->size = newSize;
  str->capacitySize = newSize + 1;
}

void string_pad_right(string *str, size_t totalLength, char padChar) {
  if (str == NULL) {
    error_func("Null String object in string_pad_right", user_defined_data);
    return;
  }
  if (str->size >= totalLength) {
    error_func("Size of String object is bigger or equal that total Length in string_pad_right", user_defined_data);
    return;
  }
  size_t padSize = totalLength - str->size;
  size_t newSize = str->size + padSize;
  char *newData = (char *)realloc(str->dataStr, newSize + 1);
  if (newData == NULL) {
    error_func("Failed to allocate memory in string_pad_right", user_defined_data);
    return;
  }
  memset(newData + str->size, padChar, padSize);
  newData[newSize] = '\0';
  str->dataStr = newData;
  str->size = newSize;
  str->capacitySize = newSize + 1;
}

string *string_to_hex(string *str) {
  if (str == NULL) {
    error_func("Null String object in string_to_hex", user_defined_data);
    return NULL;
  }
  if (string_empty(str)) {
    return string_create("");
  }
  string *hexStr = string_create("");
  if (hexStr == NULL) {
    error_func("Memory allocation failed in string_to_hex", user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < str->size; ++i) {
    char buffer[3];
    sprintf(buffer, "%02x", (unsigned char)str->dataStr[i]);
    string_append(hexStr, buffer);
  }
  return hexStr;
}

string *string_from_hex(string *hexStr) {
  if (hexStr == NULL) {
    error_func("Null String object in string_from_hex", user_defined_data);
    return NULL;
  }
  if (string_empty(hexStr) || (hexStr->size % 2) != 0) {
    error_func("Invalid hex string in string_from_hex", user_defined_data);
    return NULL;
  }
  string *str = string_create("");
  if (str == NULL) {
    error_func("Memory allocation failed in string_from_hex", user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < hexStr->size; i += 2) {
    char buffer[3] = {hexStr->dataStr[i], hexStr->dataStr[i + 1], '\0'};
    char ch = (char)strtol(buffer, NULL, 16);
    string_push_back(str, ch);
  }
  return str;
}

size_t string_count(string *str, const char *substr) {
  if (str == NULL) {
    error_func("Null String object in string_count", user_defined_data);
    return 0;
  }
  if (substr == NULL) {
    error_func("Null substring in string_count", user_defined_data);
    return 0;
  }
  if (str->dataStr == NULL) {
    error_func("Null data string in String object in string_count", user_defined_data);
    return 0;
  }
  size_t count = 0;
  const char *temp = str->dataStr;
  const char *found;
  while ((found = strstr(temp, substr)) != NULL) {
    count++;
    temp = found + strlen(substr);
  }
  return count;
}

void string_remove(string *str, const char *substr) {
  if (str == NULL) {
    error_func("Null String object in string_remove", user_defined_data);
    return;
  }
  if (substr == NULL) {
    error_func("Null substring in string_remove", user_defined_data);
    return;
  }
  if (str->dataStr == NULL) {
    error_func("Null data string in String object in string_remove", user_defined_data);
    return;
  }
  if (strlen(substr) == 0) {
    error_func("Empty substring in string_remove", user_defined_data);
    return;
  }
  size_t len = strlen(substr);
  char *p = str->dataStr;
  while ((p = strstr(p, substr)) != NULL) { 
    memmove(p, p + len, strlen(p + len) + 1);
  }
}

void string_remove_range(string *str, size_t startPos, size_t endPos) {
  if (str == NULL || str->dataStr == NULL) {
    error_func("Null String object in string_remove_range", user_defined_data);
    return;
  }
  if (startPos >= endPos || endPos > str->size) {
    error_func("Invalid range in string_remove_range", user_defined_data);
    return;
  }
  size_t length = endPos - startPos;
  memmove(str->dataStr + startPos, str->dataStr + endPos, str->size - endPos + 1);
  str->size -= length;
}

string *string_from_int(int value) {
  char buffer[12];
  sprintf(buffer, "%d", value);
  return string_create(buffer);
}

char *string_from_int_cstr(int value) {
  char buffer[12];
  sprintf(buffer, "%d", value);
  char *result = malloc(strlen(buffer) + 1);
  if (result) {
    strcpy(result, buffer);
  }
  return result;
}

string *string_from_float(float value) {
  char buffer[32];
  sprintf(buffer, "%f", value);
  return string_create(buffer);
}

string *string_from_double(double value) {
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%f", value);
  return string_create(buffer);
}

string **string_tokenize(string *str, const char *delimiters, int *count) {
  if (str == NULL || delimiters == NULL) {
    error_func("Invalid input in string_tokenize", user_defined_data);
    return NULL;
  }
  size_t num_tokens = 0;
  char *temp_str = string_strdup(str->dataStr);
  if (temp_str == NULL) {
    error_func("Memory allocation failed in string_tokenize", user_defined_data);
    return NULL;
  }
  char *token = strtok(temp_str, delimiters);
  while (token != NULL) {
    num_tokens++;
    token = strtok(NULL, delimiters);
  }
  free(temp_str);
  string **tokens = malloc(num_tokens * sizeof(string *));
  if (tokens == NULL) {
    error_func("Memory allocation failed for tokens in string_tokenize", user_defined_data);
    return NULL;
  }
  temp_str = string_strdup(str->dataStr);
  if (temp_str == NULL) {
    error_func("Memory allocation failed in string_tokenize", user_defined_data);
    free(tokens);
    return NULL;
  }
  token = strtok(temp_str, delimiters);
  size_t idx = 0;
  while (token != NULL && idx < num_tokens) {
    tokens[idx] = string_create(token);
    if (tokens[idx] == NULL) {
      error_func("String_create failed in string_tokenize", user_defined_data);
      for (size_t i = 0; i < idx; ++i) {
        string_deallocate(tokens[i]);
      }
      free(tokens);
      free(temp_str);
      return NULL;
    }
    idx++;
    token = strtok(NULL, delimiters);
  }
  free(temp_str);
  *count = num_tokens;
  return tokens;
}

int string_compare_ignore_case(string *str1, string *str2) {
  if (str1 == NULL || str2 == NULL) {
    if (str1 == str2) {
      return 0;
    }
    return (str1 == NULL) ? -1 : 1;
  }
  if (str1->dataStr == NULL || str2->dataStr == NULL) {
    if (str1->dataStr == str2->dataStr) {
      return 0;
    }
    return (str1->dataStr == NULL) ? -1 : 1;
  }
  return strcasecmp(str1->dataStr, str2->dataStr);
}

string *string_base64_encode(const string *input) {
  if (input == NULL) {
    error_func("The String object is NULL in string_baes64_encode", user_defined_data);
    return NULL;
  }
  if (input->dataStr == NULL) {
    error_func("The dataStr of String object is NULL in string_base64_encode", user_defined_data);
    return NULL;
  }
  string *encoded = string_create("");
  int val = 0, valb = -6;
  size_t i;
  for (i = 0; i < input->size; i++) {
    unsigned char c = input->dataStr[i];
    val = (val << 8) + c;
    valb += 8;
    while (valb >= 0) {
      string_push_back(encoded, base64_chars[(val >> valb) & 0x3F]);
      valb -= 6;
    }
  }
  if (valb > -6) {
    string_push_back(encoded, base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
  }
  while (encoded->size % 4) { 
    string_push_back(encoded, '=');
  }
  return encoded;
}

string *string_base64_decode(const string *encodedStr) {
  if (encodedStr == NULL) {
    error_func("EncodedStr param is null in string_base64_decode", user_defined_data);
    return NULL;
  }
  if (encodedStr->dataStr == NULL) {
    error_func("DataStr item of encodedStr object is null in string_base64_decode", user_defined_data);
    return NULL;
  }
  char *decodedStr = (char *)malloc(encodedStr->size * 3 / 4 + 1); 
  if (decodedStr == NULL) {
    error_func("Failed to allocate memory for base64 decoding", user_defined_data);
    return NULL;
  }
  int val = 0, valb = -8;
  size_t i = 0;
  size_t j = 0;
  for (i = 0; i < encodedStr->size; i++) {
    char c = encodedStr->dataStr[i];
    if (c == '=') {
      break;
    }
    if (c >= 'A' && c <= 'Z') {
      c -= 'A';
    }
    else if (c >= 'a' && c <= 'z') {
      c = c - 'a' + 26;
    }
    else if (c >= '0' && c <= '9') {
      c = c - '0' + 52;
    }
    else if (c == '+') {
      c = 62;
    }
    else if (c == '/') {
      c = 63;
    }
    else { 
      continue;
    }
    val = (val << 6) | c;
    valb += 6;
    if (valb >= 0) {
      decodedStr[j++] = (char)((val >> valb) & 0xFF);
      valb -= 8;
    }
  }
  decodedStr[j] = '\0';
  string *decodedStringObject = string_create(decodedStr);
  free(decodedStr); 
  return decodedStringObject;
}

void string_format(string *str, const char *format, ...) {
  if (str == NULL) {
    error_func("The String object is NULL in string_format", user_defined_data);
    return;
  }
  if (format == NULL) {
    error_func("The format string is NULL in string_format", user_defined_data);
    return;
  }
  va_list args;
  va_start(args, format);
  int length = vsnprintf(NULL, 0, format, args);
  if (length < 0) {
    error_func("Vsnprintf failed in string_format", user_defined_data);
    va_end(args);
    return;
  }
  char* buffer = (char*)malloc(length + 1);
  if (!buffer) {
    error_func("Failed to allocate memory in string_format", user_defined_data);
    va_end(args);
    return;
  }
  vsnprintf(buffer, length + 1, format, args);
  string_assign(str, buffer);
  free(buffer);
  va_end(args);
}

string *string_repeat(const string *str, size_t count) {
  if (str == NULL) {
    error_func("The String object is NULL in string_repeat", user_defined_data);
    return NULL;
  }
  if (str->dataStr == NULL) {
    error_func("The dataStr of String object is NULL in string_repeat", user_defined_data);
    return NULL;
  }
  size_t newLength = str->size * count;
  char *repeatedStr = (char *)malloc(newLength + 1);
  if (repeatedStr == NULL) {
    error_func("Failed to allocate memory in string_repeat", user_defined_data);
    return NULL;
  }
  char *current = repeatedStr;
  for (size_t i = 0; i < count; ++i) {
    memcpy(current, str->dataStr, str->size);
    current += str->size;
  }
  *current = '\0';
  string *result = string_create(repeatedStr);
  free(repeatedStr);
  return result;
}

string *string_join_variadic(size_t count, ...) {
  va_list args;
  va_start(args, count);
  size_t totalLength = 0;
  for (size_t i = 0; i < count; ++i) {
    string *str = va_arg(args, string*);
    totalLength += str->size;
  }
  va_end(args);
  char *joinedStr = (char *)malloc(totalLength + 1);
  if (joinedStr == NULL) {
    error_func("Failed to allocate memory in string_join_variadic", user_defined_data);
    return NULL;
  }
  char *current = joinedStr;
  va_start(args, count);
  for (size_t i = 0; i < count; ++i) {
    string *str = va_arg(args, string*);
    memcpy(current, str->dataStr, str->size);
    current += str->size;
  }
  *current = '\0';
  va_end(args);
  string *result = string_create(joinedStr);
  free(joinedStr);
  return result;
}

void string_trim_characters(string *str, const char *chars) {
  if (str == NULL) {
    error_func("The String object is NULL in string_trim_characters", user_defined_data);
    return;
  }
  if (str->dataStr == NULL) {
    error_func("The dataStr of String object is NULL in string_trim_characters", user_defined_data);
    return;
  }
  if (chars == NULL) {
    error_func("The chars parameter is NULL in string_trim_characters", user_defined_data);
    return;
  }
  char *start = str->dataStr;
  char *end = str->dataStr + str->size - 1;
  while (start <= end && strchr(chars, *start)) { 
    start++;
  }
  while (end > start && strchr(chars, *end)) { 
    end--;
  }
  size_t newLength = end - start + 1;
  memmove(str->dataStr, start, newLength);
  str->dataStr[newLength] = '\0';
  str->size = newLength;
}

void string_shuffle(string *str){
  if (str == NULL) {
    error_func("The String object is NULL in string_shuffle", user_defined_data);
    return;
  }
  if (str->dataStr == NULL) {
    error_func("The dataStr of String object is NULL in string_shuffle", user_defined_data);
    return;
  }
  srand(time(NULL)); 
  size_t length = strlen(str->dataStr);
  for (size_t i = length - 1; i > 0; i--) {
    size_t j = rand() % (i + 1);
    char temp = str->dataStr[i];
    str->dataStr[i] = str->dataStr[j];
    str->dataStr[j] = temp;
  }
}

void string_to_title(string *str) {
  if (str == NULL) {
    error_func("The String object is NULL in string_to_title", user_defined_data);
    return;
  }
  if (str->dataStr == NULL) {
    error_func("The dataStr of String object is NULL in string_to_title", user_defined_data);
    return;
  }
  bool capitalize = true;
  for (size_t i = 0; i < str->size; i++) {
    if (capitalize && isalpha(str->dataStr[i])) {
      str->dataStr[i] = toupper(str->dataStr[i]);
      capitalize = false;
    } 
    else if (!isalpha(str->dataStr[i])) {
      capitalize = true;
    } 
    else {
      str->dataStr[i] = tolower(str->dataStr[i]);
    }
  }
}

void string_to_capitalize(string *str) {
  if (str == NULL) {
    error_func("The String object is NULL in string_to_capitalize", user_defined_data);
    return;
  }
  if (str->dataStr == NULL) {
    error_func("The dataStr of String object is NULL in string_to_capitalize", user_defined_data);
    return;
  }
  if (str->size == 0) {
    error_func("The size of String object is zero in string_to_capitalize", user_defined_data);
    return;
  }
  str->dataStr[0] = toupper(str->dataStr[0]);
}

void string_to_casefold(string *str) {
  if (str == NULL || str->dataStr == NULL) {
    error_func("Invalid string input in string_to_casefold", user_defined_data);
    return;
  }
  for (size_t i = 0; i < str->size; i++) {
    str->dataStr[i] = tolower(str->dataStr[i]);
  }
}

bool string_starts_with(const string *str, const char *substr) {
  if (str == NULL) {
    error_func("The String object is NULL in string_starts_with", user_defined_data);
    return false;
  }
  if (str->dataStr == NULL) {
    error_func("The dataStr of String object is NULL in string_starts_with", user_defined_data);
    return false;
  }
  if (substr == NULL) {
    error_func("The substring is NULL in string_starts_with", user_defined_data);
    return false;
  }
  size_t substrLen = strlen(substr);
  if (substrLen > str->size) {
    return false;
  }
  return strncmp(str->dataStr, substr, substrLen) == 0;
}

bool string_ends_with(const string *str, const char *substr) {
  if (str == NULL) {
    error_func("The String object is NULL in string_ends_with", user_defined_data);
    return false;
  }
  if (str->dataStr == NULL) {
    error_func("The dataStr of String object is NULL in string_ends_with", user_defined_data);
    return false;
  }
  if (substr == NULL) {
    error_func("The substring is NULL in string_ends_with", user_defined_data);
    return false;
  }
  size_t substrLen = strlen(substr);
  size_t strLen = str->size;
  if (substrLen > strLen) {
    return false;
  }
  return strncmp(str->dataStr + strLen - substrLen, substr, substrLen) == 0;
}

void string_swap_case(string *str) {
  if (str == NULL) {
    error_func("Str is NULL in string_swap_case", user_defined_data);
    return;
  }
  if (str->dataStr == NULL) {
    error_func("Str->dataStr is NULL in string_swap_case", user_defined_data);
    return;
  }
  for (size_t i = 0; i < str->size; i++) {
    if (islower(str->dataStr[i])) {
      str->dataStr[i] = toupper(str->dataStr[i]);
    }
    else if (isupper(str->dataStr[i])) { 
      str->dataStr[i] = tolower(str->dataStr[i]);
    }
  }
}

wchar_t *string_to_unicode(const char *str) {
  if (str == NULL) {
    error_func("Input string is NULL in string_to_unicode", user_defined_data);
    return NULL;
  }
  size_t len = mbstowcs(NULL, str, 0) + 1;
  if (len == (size_t)-1) {
    error_func("Conversion failed in string_to_unicode", user_defined_data);
    return NULL;
  }
  wchar_t *wstr = malloc(len * sizeof(wchar_t));
  if (!wstr) {
    error_func("Memory allocation failed in string_to_unicode", user_defined_data);
    return NULL;
  }
  mbstowcs(wstr, str, len);
  return wstr;
}

string *string_from_unicode(const wchar_t *wstr) {
    if (wstr == NULL) {
      error_func("Input wide string is NULL in string_from_unicode", user_defined_data);
      return NULL;
    }
    size_t len = wcstombs(NULL, wstr, 0);
    if (len == (size_t)-1) {
      error_func("Conversion failed in string_from_unicode", user_defined_data);
      return NULL;
    }
    char *str = malloc(len + 1);
    if (!str) {
      error_func("Memory allocation failed in string_from_unicode", user_defined_data);
      return NULL;
    }
    wcstombs(str, wstr, len + 1);
    string *stringObj = string_create(str);
    free(str);
    return stringObj;
}

string **string_create_from_initializer(size_t count, ...) {
  if (count == 0) {
    error_func("Count is zero in string_create_from_initializer", user_defined_data);
    return NULL;
  }
  va_list args;
  va_start(args, count);
  string **strings = (string **)malloc(sizeof(string *) * (count + 1));
  if (!strings) {
    error_func("Memory allocation failed for strings array in string_create_from_initializer", user_defined_data);
    va_end(args);
    return NULL;
  }
  for (size_t i = 0; i < count; i++) {
    char *str = va_arg(args, char *);
    strings[i] = string_create(str);
    if (!strings[i]) {
      error_func("String_create failed in string_create_from_initializer", user_defined_data);
      for (size_t j = 0; j < i; j++) {
        string_deallocate(strings[j]);
      }
      free(strings);
      va_end(args);
      return NULL;
    }
  }
  strings[count] = NULL;
  va_end(args);
  return strings;
}

char *string_strdup(const char *s) {
  if (s == NULL) {
    error_func("Parameter 's' is NULL in string_strdup", user_defined_data);
    return NULL;
  }
  char *new_str = malloc(strlen(s) + 1);
  if (new_str == NULL) {
    error_func("Memory allocation failed in string_strdup", user_defined_data);
    return NULL;
  }
  strcpy(new_str, s);
  return new_str;
}

size_t string_length_cstr(const char *str) {
  if (!str) {
    error_func("Str is null in string_length_cstr", user_defined_data);
    return 0;
  }
  return (size_t)strlen(str);
}

size_t string_length_utf8(const char *str) {
  if (!str) {
    error_func("Str is null in string_length_cstr", user_defined_data);
    return 0;
  }
  size_t length = 0;
  while (*str) {
    if ((*str & 0xC0) != 0x80) {
      length++;
    }
    str++;
  }
  return length;
}

bool string_to_bool_from_cstr(const char *boolstr) {
  if (!boolstr) {
    error_func("Bool str is NULL and invalid in string_to_bool_cstr", user_defined_data);
    return false;
  }
  if (strcmp(boolstr, "true") == 0) {
    return true;
  }
  else if (strcmp(boolstr, "false") == 0) {
    return false;
  }
  return false;
}

size_t string_utf8_char_len(char c) {
  if ((c & 0x80) == 0) { 
    return 1;
  }
  if ((c & 0xE0) == 0xC0) {
    return 2;
  }
  if ((c & 0xF0) == 0xE0) {
    return 3;
  }
  if ((c & 0xF8) == 0xF0) {
    return 4;
  }
  return 0;
}

int string_strcmp(const char *str1, const char *str2) {
  return strcmp(str1, str2);
}

