#include "khg_utl/csv.h"
#include "khg_utl/error_func.h"
#include "khg_utl/string.h"
#include <stdlib.h> 
#include <string.h> 

CsvRow *csv_row_create() {
  CsvRow *row = malloc(sizeof(CsvRow));
  if (!row) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    exit(-1);
  }
  row->cells = NULL;
  row->size = 0;
  row->capacity = 0;
  return row;
}

void csv_row_destroy(CsvRow *row) {
  if (!row) {
    utl_error_func("Row object is null and invalid", utl_user_defined_data);
    return;
  }
  for (size_t i = 0; i < row->size; ++i) {
    free(row->cells[i]);
  }
  free(row->cells); 
  free(row);  
}

void csv_row_append_cell(CsvRow *row, const char *value) {
  if (!row || !value) {
    utl_error_func("Null parameter passed", utl_user_defined_data);
    return;
  }
  if (row->size >= row->capacity) {
    size_t newCapacity = row->capacity == 0 ? 1 : row->capacity * 2;
    char **newCells = realloc(row->cells, newCapacity * sizeof(char *));
    if (!newCells) {
      utl_error_func("Memory allocation failed for new cells", utl_user_defined_data);
      return;
    }
    row->cells = newCells;
    row->capacity = newCapacity;
  }
  row->cells[row->size] = utl_string_strdup(value); 
  if (!row->cells[row->size]) {
    utl_error_func("Memory allocation failed for cell value", utl_user_defined_data);
    return;
  }
  row->size++;
}

char *csv_row_get_cell(const CsvRow *row, size_t index) {
  if (row == NULL || index >= row->size) {
    utl_error_func("Invalid index or null row", utl_user_defined_data);
    return NULL;
  }
  return row->cells[index];
}

CsvFile *csv_file_create(char delimiter) {
  CsvFile *file = malloc(sizeof(CsvFile));
  if (!file) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    exit(-1);
  }
  file->rows = NULL;
  file->size = 0;
  file->capacity = 0;
  file->delimiter = delimiter;
  return file;
}

void csv_file_destroy(CsvFile *file) {
  if (!file) {
    utl_error_func("File object is null and invalid", utl_user_defined_data);
    return;
  }
  for (size_t i = 0; i < file->size; ++i) {
    csv_row_destroy(file->rows[i]);
  }
  free(file->rows);
  free(file);
}

static void parse_csv_line(const char *line, char delimiter, CsvRow *row) {
  if (!line || !row) {
    utl_error_func("Null parameter passed", utl_user_defined_data);
    return;
  }
  bool inQuotes = false;
  size_t start = 0;
  for (size_t i = 0; line[i] != '\0'; ++i) {
    if (line[i] == '"') {
      inQuotes = !inQuotes;
    } 
    else if (line[i] == delimiter && !inQuotes) {
      size_t len = i - start;
      char *cell = (char *)malloc(len + 1);
      if (!cell) {
        utl_error_func("Memory allocation failed for cell", utl_user_defined_data);
        return; 
      }
      strncpy(cell, line + start, len);
      cell[len] = '\0';
      csv_row_append_cell(row, cell);
      free(cell);
      start = i + 1;
    }
  }
  char *cell = utl_string_strdup(line + start);
  if (!cell) {
    utl_error_func("Memory allocation failed for last cell", utl_user_defined_data);
    return; 
  }
  csv_row_append_cell(row, cell);
  free(cell);
}

void csv_file_read(CsvFile *file, const char *filename) {
  if (!file || !filename) {
    utl_error_func("NULL parameter passed", utl_user_defined_data);
    return;
  }
  utl_file_reader *fr = utl_file_reader_open(filename, UTL_READ_TEXT);
  if (!fr) {
    utl_error_func("Unable to open file", utl_user_defined_data);
    return;
  }
  char buffer[BUFFER_SIZE] = {0};
  while (utl_file_reader_read_line(buffer, BUFFER_SIZE, fr)) {
    CsvRow *row = csv_row_create();
    buffer[strcspn(buffer, "\r\n")] = 0; 
    parse_csv_line(buffer, file->delimiter, row);
    csv_file_append_row(file, row);
  }
  utl_file_reader_close(fr);
}

