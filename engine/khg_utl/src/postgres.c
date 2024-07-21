#include "khg_utl/error_func.h"
#include "khg_utl/postgres.h"
#include "khg_utl/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CON_INFO_SIZE 256 

static void print_line(int nFields, int *widths) {
  for (int i = 0; i < nFields; i++) {
    printf("+");
    for (int j = 0; j < widths[i] + 2; j++) {
        printf("-");
      }
  }
  printf("+\n");
}

postgres *postgres_create() {
  postgres *pg = (postgres *) malloc(sizeof(postgres));
  if (pg) {
    return pg;
  }
  else {
    return NULL;
  }
}

void postgres_init(postgres *pg, const char *database, const char *user, const char *password, const char *host, const char *port) {
  pg->database = string_strdup(database);
  pg->user = string_strdup(user);
  pg->password = string_strdup(password);
  pg->host = string_strdup(host);
  pg->port = string_strdup(port);
  pg->connection = NULL;
}

bool postgres_connect(postgres *pg) {
  char conninfo[CON_INFO_SIZE];
  snprintf(conninfo, sizeof(conninfo), "dbname=%s user=%s password=%s host=%s port=%s", pg->database, pg->user, pg->password, pg->host, pg->port);
  pg->connection = PQconnectdb(conninfo);
  if (PQstatus(pg->connection) != CONNECTION_OK) {
    error_func("Connection to database failed", user_defined_data);
    PQfinish(pg->connection);
    return false;
  }
  return true;
}

bool postgres_execute_non_query(postgres *pg, const char *command) {
  if (pg->connection != NULL) {
    PGresult *res = PQexec(pg->connection, command);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
      error_func("Command execution failed", user_defined_data);
      PQclear(res);
      return false;
    }
    PQclear(res);
    return true;
  }
  else {
    error_func("Connection of postgres is null", user_defined_data);
    return false;
  }
}

void postgres_disconnect(postgres *pg) {
  if (pg->connection != NULL) {
    PQfinish(pg->connection);
    pg->connection = NULL;
  }
}

postgres_result *postgres_query(postgres *pg, const char *query) {
  if (query == NULL) {
    error_func("Query is null", user_defined_data);
    return NULL;
  }
  else if (pg->connection != NULL) {
    PGresult *res = PQexec(pg->connection, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
      error_func("Query failed", user_defined_data);
      PQclear(res);
      return NULL;
    }
    postgres_result *pgRes = (postgres_result *) malloc(sizeof(postgres_result));
    pgRes->result = res;
    return pgRes;
  } 
  else {
    error_func("Connection of postgres is null", user_defined_data);
    return NULL;
  }
}

void postgres_clear_result(postgres_result *pgResult) {
  if (pgResult) {
    PQclear(pgResult->result);
    free(pgResult);
  }
}

void postgres_deallocate(postgres *pg) {
  if (pg) {
    if (pg->database) {
      free(pg->database);
      pg->database = NULL;
    }
    if (pg->user) {
      free(pg->user);
      pg->user = NULL;
    }
    if (pg->password) {
      free(pg->password);
      pg->password = NULL;
    }
    if (pg->host) {
      free(pg->host);
      pg->host = NULL;
    }
    if (pg->port) {
      free(pg->port);
      pg->port = NULL;
    }
    if (pg->connection) {
      PQfinish(pg->connection);
      pg->connection = NULL;
    }
  }
}

bool postgres_begin_transaction(postgres *pg) {
  const char *beginCommand = "BEGIN";
  if (pg->connection != NULL) {
    PGresult *res = PQexec(pg->connection, beginCommand);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
      error_func("Begin transaction failed", user_defined_data);
      PQclear(res);
      return false;
    }
    PQclear(res);
    return true;
  } 
  else {
    error_func("Connection of postgres is null", user_defined_data);
    return false;
  }
}

bool postgres_commit_transaction(postgres *pg) {
  const char *commitCommand = "COMMIT";
  if (pg->connection != NULL) {
    PGresult *res = PQexec(pg->connection, commitCommand);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
      error_func("Commit transaction failed", user_defined_data);
      PQclear(res);
      return false;
    }
    PQclear(res);
    return true;
  } 
  else {
    error_func("Connection of postgres is null", user_defined_data);
    return false;
  }
}

