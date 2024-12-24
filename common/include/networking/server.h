#pragma once

#include "khg_dbm/dbm.h"

void server_run(void);

dbm_db_state create_or_open_db(const char *asset_name, dbm_db **db, size_t size);
