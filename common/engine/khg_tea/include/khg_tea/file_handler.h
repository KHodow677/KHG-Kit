#include "khg_tea/tea.h"
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_FILENAME_LENGTH 255
#define MAX_INPUT_FILES 50
#define ENCRYPTED_FILE_EXTENSION ".3"

#define FLAG_DELETE_SOURCE_FILE (1 << 0)
#define FLAG_VERBOSE (1 << 1)
#define FLAG_OUTPUT_TO_STDOUT (1 << 2)

enum errors { 
  ERR_NONE, 
  ERR_MALLOC, 
  ERR_FILE_FAILED, 
  ERR_INVALID_ARG 
};

typedef struct {
  int mode;
  int count;
  char key[KEY_SIZE];
  char *files[MAX_INPUT_FILES];
  int flags;
} tea_operation;

int tea_operate(tea_operation *prm);
bool tea_delete(char *filename);
int tea_strip_extension(char *filename, char *extension, char *out);