bool postgres_rollback_transaction(postgres *pg) {
  const char *rollbackCommand = "ROLLBACK";
  if (pg->connection != NULL) {
    PGresult *res = PQexec(pg->connection, rollbackCommand);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
      char error_message[512];
      error_func("Rollback transaction failed", user_defined_data);
      PQclear(res);
      return false;
    }
    PQclear(res);
    return true;
  } 
  else {
    error_func("Connection of postgres is null", user_defined_data);
    return false;
  }
}

const char *postgres_get_last_error(postgres *pg) {
  if (pg->connection != NULL) {
    return PQerrorMessage(pg->connection);
  } 
  else {
    return "Connection of postgres is null";
  }
}

int postgres_get_affected_rows(postgres *pg, postgres_result *pgRes) {
  if (pg->connection != NULL && pgRes->result != NULL) {
    return atoi(PQcmdTuples(pgRes->result));
  } 
  else {
    error_func("Connection of postgres or result is null", user_defined_data);
    return -1;
  }
}

void postgres_print_result(postgres_result *pgRes) {
  if (pgRes && pgRes->result) {
    int nFields = PQnfields(pgRes->result);
    int nRows = PQntuples(pgRes->result);
    int widths[nFields];
    for (int i = 0; i < nFields; i++) {
      widths[i] = string_length_cstr(PQfname(pgRes->result, i));
      for (int j = 0; j < nRows; j++) {
        int len = string_length_cstr(PQgetvalue(pgRes->result, j, i));
        if (len > widths[i]) {
          widths[i] = len;
        }
      }
    }
    print_line(nFields, widths);
    for (int i = 0; i < nFields; i++) {
      printf("| %-*s ", widths[i], PQfname(pgRes->result, i));
    }
    printf("|\n");
    print_line(nFields, widths);
    for (int i = 0; i < nRows; i++) {
      for (int j = 0; j < nFields; j++) {
        printf("| %-*s ", widths[j], PQgetvalue(pgRes->result, i, j));
      }
      printf("|\n");
    }
    print_line(nFields, widths);
  } 
  else {
    error_func("PgRes or pgRes->result is null", user_defined_data);
  }
}

int postgres_get_table_row_count(postgres *pg, const char *tableName) {
  if (pg->connection == NULL) {
    error_func("Postgres connection is null", user_defined_data);
    return -1;
  }
  else if (tableName == NULL) {
    error_func("TableName is null", user_defined_data);
    return -1;
  }
  char query[CON_INFO_SIZE];
  snprintf(query, sizeof(query), "SELECT COUNT(*) from %s", tableName);
  postgres_result *pgRes = postgres_query(pg, query);
  if (PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
    int rowCount = atoi(PQgetvalue(pgRes->result, 0, 0));
    postgres_clear_result(pgRes);
    return rowCount;
  }
  else {
    error_func("Quert failed", user_defined_data);
    postgres_clear_result(pgRes);
    return -1;
  }
}

bool postgres_table_exists(postgres *pg, const char *tableName) {
  if (pg->connection == NULL) {
    error_func("Postgres connection is null", user_defined_data);
    return -1;
  }
  else if (tableName == NULL) {
    error_func("TableName is null", user_defined_data);
    return -1;
  }
  char query[CON_INFO_SIZE * 2];
  snprintf(query, sizeof(query), 
    "SELECT EXISTS ("
    "SELECT FROM information_schema.tables "
    "WHERE table_schema = 'public' "
    "AND table_name = '%s'"
    ")", tableName);
  postgres_result *pgRes = postgres_query(pg, query);
  if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
    bool exists = strcmp(PQgetvalue(pgRes->result, 0, 0), "t") == 0;
    postgres_clear_result(pgRes);
    return exists;
  }
  else {
    error_func("Query failed", user_defined_data);
    postgres_clear_result(pgRes);
    return false;
  }
}

postgres_result* postgres_list_tables(postgres* pg) {
  if (pg->connection == NULL) {
    error_func("Postgres connection is null", user_defined_data);
    return NULL;       
  }
  const char* query = 
    "SELECT table_name "
    "FROM information_schema.tables "
    "WHERE table_schema = 'public' "
    "ORDER BY table_name;";
  postgres_result *pgRes = postgres_query(pg, query);
  if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
    return pgRes;
  }
  else {
    error_func("Query failed", user_defined_data);
    if (pgRes) {
      postgres_clear_result(pgRes);
    }
    return NULL;
  }
}

