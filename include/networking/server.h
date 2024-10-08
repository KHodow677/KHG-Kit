#pragma once

#include "khg_dbm/database.h"

extern dbm_database *PLAYER_DB;

void server_run(void);

int file_exists (char *filename);
void load_db(const char *asset_name);
void save_db(const char *asset_name);

