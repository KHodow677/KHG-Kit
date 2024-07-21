#pragma once

#include <stdbool.h>
#include "libpq-fe.h"

typedef struct {
  char *database;
  char *user;
  char *password;
  char *host;
  char *port;
  PGconn *connection;
} postgres;


typedef struct {
  PGresult *result;
} postgres_result;

postgres* postgres_create();

postgres_result *postgres_query(postgres *pg, const char *query);
postgres_result *postgres_list_tables(postgres *pg);
postgres_result *postgres_get_table_schema(postgres *pg, const char *tableName);
postgres_result *postgres_get_table_columns(postgres *pg, const char *tableName);
postgres_result *postgres_get_table_primary_keys(postgres *pg, const char *tableName); 
postgres_result *postgres_get_table_foreign_keys(postgres *pg, const char *tableName); 
postgres_result *postgres_get_table_indexes(postgres *pg, const char *tableName); 
postgres_result *postgres_get_table_size(postgres *pg, const char *tableName);
postgres_result *postgres_get_column_details(postgres *pg, const char *tableName);
postgres_result *postgres_get_table_constraints(postgres *pg, const char *tableName);
postgres_result *postgres_get_result(postgres *pg);

bool postgres_connect(postgres *pg);
bool postgres_execute_non_query(postgres *pg, const char *command);
bool postgres_begin_transaction(postgres *pg);
bool postgres_commit_transaction(postgres *pg);
bool postgres_rollback_transaction(postgres *pg);
bool postgres_table_exists(postgres *pg, const char *tableName);
bool postgres_execute_prepared(postgres *pg, const char *stmtName, const char *query, int nParams, const char *const *paramValues);
bool postgres_is_null(const postgres_result *pgRes, int row, int col);
bool postgres_is_busy(postgres *pg);
bool postgres_is_non_blocking(const postgres *pg);

void postgres_init(postgres *pg, const char *database, const char *user, const char *password, const char *host, const char *port);
void postgres_disconnect(postgres *pg);
void postgres_clear_result(postgres_result *pgResult);
void postgres_deallocate(postgres *pg);
void postgres_print_result(postgres_result *pgRes);
void postgres_reset(postgres *pg);
void postgres_trace(postgres *pg, FILE *stream);
void postgres_un_trace(postgres *pg);

const char *postgres_get_last_error(postgres *pg);
const char *postgres_get_value(postgres_result *pgRes, int row, int col);

char *postgres_db_value(const postgres *pg);
char *postgres_user_value(const postgres *pg);
char *postgres_password_value(const postgres *pg);
char *postgres_host_value(const postgres *pg);
char *postgres_port_value(const postgres *pg);
char *postgres_object_id_status(const postgres_result *pgRes);
char *postgres_command_status(postgres_result *pgRes);

int postgres_get_affected_rows(postgres *pg, postgres_result *pgRes);
int postgres_get_table_row_count(postgres *pg, const char *tableName);
int postgres_get_table_index_count(postgres *pg, const char *tableName);
int postgres_num_tuples(const postgres_result *pgRes);
int postgres_num_fields(const postgres_result *pgRes);
int postgres_command_tuples(postgres_result *pgRes);
int postgres_backend_pid(postgres *pg);
int postgres_binary_tuples(const postgres_result *pgRes);
int postgres_bytes_size(const postgres_result *pgRes, int colsNumber);
int postgres_reset_start(postgres *pg);
int postgres_protocol_version(const postgres *pg);
int postgres_server_version(const postgres *pg);
int postgres_socket_descriptor(const postgres *pg);
int postgres_flush(postgres *pg);
int postgres_set_non_blocking(postgres *pg, int state);
int postgres_get_line(postgres *pg, char *buffer, int length);
int postgres_get_line_async(postgres *pg, char *buffer, int length);
int postgres_put_line(postgres *pg, const char *buffer);
int postgres_put_bytes(postgres *pg, const char *buffer, int bytes);
int postgres_request_cancle(postgres *pg);