postgres_result *postgres_get_table_schema(postgres *pg, const char *tableName) {
  if (pg->connection == NULL) {
    error_func("Postgres connection is null", user_defined_data);
    return NULL;
  } 
  else if (tableName == NULL) {
    error_func("TableName is null", user_defined_data);
    return NULL;
  }
  char query[CON_INFO_SIZE * 2];
  snprintf(query, sizeof(query), 
    "SELECT column_name, data_type "
    "FROM information_schema.columns "
    "WHERE table_schema = 'public' "
    "AND table_name = '%s';", tableName);
  postgres_result *pgRes = postgres_query(pg, query);
  if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
    return pgRes;
  } 
  else {
    error_func("Query failed", user_defined_data);
    if (pgRes) {
      postgres_clear_result(pgRes);
    }
    return NULL;
  }
}

bool postgres_execute_prepared(postgres *pg, const char *stmtName, const char *query, int nParams, const char *const *paramValues) {
  if (pg->connection == NULL) {
    error_func("Postgres connection is null", user_defined_data);
    return false;
  } 
  else if (stmtName == NULL || query == NULL || paramValues == NULL) {
    error_func("Statement name, query, or parameters are null", user_defined_data);
    return false;
  }
  PGresult *res = PQprepare(pg->connection, stmtName, query, nParams, NULL);
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    error_func("Statement preparation failed", user_defined_data);
    PQclear(res);
    return false;
  }
  PQclear(res);
  res = PQexecPrepared(pg->connection, stmtName, nParams, paramValues, NULL, NULL, 0);
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    error_func("Statement execution failed", user_defined_data);
    PQclear(res);
    return false;
  }
  PQclear(res);
  return true;
}

postgres_result *postgres_get_table_columns(postgres *pg, const char *tableName) {
  if (pg->connection == NULL) {
    error_func("Postgres connection object is null", user_defined_data);
    return NULL;
  }
  else if (tableName == NULL) {
    error_func("TableName is null", user_defined_data);
    return NULL;
  }
  char query[CON_INFO_SIZE * 2];
  snprintf(query, sizeof(query), 
    "SELECT column_name "
    "FROM information_schema.columns "
    "WHERE table_schema = 'public' "
    "AND table_name = '%s';", tableName);
  postgres_result *pgRes = postgres_query(pg, query);
  if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
    return pgRes;
  } 
  else {
    error_func("Query failed", user_defined_data);
    if (pgRes) {
      postgres_clear_result(pgRes);
    }
    return NULL;
  }
}

postgres_result *postgres_get_table_primary_keys(postgres *pg, const char *tableName) {
  if (pg->connection == NULL) {
    error_func("Postgres connection is null", user_defined_data);
    return NULL;
  } 
  else if (tableName == NULL) {
    error_func("TableName is null", user_defined_data);
    return NULL;
  }
  char query[CON_INFO_SIZE * 2];
  snprintf(query, sizeof(query), 
    "SELECT a.attname, format_type(a.atttypid, a.atttypmod) AS data_type "
    "FROM pg_index i "
    "JOIN pg_attribute a ON a.attrelid = i.indrelid "
    "AND a.attnum = ANY(i.indkey) "
    "WHERE i.indrelid = '%s'::regclass "
    "AND i.indisprimary;", tableName);
  postgres_result *pgRes = postgres_query(pg, query);
  if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
    return pgRes;
  } 
  else {
    error_func("Query failed", user_defined_data);
    if (pgRes) {
      postgres_clear_result(pgRes);
    }
    return NULL;
  }
}

