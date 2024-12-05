#pragma once

#define UTL_REGEX_MAX_OBJECTS 30
#define UTL_REGEX_MAX_CHAR_CLASS_LEN 40

#define FOREACH_REGEX_TYPE(REGEX)\
  REGEX(UTL_REGEX_UNUSED)\
  REGEX(UTL_REGEX_DOT)\
  REGEX(UTL_REGEX_BEGIN)\
  REGEX(UTL_REGEX_END)\
  REGEX(UTL_REGEX_QUESTIONMARK)\
  REGEX(UTL_REGEX_STAR)\
  REGEX(UTL_REGEX_PLUS)\
  REGEX(UTL_REGEX_CHAR)\
  REGEX(UTL_REGEX_CHAR_CLASS)\
  REGEX(UTL_REGEX_INV_CHAR_CLASS)\
  REGEX(UTL_REGEX_DIGIT)\
  REGEX(UTL_REGEX_NOT_DIGIT)\
  REGEX(UTL_REGEX_ALPHA)\
  REGEX(UTL_REGEX_NOT_ALPHA)\
  REGEX(UTL_REGEX_WHITESPACE)\
  REGEX(UTL_REGEX_NOT_WHITESPACE)\

#define GENERATE_REGEX_ENUM(ENUM) ENUM,
#define GENERATE_REGEX_STRING(STRING) #STRING,

typedef enum {
  FOREACH_REGEX_TYPE(GENERATE_REGEX_ENUM)
} regex_id;

#define REGEX_STRINGS (char *[]){ FOREACH_REGEX_TYPE(GENERATE_REGEX_STRING) }
#define REGEX_STRINGS_SIZE sizeof(REGEX_STRINGS) / sizeof(REGEX_STRINGS[0])

typedef struct utl_regex {
  unsigned char type;
  union {
    unsigned char ch;
    unsigned char *ccl;
  } u;
} utl_regex;

utl_regex *utl_regex_compile(const char *pattern);
int utl_regex_match_p(utl_regex *pattern, const char *text, int *match_length);
int utl_regex_match(const char *pattern, const char *text, int *match_length);
void utl_regex_print(utl_regex *pattern);

int utl_regex_match_pattern(utl_regex *pattern, const char *text, int *match_length);
int utl_match_char_class(char c, const char *str);
int utl_regex_match_star(utl_regex p, utl_regex *pattern, const char *text, int *match_length);
int utl_regex_match_plus(utl_regex p, utl_regex *pattern, const char *text, int *match_length);
int utl_regex_match_one(utl_regex p, char c);
int utl_regex_match_digit(char c);
int utl_regex_match_alpha(char c);
int utl_regex_match_whitespace(char c);
int utl_regex_match_meta_char(char c, const char *str);
int utl_regex_match_range(char c, const char *str);
int utl_regex_match_dot(char c);
int utl_regex_match_alphanum(char c);
int utl_regex_match_question(utl_regex p, utl_regex *pattern, const char *text, int *match_length);
int utl_regex_is_meta_char(char c);
