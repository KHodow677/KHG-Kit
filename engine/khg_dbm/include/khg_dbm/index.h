#pragma once

#include "khg_dbm/dbm.h"
#include <stdint.h>

void minidb_index_init(MiniDbIndex *index);
MiniDbState minidb_index_open(MiniDbIndex *index, const char *path, int64_t row_count, int64_t freelist_count);
void minidb_index_close(MiniDbIndex *index);

void minidb_index_write(const MiniDbIndex *index);
