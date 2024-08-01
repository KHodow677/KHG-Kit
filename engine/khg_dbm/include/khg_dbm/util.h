#pragma once

#include "khg_dbm/rownodes.h"
#include <stdio.h>

void hexDump (char *desc, void *addr, int len);
size_t get_file_size(FILE *fp);
size_t read_buffer(FILE *fp, char **buf, size_t size);
size_t write_buffer(FILE *fp, char *buf, size_t size);
void db_read(rownode **rowset2);

size_t pack_string(char *buf, char *string);
off_t unpack_string(char *buf, char **string);
int streq(char *str1, char *str2);
size_t mstrcpy(char **dest, char **src);
int prefix(const char *pre, const char *str);