void csv_file_write(const CsvFile *file, const char *filename) {
  if (!file || !filename) {
    utl_error_func("Null parameter passed", utl_user_defined_data);
    return;
  }
  utl_file_writer *fw = utl_file_writer_open(filename, UTL_WRITE_UNICODE); 
  if (!fw) {
    utl_error_func("Unable to open file", utl_user_defined_data);
    return;
  }
  for (size_t i = 0; i < file->size; ++i) {
    CsvRow *row = file->rows[i];
    for (size_t j = 0; j < row->size; ++j) {
      utl_file_writer_write_fmt(fw, "%s", row->cells[j]);
      if (j < row->size - 1) {
        utl_file_writer_write_fmt(fw, "%c", file->delimiter);
      }
    }
    char newline[] = "\n";
    utl_file_writer_write_line(newline, 1, fw);
  }
  utl_file_writer_close(fw);
}

void csv_file_append_row(CsvFile *file, CsvRow *row) {
  if (!file || !row) {
    utl_error_func("Null parameter passed", utl_user_defined_data);
    return;
  }
  if (file->size >= file->capacity) {
    size_t newCapacity = file->capacity == 0 ? 1 : file->capacity * 2;
    CsvRow **newRows = realloc(file->rows, newCapacity * sizeof(CsvRow *));
    if (!newRows) {
      utl_error_func("Unable to allocate memory for new rows", utl_user_defined_data);
      return;
    }
    file->rows = newRows;
    file->capacity = newCapacity;
  }
  file->rows[file->size++] = row;
}

CsvRow *csv_file_get_row(const CsvFile *file, size_t index) {
  if (!file || index >= file->size) {
    utl_error_func("Invalid index or null file", utl_user_defined_data);
    return NULL;
  }
  return file->rows[index];
}

void csv_file_remove_row(CsvFile *file, size_t index) {
  if (!file || index >= file->size) {
    utl_error_func("Invalid index or null file", utl_user_defined_data);
    return;
  }
  csv_row_destroy(file->rows[index]);  
  for (size_t i = index; i < file->size - 1; i++) {
    file->rows[i] = file->rows[i + 1];
  }
  file->size--;
}

void csv_print(const CsvFile *file) {
  if (!file) {
    utl_error_func("Null file pointer", utl_user_defined_data);
    return;
  }
  for (size_t i = 0; i < file->size; ++i) {
    CsvRow *row = file->rows[i];
    for (size_t j = 0; j < row->size; ++j) {
      printf("%s", row->cells[j]);
      if (j < row->size - 1) { 
        printf("%c", file->delimiter);
      }
    }
    printf("\n");
  }
}

CsvRow *csv_row_read_next(utl_file_reader *reader, char delimiter) {
  if (!reader || !utl_file_reader_is_open(reader)) {
    utl_error_func("File reader is null or not open", utl_user_defined_data);
    return NULL;
  }
  char buffer[BUFFER_SIZE];
  if (!utl_file_reader_read_line(buffer, BUFFER_SIZE, reader)) {
    utl_error_func("No more lines to read or error occurred", utl_user_defined_data);
    return NULL;
  }
  buffer[strcspn(buffer, "\r\n")] = 0; 
  CsvRow *row = csv_row_create();
  char *token = strtok(buffer, &delimiter);
  while (token) {
    csv_row_append_cell(row, token);
    token = strtok(NULL, &delimiter);
  }
  return row;
}

void csv_file_insert_column(CsvFile *file, size_t colIndex, const CsvRow *colData) {
  if (!file || !colData || colIndex > colData->size) {
    utl_error_func("Invalid parameters", utl_user_defined_data);
    return;
  }
  for (size_t i = 0; i < file->size; ++i) {
    CsvRow *row = file->rows[i];
    char *cellValue = NULL;
    if (i < colData->size) {
      cellValue = utl_string_strdup(colData->cells[i]);
      if (cellValue == NULL) {
        utl_error_func("Memory allocation failed for cell value in row", utl_user_defined_data);
        continue;
      }
    }
    if (row->size == row->capacity) {
      size_t newCapacity = row->capacity == 0 ? 1 : row->capacity * 2;
      char **newCells = realloc(row->cells, newCapacity * sizeof(char *));
      if (!newCells) {
        utl_error_func("Memory allocation failed for new cells in row", utl_user_defined_data);
        free(cellValue);
        continue;
      }
      row->cells = newCells;
      row->capacity = newCapacity;
    }
    for (size_t j = row->size; j > colIndex; --j) {
      row->cells[j] = row->cells[j - 1];
    }
    row->cells[colIndex] = cellValue;
    row->size++;
  }
}

CsvRow *csv_file_get_header(const CsvFile *file) {
  if (!file || file->size == 0) {
    utl_error_func("File is empty or null", utl_user_defined_data);
    return NULL;
  }
  return file->rows[0]; 
}

