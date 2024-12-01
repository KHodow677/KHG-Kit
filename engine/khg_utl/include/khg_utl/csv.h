#pragma once

#include "khg_utl/file_reader.h"
#include <stdbool.h>

#define UTL_CSV_BUFFER_SIZE 1024

typedef struct CsvRow {
  char **cells;
  size_t size;
  size_t capacity;
} utl_csv_row;

typedef struct CsvFile {
  utl_csv_row **rows;
  size_t size;
  size_t capacity;
  char delimiter;
} utl_csv_file;

utl_csv_row *utl_csv_row_create(void);
utl_csv_row *utl_csv_file_get_row(const utl_csv_file *file, size_t index);
utl_csv_row *utl_csv_row_read_next(utl_file_reader *reader, char delimiter);
utl_csv_row *utl_csv_file_get_header(const utl_csv_file *file);
utl_csv_row **utl_csv_file_find_rows(const utl_csv_file *file, const char *search_term); 

char *utl_csv_row_get_cell(const utl_csv_row *row, size_t index);
char *utl_csv_export_to_json(const utl_csv_file *file); 

utl_csv_file *utl_csv_file_create(char delimiter);

void utl_csv_file_destroy(utl_csv_file *file);
void utl_csv_file_read(utl_csv_file *file, const char *filename);
void utl_csv_file_write(const utl_csv_file *file, const char *filename);
void utl_csv_file_append_row(utl_csv_file *file, utl_csv_row *row);
void utl_csv_file_remove_row(utl_csv_file *file, size_t index);
void utl_csv_print(const utl_csv_file *file);
void utl_csv_file_insert_column(utl_csv_file *file, size_t col_index, const utl_csv_row *col_data); 
void utl_csv_file_set_header(utl_csv_file *file, utl_csv_row *header);
void utl_csv_file_concatenate(utl_csv_file *file1, const utl_csv_file *file2); 
void utl_csv_row_destroy(utl_csv_row *row);
void utl_csv_row_append_cell(utl_csv_row *row, const char *value);

int utl_csv_row_get_cell_as_int(const utl_csv_row *row, size_t index); 
int utl_csv_column_sum(const utl_csv_file *file, size_t column_index); 

bool utl_csv_validate_cell_format(const utl_csv_row *row, size_t index, const char *format); 