postgres_result *postgres_get_table_foreign_keys(postgres *pg, const char *tableName) {
  if (pg->connection == NULL) {
    error_func("Postgres connection is null", user_defined_data);
    return NULL;
  } 
  else if (tableName == NULL) {
    error_func("TableName is null", user_defined_data);
    return NULL;
  }
  char query[CON_INFO_SIZE * 4];
  snprintf(query, sizeof(query),
    "SELECT "
    "    tc.constraint_name, "
    "    kcu.column_name, "
    "    ccu.table_name AS foreign_table_name, "
    "    ccu.column_name AS foreign_column_name "
    "FROM "
    "    information_schema.table_constraints AS tc "
    "    JOIN information_schema.key_column_usage AS kcu "
    "      ON tc.constraint_name = kcu.constraint_name "
    "      AND tc.table_schema = kcu.table_schema "
    "    JOIN information_schema.constraint_column_usage AS ccu "
    "      ON ccu.constraint_name = tc.constraint_name "
    "WHERE tc.constraint_type = 'FOREIGN KEY' AND tc.table_name='%s';", tableName);
  postgres_result *pgRes = postgres_query(pg, query);
  if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
    return pgRes;
  } 
  else {
    error_func("Query failed", user_defined_data);
    if (pgRes) {
      postgres_clear_result(pgRes);
    }
    return NULL;
  }
}

postgres_result *postgres_get_table_indexes(postgres *pg, const char *tableName) {
  if (pg->connection == NULL) {
    error_func("Postgres connection is null", user_defined_data);
    return NULL;
  } 
  else if (tableName == NULL) {
    error_func("TableName is null", user_defined_data);
    return NULL;
  }
  char query[CON_INFO_SIZE * 4];
  snprintf(query, sizeof(query),
    "SELECT indexname, indexdef "
    "FROM pg_indexes "
    "WHERE tablename='%s';", tableName);
  postgres_result *pgRes = postgres_query(pg, query);
  if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
    return pgRes;
  } 
  else {
    error_func("Query failed", user_defined_data);
    if (pgRes) {
      postgres_clear_result(pgRes);
    }
    return NULL;
  }
}

postgres_result *postgres_get_table_size(postgres *pg, const char *tableName) {
  if (pg->connection == NULL) {
    error_func("Postgres connection is null", user_defined_data);
    return NULL;
  } 
  else if (tableName == NULL) {
    error_func("TableName is null", user_defined_data);
    return NULL;
  }
  char query[CON_INFO_SIZE * 4];
  snprintf(query, sizeof(query), "SELECT pg_size_pretty(pg_total_relation_size('%s')) AS size;", tableName);
  postgres_result *pgRes = postgres_query(pg, query);
  if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
    return pgRes;
  } 
  else {
    error_func("Query failed", user_defined_data);
    if (pgRes) {
      postgres_clear_result(pgRes);
    }
    return NULL;
  }
}

int postgres_get_table_index_count(postgres* pg, const char* tableName) {
  if (pg->connection == NULL) {
    error_func("Postgres connection is null", user_defined_data);
    return -1;
  } 
  else if (tableName == NULL) {
    error_func("TableName is null", user_defined_data);
    return -1;
  }
  char query[CON_INFO_SIZE * 4];
  snprintf(query, sizeof(query),
    "SELECT COUNT(*) AS index_count "
    "FROM pg_indexes "
    "WHERE tablename='%s';", tableName);
  postgres_result *pgRes = postgres_query(pg, query);
  if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
    int index_count = atoi(PQgetvalue(pgRes->result, 0, 0));
    postgres_clear_result(pgRes);
    return index_count;
  } 
  else {
    error_func("Query failed", user_defined_data);
    if (pgRes) {
      postgres_clear_result(pgRes);
    }
    return -1;
  }
}

postgres_result *postgres_get_column_details(postgres *pg, const char *tableName) {
  if (pg->connection == NULL) {
    error_func("Postgres connection is null", user_defined_data);
    return NULL;
  } 
  else if (tableName == NULL) {
    error_func("TableName is null", user_defined_data);
    return NULL;
  }
  char query[CON_INFO_SIZE * 4];
  snprintf(query, sizeof(query), 
    "SELECT column_name, data_type, is_nullable, column_default "
    "FROM information_schema.columns "
    "WHERE table_schema = 'public' "
    "AND table_name = '%s';", tableName);
  postgres_result *pgRes = postgres_query(pg, query);
  if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
    return pgRes;
  } 
  else {
    error_func("Query failed", user_defined_data);
    if (pgRes) {
      postgres_clear_result(pgRes);
    }
    return NULL;
  }
}

