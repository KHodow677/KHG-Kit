#pragma once

#include "khg_dbm/dbm.h"
#include <stdint.h>

void minidb_index_init(dbm_index *index);
dbm_db_state minidb_index_open(dbm_index *index, const char *path, int64_t row_count, int64_t freelist_count);
void minidb_index_close(dbm_index *index);

void minidb_index_write(const dbm_index *index);
