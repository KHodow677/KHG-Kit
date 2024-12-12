#pragma once

#include "khg_khs/khs.h"
#include <stdio.h>

char *load_file(const char *path, size_t *len);

void print_i_val(FILE *f, i_val val);