const char* postgres_get_value(postgres_result* pgRes, int row, int col) {
  if (pgRes == NULL || pgRes->result == NULL) {
    error_func("PostgresResult or its result is null", user_defined_data);
    return NULL;
  }
  if (row < 0 || row >= PQntuples(pgRes->result)) {
    error_func("Row index out of bounds", user_defined_data);
    return NULL;
  }
  if (col < 0 || col >= PQnfields(pgRes->result)) {
    error_func("Column index out of bounds", user_defined_data);
    return NULL;
  }
  return PQgetvalue(pgRes->result, row, col);
}

postgres_result *postgres_get_table_constraints(postgres *pg, const char *tableName) {
  if (pg->connection == NULL) {
    error_func("Postgres connection is null", user_defined_data);
    return NULL;
  } 
  else if (tableName == NULL) {
    error_func("TableName is null", user_defined_data);
    return NULL;
  }
  char query[CON_INFO_SIZE * 4];
  snprintf(query, sizeof(query), 
    "SELECT "
    "tc.constraint_name, "
    "tc.constraint_type, "
    "kcu.column_name, "
    "ccu.table_name AS foreign_table_name, "
    "ccu.column_name AS foreign_column_name, "
    "chk.check_clause "
    "FROM information_schema.table_constraints AS tc "
    "LEFT JOIN information_schema.key_column_usage AS kcu "
    "ON tc.constraint_name = kcu.constraint_name "
    "LEFT JOIN information_schema.constraint_column_usage AS ccu "
    "ON ccu.constraint_name = tc.constraint_name "
    "LEFT JOIN information_schema.check_constraints AS chk "
    "ON tc.constraint_name = chk.constraint_name "
    "WHERE tc.table_name = '%s' AND tc.table_schema = 'public';", tableName);
  postgres_result *pgRes = postgres_query(pg, query);
  if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
    return pgRes;
  } 
  else {
    error_func("Query failed", user_defined_data);
    if (pgRes) {
      postgres_clear_result(pgRes);
    }
    return NULL;
  }
}

int postgres_num_tuples(const postgres_result *pgRes) {
  if (pgRes == NULL || pgRes->result == NULL) {
    error_func("PostgresResult or its result is null", user_defined_data);
    return -1;
  } 
  return PQntuples(pgRes->result);
}

int postgres_num_fields(const postgres_result *pgRes) {
  if (pgRes == NULL || pgRes->result == NULL) {
    error_func("PostgresResult or its result is null", user_defined_data);
    return -1;
  }
  return PQnfields(pgRes->result);
}

int postgres_command_tuples(postgres_result *pgRes) {
  if (pgRes == NULL || pgRes->result == NULL) {
    error_func("PostgresResult or its result is null", user_defined_data);
    return -1;
  }
  return atoi(PQcmdTuples(pgRes->result));
}

int postgres_backend_pid(postgres *pg) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Postgres or its connection is null", user_defined_data);
    return -1;
  }  
  return PQbackendPID(pg->connection);
}

int postgres_binary_tuples(const postgres_result *pgRes) {
  if (pgRes == NULL || pgRes->result == NULL) {
    error_func("PostgresResult or result fields is null", user_defined_data);
    return -1;
  }
  return PQbinaryTuples(pgRes->result);
}

int postgres_bytes_size(const postgres_result *pgRes, int colsNumber) {
  if (pgRes == NULL || pgRes->result == NULL) {
    error_func("PgRes or pgRes->result is null", user_defined_data);
    return -1;
  }
  else if (colsNumber < 0) {
    error_func("ColsNumber start at zero", user_defined_data);
    return -1;
  }
  return PQfsize(pgRes->result, colsNumber);
}

bool postgres_is_null(const postgres_result *pgRes, int row, int col) {
  if (pgRes == NULL || pgRes->result == NULL) {
    error_func("PgRes or pgRes->result is null", user_defined_data);
    return false;
  } 
  else if (row < 0 || row >= postgres_num_tuples(pgRes)) {
    error_func("Row index out of bounds", user_defined_data);
    return false;
  }
  else if (col < 0 || col >= postgres_num_fields(pgRes)) {
    error_func("Column index out of bounds", user_defined_data);
    return false;
  }
  return (bool)PQgetisnull(pgRes->result, row, col);
}

void postgres_reset(postgres *pg) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return;
  }
  else {
    PQreset(pg->connection);
  }
}

