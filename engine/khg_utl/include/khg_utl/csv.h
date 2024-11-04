#pragma once

#include "khg_utl/file_reader.h"
#include <stdbool.h>

#define BUFFER_SIZE 1024

typedef struct CsvRow {
  char **cells;
  size_t size;
  size_t capacity;
} CsvRow;

typedef struct CsvFile {
  CsvRow **rows;
  size_t size;
  size_t capacity;
  char delimiter;
} CsvFile;

CsvRow *csv_row_create();
CsvRow *csv_file_get_row(const CsvFile *file, size_t index);
CsvRow *csv_row_read_next(utl_file_reader *reader, char delimiter);
CsvRow *csv_file_get_header(const CsvFile *file);
CsvRow **csv_file_find_rows(const CsvFile *file, const char* searchTerm); 

char *csv_row_get_cell(const CsvRow *row, size_t index);
char *csv_export_to_json(const CsvFile *file); 

CsvFile *csv_file_create(char delimiter);

void csv_file_destroy(CsvFile *file);
void csv_file_read(CsvFile *file, const char *filename);
void csv_file_write(const CsvFile *file, const char *filename);
void csv_file_append_row(CsvFile *file, CsvRow *row);
void csv_file_remove_row(CsvFile *file, size_t index);
void csv_print(const CsvFile *file);
void csv_file_insert_column(CsvFile *file, size_t colIndex, const CsvRow *colData); 
void csv_file_set_header(CsvFile *file, CsvRow *header);
void csv_file_concatenate(CsvFile *file1, const CsvFile *file2); 
void csv_row_destroy(CsvRow *row);
void csv_row_append_cell(CsvRow *row, const char *value);

int csv_row_get_cell_as_int(const CsvRow *row, size_t index); 
int csv_column_sum(const CsvFile *file, size_t columnIndex); 

bool csv_validate_cell_format(const CsvRow *row, size_t index, const char *format); 
