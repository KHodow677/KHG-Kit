#pragma once

#include "khg_khs/khs.h"
#include <stdio.h>

char *khs_load_file(const char *path, size_t *len);

void khs_print_file_val(FILE *f, khs_val val);