int postgres_reset_start(postgres *pg) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return -1;
  }
  else {
    return PQresetStart(pg->connection);
  }
}

char *postgres_db_value(const postgres *pg) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return NULL;
  }
  else {
    return PQdb(pg->connection);
  }
}

char *postgres_user_value(const postgres *pg) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return NULL;
  }
  else {
    return PQuser(pg->connection);
  }
}

char *postgres_password_value(const postgres *pg) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return NULL;
  }
  else {
    return PQpass(pg->connection);
  }
}

char *postgres_host_value(const postgres *pg) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return NULL;
  }
  else {
    return PQhost(pg->connection);
  }
}

char *postgres_port_value(const postgres *pg) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return NULL;
  }
  else {
    return PQport(pg->connection);
  }
}

char *postgres_object_id_status(const postgres_result *pgRes) {
  if (pgRes == NULL || pgRes->result == NULL) {
    error_func("PgRes or pgRes->result is null", user_defined_data);
    return NULL;
  }
  else {
    return PQoidStatus(pgRes->result);
  }
}

char *postgres_command_status(postgres_result *pgRes) {
  if (pgRes == NULL || pgRes->result == NULL) {
    error_func("PgRes or pgRes->result is null", user_defined_data);
    return NULL;
  }
  else {
    return PQcmdStatus(pgRes->result);
  }
}

int postgres_protocol_version(const postgres *pg) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return -1;
  }
  return PQprotocolVersion(pg->connection);
}

int postgres_server_version(const postgres *pg) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return -1;
  }
  return PQserverVersion(pg->connection);
}

int postgres_socket_descriptor(const postgres *pg) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return -1;
  }
  return PQsocket(pg->connection);
}

bool postgres_is_busy(postgres *pg) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return false;
  }
  return (bool)PQisBusy(pg->connection);
}

bool postgres_is_non_blocking(const postgres *pg) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return false;
  }
  return (bool)PQisnonblocking(pg->connection);
}

int postgres_flush(postgres *pg) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return -1;
  }
  return PQflush(pg->connection);
}

int postgres_set_non_blocking(postgres *pg, int state) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return -1;
  }
  return PQsetnonblocking(pg->connection, state);
}

int postgres_get_line(postgres *pg, char *buffer, int length) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return -1;
  }
  else if (buffer == NULL) {
    error_func("Buffer is null", user_defined_data);
    return -1;
  }
  return PQgetline(pg->connection, buffer, length);
}

int postgres_get_line_async(postgres *pg, char *buffer, int length) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return -1;
  }
  else if (buffer == NULL) {
    error_func("Buffer is null", user_defined_data);
    return -1;
  }
  return PQgetlineAsync(pg->connection, buffer, length);
}

int postgres_put_line(postgres *pg, const char *buffer) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return -1;
  }
  else if (buffer == NULL) {
    error_func("Buffer is null", user_defined_data);
    return -1;
  }
  return PQputline(pg->connection, buffer);
}

int postgres_put_bytes(postgres *pg, const char *buffer, int bytes) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return -1;
  }
  else if (buffer == NULL) {
    error_func("Buffer is null", user_defined_data);
    return -1;
  }
  return PQputnbytes(pg->connection, buffer, bytes);
}

void postgres_trace(postgres *pg, FILE *stream) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return;
  }
  else if (stream == NULL) {
    error_func("Stream is null", user_defined_data);
    return;
  }
  PQtrace(pg->connection, stream);
}

void postgres_un_trace(postgres *pg) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return;
  }
  PQuntrace(pg->connection);
}

postgres_result *postgres_get_result(postgres *pg) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return NULL;
  }
  PGresult *res = PQgetResult(pg->connection);
  if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
    error_func("Query failed", user_defined_data);
    PQclear(res);
    return NULL;
  }
  postgres_result *pgRes = (postgres_result *) malloc(sizeof(postgres_result));
  if (!pgRes) {
    error_func("Object creationg failed in PostgresResult", user_defined_data);
    PQclear(res);
    return NULL;
  }
  pgRes->result = res;
  return pgRes;
}

int postgres_request_cancle(postgres *pg) {
  if (pg == NULL || pg->connection == NULL) {
    error_func("Pg connection is null", user_defined_data);
    return -1;
  }
  return PQrequestCancel(pg->connection);
}
