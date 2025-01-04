#include "khg_khs/io.h"
#include "khg_khs/khs.h"

#include <stdio.h>
#include <string.h>

char *khs_load_file(const char *path, unsigned int *len) {
  FILE *f = fopen(path, "r");
  if (f == NULL) {
    return NULL;
  }
  size_t buff_cap = 128, buff_size = 0;
  char *buff = khs_alloc(buff_cap);
  if (buff == NULL) {
    goto ERR;
  }
  size_t n_read;
  while ((n_read = fread(buff + buff_size, sizeof(char), buff_cap - buff_size, f)) != 0 ) {
    buff_size += n_read;
    if (buff_size == buff_cap) {
      buff_cap *= 2;
      void *tmp = khs_realloc(buff, buff_cap);
      if (tmp == NULL) {
        goto ERR;
      }
      buff = tmp;
    }
  }
  fclose(f);
  *len = buff_size;
  return buff;
ERR:
  khs_free(buff);
  fclose(f);
  return NULL;
}

void khs_print_file_val(FILE *f, khs_val val) {
  switch(val.type) {
    case KHS_TYPE_NULL:
      fputs("Null", f);
      break;
    case KHS_TYPE_NUMBER: {
      khs_float num = khs_as_num(val);
      if (khs_is_integer(val) && num <= LLONG_MAX && num >= LLONG_MIN) {
        fprintf(f, "%lli", (long long int) num);
      }
      else {
        fprintf(f, "%f", num);
      } } 
      break;
    case KHS_TYPE_BOOL:
      fputs(khs_as_bool(val) ? "True" : "False", f);
      break;
    case KHS_TYPE_ERR:
      fputs("Error: ", f);
    case KHS_TYPE_STR: {
      const char *str = khs_get_raw_str(&val);
      khs_size len = KHS_LENOF(val);
      fwrite(str, sizeof(char), len, f); } 
      break;
    case KHS_TYPE_TABLE: {
      putc('{', f);
      bool first = true;
      for (khs_val_pair *i = khs_iter_table(val, NULL); i != NULL; i = khs_iter_table(val, i)) {
        if (!first) {
          putc(' ', f);
        }
        putc('(', f);
        khs_print_val(f, i->key);
        putc(' ', f);
        khs_print_val(f, i->val);
        putc(')', f);
        first = false;
      }
      putc('}', f); } 
      break;
    case KHS_TYPE_ARRAY: {
      putc('(', f);
      const khs_val *items = khs_get_raw_array(val);
      for (khs_size i = 0; i < KHS_LENOF(val); i++) {
        khs_print_val(f, items[i]);
        if (i != KHS_LENOF(val) - 1) {
          putc(' ', f);
        }
      }
      putc(')', f); } 
      break;
    case KHS_TYPE_FN:
    case KHS_TYPE_EXT_FN:
      fputs("Function", f);
      break;
    case KHS_TYPE_TAG:
      fprintf(f, "Tag (%llu)", (unsigned long long) khs_as_tag(val));
      break;
    case KHS_TYPE_OBJECT: {
      khs_object_class *c = khs_object_class_type(val);
      fputs("Object (", f);
      fwrite(c->name, sizeof(char), c->name_len, f);
      putc(')', f); } 
      break;
    default:
      fputs("Unkown", f);
  }
}

