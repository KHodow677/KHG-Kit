#pragma once

#include <stddef.h>

typedef struct Regex Regex;

typedef enum {
  REGEX_SUCCESS,
  REGEX_COMPILE_ERROR,
  REGEX_MATCH_ERROR,
  REGEX_NO_MATCH
} RegexResult;


typedef enum {
  REGEX_DEFAULT = 0,          
  REGEX_CASE_INSENSITIVE = 1, 
  REGEX_MULTILINE = 2,        
  REGEX_DOTALL = 4            
} RegexFlags;


typedef struct {
  const char *start;  
  const char *end;    
  size_t length;      
  int group_count;     
  const char **group_starts;   
  size_t *group_lengths;       
} RegexMatch;

Regex *regex_compile(const char *pattern, RegexFlags flags);

RegexResult regex_match(Regex *regex, const char *string, RegexMatch *match);
RegexResult regex_search(Regex *regex, const char *string, RegexMatch *match);

int regex_find_all(Regex *regex, const char *string, RegexMatch *matches, int max_matches);
void regex_deallocate(Regex *regex);
const char *regex_error_message(RegexResult result);

