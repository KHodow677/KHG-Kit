#include "khg_utl/regex.h"
#include <stdio.h>
#include <ctype.h>

int utl_regex_match(const char *pattern, const char *text, int *match_length) {
  return utl_regex_match_p(utl_regex_compile(pattern), text, match_length);
}

int utl_regex_match_p(utl_regex *pattern, const char *text, int *match_length) {
  *match_length = 0;
  if (pattern != 0) {
    if (pattern[0].type == UTL_REGEX_BEGIN) {
      return (utl_regex_match_pattern(&pattern[1], text, match_length)) ? 0 : -1;
    }
    else {
      int idx = -1;
      do {
        idx += 1;
        if (utl_regex_match_pattern(pattern, text, match_length)) {
          if (text[0] == '\0') {
            return -1;
          }
          return idx;
        }
      }
      while (*text++ != '\0');
    }
  }
  return -1;
}

utl_regex *utl_regex_compile(const char *pattern) {
  static utl_regex re_compiled[UTL_REGEX_MAX_OBJECTS];
  static unsigned char ccl_buf[UTL_REGEX_MAX_CHAR_CLASS_LEN];
  int ccl_bufidx = 1;
  char c;
  int i = 0;
  int j = 0;
  while (pattern[i] != '\0' && (j+1 < UTL_REGEX_MAX_OBJECTS)) {
    c = pattern[i];
    switch (c) {
      case '^': { 
        re_compiled[j].type = UTL_REGEX_BEGIN; } 
        break;
      case '$': { 
        re_compiled[j].type = UTL_REGEX_END; } 
        break;
      case '.': {
        re_compiled[j].type = UTL_REGEX_DOT; }
        break;
      case '*': {
        re_compiled[j].type = UTL_REGEX_STAR; } 
        break;
      case '+': {
        re_compiled[j].type = UTL_REGEX_PLUS; } 
        break;
      case '?': { 
        re_compiled[j].type = UTL_REGEX_QUESTIONMARK; }
        break;
      case '\\': {
        if (pattern[i+1] != '\0') {
          i += 1;
          switch (pattern[i]) {
            case 'd': { 
              re_compiled[j].type = UTL_REGEX_DIGIT; } 
              break;
            case 'D': { 
              re_compiled[j].type = UTL_REGEX_NOT_DIGIT; } 
              break;
            case 'w': { 
              re_compiled[j].type = UTL_REGEX_ALPHA; } 
              break;
            case 'W': { re_compiled[j].type = UTL_REGEX_NOT_ALPHA; } 
              break;
            case 's': {    
              re_compiled[j].type = UTL_REGEX_WHITESPACE; } 
              break;
            case 'S': { 
              re_compiled[j].type = UTL_REGEX_NOT_WHITESPACE; } 
              break;
            default: {
              re_compiled[j].type = UTL_REGEX_CHAR;
              re_compiled[j].u.ch = pattern[i]; } 
              break;
          }
        } } 
        break;
      case '[': {
        int buf_begin = ccl_bufidx;
        if (pattern[i+1] == '^') {
          re_compiled[j].type = UTL_REGEX_INV_CHAR_CLASS;
          i += 1;
          if (pattern[i+1] == 0) {
            return 0;
          }
        }
        else {
          re_compiled[j].type = UTL_REGEX_CHAR_CLASS;
        }
        while ((pattern[++i] != ']') && (pattern[i]   != '\0')) {
          if (pattern[i] == '\\') {
            if (ccl_bufidx >= UTL_REGEX_MAX_CHAR_CLASS_LEN - 1) {
              return 0;
            }
            if (pattern[i+1] == 0) {
              return 0;
            }
            ccl_buf[ccl_bufidx++] = pattern[i++];
          }
          else if (ccl_bufidx >= UTL_REGEX_MAX_CHAR_CLASS_LEN) {
            return 0;
          }
          ccl_buf[ccl_bufidx++] = pattern[i];
        }
        if (ccl_bufidx >= UTL_REGEX_MAX_CHAR_CLASS_LEN) {
          return 0;
        }
        ccl_buf[ccl_bufidx++] = 0;
        re_compiled[j].u.ccl = &ccl_buf[buf_begin]; } 
        break;
      default: {
        re_compiled[j].type = UTL_REGEX_CHAR;
        re_compiled[j].u.ch = c; } 
        break;
    }
    if (pattern[i] == 0) {
      return 0;
    }
    i += 1;
    j += 1;
  }
  re_compiled[j].type = UTL_REGEX_UNUSED;
  return (utl_regex *)re_compiled;
}

void utl_regex_print(utl_regex *pattern) {
  int i;
  int j;
  char c;
  for (i = 0; i < UTL_REGEX_MAX_OBJECTS; ++i) {
    if (pattern[i].type == UTL_REGEX_UNUSED) {
      break;
    }
    printf("type: %s", REGEX_STRINGS[pattern[i].type]);
    if (pattern[i].type == UTL_REGEX_CHAR_CLASS || pattern[i].type == UTL_REGEX_INV_CHAR_CLASS)
    {
      printf(" [");
      for (j = 0; j < UTL_REGEX_MAX_CHAR_CLASS_LEN; ++j)
      {
        c = pattern[i].u.ccl[j];
        if ((c == '\0') || (c == ']'))
        {
          break;
        }
        printf("%c", c);
      }
      printf("]");
    }
    else if (pattern[i].type == UTL_REGEX_CHAR)
    {
      printf(" '%c'", pattern[i].u.ch);
    }
    printf("\n");
  }
}

