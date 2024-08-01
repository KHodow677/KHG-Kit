#include "khg_dbm/database.h"
#include "khg_dbm/parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

int file_exists (char *filename) {
  struct stat buffer;
  return (stat(filename, &buffer) == 0);
}

#if defined(_WIN32) || defined(_WIN64)

ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
  if (lineptr == NULL || n == NULL || stream == NULL) {
    return -1;
  }
  size_t bufsize = *n;
  if (bufsize == 0) {
    bufsize = 128;
    *lineptr = malloc(bufsize);
    if (*lineptr == NULL) {
      return -1;
    }
  }
  char *buffer = *lineptr;
  size_t position = 0;
  while (fgets(buffer + position, bufsize - position, stream)) {
    position = strlen(buffer);
    if (buffer[position - 1] == '\n') {
      return position;
    }
    bufsize *= 2;
    char *new_buffer = realloc(*lineptr, bufsize);
    if (new_buffer == NULL) {
      return -1;
    }
    *lineptr = new_buffer;
    buffer = new_buffer;
  }
  if (position == 0) {
    return -1;
  }
  return position;
}

#endif

int main_test(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Please include database name: ./mini <db_name>\n");
    return 1;
  }
  database *db = create_database(argv[1]);
  char file_name[255];
  snprintf(file_name, 255, "%s.meta",db->name);
  if (file_exists(file_name)) {
    read_files(&db);
  }
  char *query;
  size_t len;
  ssize_t read_in;
  while (1) {
    printf("%s=>", db->name);
    if ((read_in = getline(&query, &len, stdin)) != -1) {
      if (read_in <= 2) {
        write_files(&db);
        return 0;
      }
      interactive(&db, query);
    }
  }
}