void csv_file_set_header(CsvFile *file, CsvRow *header) {
  if (!file || !header) {
    utl_error_func("Null parameters passed", utl_user_defined_data);
    return;
  }
  if (file->size > 0) {
    csv_row_destroy(file->rows[0]);
  } 
  else {
    if (file->size == file->capacity) {
      size_t newCapacity = file->capacity == 0 ? 1 : file->capacity * 2;
      CsvRow **newRows = realloc(file->rows, newCapacity * sizeof(CsvRow *));
      if (!newRows) {
        utl_error_func("Memory allocation failed for new rows", utl_user_defined_data);
        return;
      }
      file->rows = newRows;
      file->capacity = newCapacity;
    }
  }
  file->rows[0] = header;
  file->size = (file->size > 0) ? file->size : 1;
}

int csv_row_get_cell_as_int(const CsvRow *row, size_t index) {
  if (!row || index >= row->size) {
    utl_error_func("Invalid index or null row", utl_user_defined_data);
    return 0;
  }
  int value = atoi(row->cells[index]);
  return value;
}

CsvRow **csv_file_find_rows(const CsvFile *file, const char* searchTerm) {
  if (!file || !searchTerm) {
    utl_error_func("Null parameters passed", utl_user_defined_data);
    return NULL;
  }
  size_t foundCount = 0;
  CsvRow **foundRows = malloc((file->size + 1) * sizeof(CsvRow *)); 
  if (!foundRows) {
    utl_error_func("Memory allocation failed for foundRows", utl_user_defined_data);
    return NULL;
  }
  for (size_t i = 0; i < file->size; ++i) {
    CsvRow* row = file->rows[i];
    for (size_t j = 0; j < row->size; ++j) {
      if (strstr(row->cells[j], searchTerm)) {
        foundRows[foundCount++] = row;
        break;
      }
    }
  }
  foundRows[foundCount] = NULL; 
  CsvRow **resizedFoundRows = realloc(foundRows, (foundCount + 1) * sizeof(CsvRow *));
  if (!resizedFoundRows) {
    return foundRows; 
  }
  return resizedFoundRows; 
}

bool csv_validate_cell_format(const CsvRow *row, size_t index, const char *format) {
  if (!row || !format || index >= row->size) {
    utl_error_func("Invalid parameters", utl_user_defined_data);
    return false;
  }
  char *cell = row->cells[index];
  char buffer[256];
  snprintf(buffer, sizeof(buffer), format, cell);
  bool result = strcmp(cell, buffer) == 0;
  return result;
}

void csv_file_concatenate(CsvFile *file1, const CsvFile *file2) {
  if (!file1 || !file2) {
    utl_error_func("Null parameters provided", utl_user_defined_data);
    return;
  }
  for (size_t i = 0; i < file2->size; ++i) {
    CsvRow *row2 = file2->rows[i];
    CsvRow *newRow = csv_row_create();  
    for (size_t j = 0; j < row2->size; ++j) {
      csv_row_append_cell(newRow, row2->cells[j]);
    }
    csv_file_append_row(file1, newRow);
  }
}

int csv_column_sum(const CsvFile *file, size_t columnIndex) {
  if (!file) {
    utl_error_func("Null file provided", utl_user_defined_data);
    return 0;
  }
  int sum = 0;
  for (size_t i = 0; i < file->size; ++i) {
    CsvRow *row = file->rows[i];
    if (columnIndex < row->size) {
      char *cell = row->cells[columnIndex];
      int cellValue = atoi(cell);
      sum += cellValue;
    } 
    else {
      utl_error_func("Column index out of range in row", utl_user_defined_data);
    }
  }
  return sum;
}

char *csv_export_to_json(const CsvFile *file) {
  if (!file) {
    utl_error_func("Null file passed", utl_user_defined_data);
    return NULL;
  }
  char *json = malloc(BUFFER_SIZE);
  if (!json) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  strcpy(json, "[\n");
  for (size_t i = 0; i < file->size; ++i) {
    CsvRow *row = file->rows[i];
    strcat(json, "  {\n");
    for (size_t j = 0; j < row->size; ++j) {
      char *cell = row->cells[j];
      char line[128];
      sprintf(line, "    \"field%zu\": \"%s\"%s\n", j, cell, j < row->size - 1 ? "," : "");
      strcat(json, line);
    }
    strcat(json, i < file->size - 1 ? "  },\n" : "  }\n");
  }
  strcat(json, "]\n");
  return json;
}