int utl_regex_match_pattern(utl_regex *pattern, const char *text, int *match_length) {
  int pre = *match_length;
  do {
    if ((pattern[0].type == UTL_REGEX_UNUSED) || (pattern[1].type == UTL_REGEX_QUESTIONMARK)) {
      return utl_regex_match_question(pattern[0], &pattern[2], text, match_length);
    }
    else if (pattern[1].type == UTL_REGEX_STAR) {
      return utl_regex_match_star(pattern[0], &pattern[2], text, match_length);
    }
    else if (pattern[1].type == UTL_REGEX_PLUS) {
      return utl_regex_match_plus(pattern[0], &pattern[2], text, match_length);
    }
    else if ((pattern[0].type == UTL_REGEX_END) && pattern[1].type == UTL_REGEX_UNUSED) {
      return (text[0] == '\0');
    }
    (*match_length)++;
  }
  while ((text[0] != '\0') && utl_regex_match_one(*pattern++, *text++));
  *match_length = pre;
  return 0;
}

int utl_regex_match_char_class(char c, const char *str) {
  do {
    if (utl_regex_match_range(c, str)) {
      return 1;
    }
    else if (str[0] == '\\') {
      str += 1;
      if (utl_regex_match_meta_char(c, str)) {
        return 1;
      }
      else if ((c == str[0]) && !utl_regex_is_meta_char(c)) {
        return 1;
      }
    }
    else if (c == str[0]) {
      if (c == '-') {
        return ((str[-1] == '\0') || (str[1] == '\0'));
      }
      else {
        return 1;
      }
    }
  }
  while (*str++ != '\0');
  return 0;
}

int utl_regex_match_star(utl_regex p, utl_regex *pattern, const char *text, int *match_length) {
  int prelen = *match_length;
  const char *prepoint = text;
  while ((text[0] != '\0') && utl_regex_match_one(p, *text)) {
    text++;
    (*match_length)++;
  }
  while (text >= prepoint) {
    if (utl_regex_match_pattern(pattern, text--, match_length)) {
      return 1;
    }
    (*match_length)--;
  }
  *match_length = prelen;
  return 0;
}

int utl_regex_match_plus(utl_regex p, utl_regex *pattern, const char *text, int *match_length) {
  const char *prepoint = text;
  while ((text[0] != '\0') && utl_regex_match_one(p, *text)) {
    text++;
    (*match_length)++;
  }
  while (text > prepoint) {
    if (utl_regex_match_pattern(pattern, text--, match_length)) {
      return 1;
    }
    (*match_length)--;
  }
  return 0;
}

int utl_regex_match_one(utl_regex p, char c) {
  switch (p.type) {
    case UTL_REGEX_DOT:
      return utl_regex_match_dot(c);
    case UTL_REGEX_CHAR_CLASS:
      return  utl_regex_match_char_class(c, (const char *)p.u.ccl);
    case UTL_REGEX_INV_CHAR_CLASS:
      return !utl_regex_match_char_class(c, (const char *)p.u.ccl);
    case UTL_REGEX_DIGIT:
      return utl_regex_match_digit(c);
    case UTL_REGEX_NOT_DIGIT:
      return !utl_regex_match_digit(c);
    case UTL_REGEX_ALPHA:
      return utl_regex_match_alphanum(c);
    case UTL_REGEX_NOT_ALPHA:
      return !utl_regex_match_alphanum(c);
    case UTL_REGEX_WHITESPACE:
      return utl_regex_match_whitespace(c);
    case UTL_REGEX_NOT_WHITESPACE:
      return !utl_regex_match_whitespace(c);
    default:
      return (p.u.ch == c);
  }
}

int utl_regex_match_digit(char c) {
  return isdigit(c);
}

int utl_regex_match_alpha(char c)
{
  return isalpha(c);
}

int utl_regex_match_whitespace(char c)
{
  return isspace(c);
}

int utl_regex_match_meta_char(char c, const char *str) {
  switch (str[0]) {
    case 'd': 
      return utl_regex_match_digit(c);
    case 'D': 
      return !utl_regex_match_digit(c);
    case 'w': 
      return utl_regex_match_alphanum(c);
    case 'W': 
      return !utl_regex_match_alphanum(c);
    case 's': 
      return utl_regex_match_whitespace(c);
    case 'S': 
      return !utl_regex_match_whitespace(c);
    default:  
      return (c == str[0]);
  }
}

int utl_regex_match_range(char c, const char *str) {
  return ((c != '-') && (str[0] != '\0') && (str[0] != '-') && (str[1] == '-') && (str[2] != '\0') && ((c >= str[0]) && (c <= str[2])));
}

int utl_regex_match_dot(char c) {
  (void)c;
  return 1;
}

int utl_regex_match_alphanum(char c) {
  return ((c == '_') || utl_regex_match_alpha(c) || utl_regex_match_digit(c));
}

int utl_regex_match_question(utl_regex p, utl_regex *pattern, const char *text, int *match_length) {
  if (p.type == UTL_REGEX_UNUSED) {
    return 1;
  }
  if (utl_regex_match_pattern(pattern, text, match_length)) {
    return 1;
  }
  if (*text && utl_regex_match_one(p, *text++)) {
    if (utl_regex_match_pattern(pattern, text, match_length)) {
      (*match_length)++;
      return 1;
    }
  }
  return 0;
}

int utl_regex_is_meta_char(char c) {
  return ((c == 's') || (c == 'S') || (c == 'w') || (c == 'W') || (c == 'd') || (c == 'D'));
}
