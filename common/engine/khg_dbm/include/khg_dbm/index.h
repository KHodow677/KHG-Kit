#pragma once

#include "khg_dbm/dbm.h"
#include <stdint.h>

void dbm_index_init(dbm_index *index);
dbm_db_state dbm_index_open(dbm_index *index, const char *path, int64_t row_count, int64_t freelist_count);
void dbm_index_close(dbm_index *index);

void dbm_index_write(const dbm_index *index);