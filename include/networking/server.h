#pragma once

#include "khg_dbm/dbm.h"

void server_run(void);

MiniDbState create_or_open_db(const char *asset_name, MiniDb *db, size_t size);

