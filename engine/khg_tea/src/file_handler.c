#include "khg_tea/file_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tea_operate(tea_operation *prm) {
  int status = 0;
  char output_filename[MAX_FILENAME_LENGTH + 2];
  int ed_status, enc_dec_mode = 0, i;
  if (status != ERR_NONE || prm->mode == UNSET || prm->key[0] == 0) {
    exit(ERR_INVALID_ARG);
  }
  for(i = 0; i < prm->count; i++) {
      if (prm->mode == ENCRYPT) {
        output_filename[0] = '\0';
        strcat(output_filename, prm->files[i]);
        strcat(output_filename, ENCRYPTED_FILE_EXTENSION);
      }
      else {
        if (tea_strip_extension(prm->files[i], ENCRYPTED_FILE_EXTENSION, output_filename) == ERR_INVALID_ARG) {
          fprintf(stderr,"Warning: Invalid file extension in %s\n", prm->files[i]);
          continue;
        }
        if (prm->flags & FLAG_OUTPUT_TO_STDOUT) {
          strcpy(output_filename,"(stdout)");
          enc_dec_mode |= TEA_FLAG_OUTPUT_STDOUT;
          printf("-------------- %u: %s --------------\n", i+1, prm->files[i]);
        }
      }
      ed_status = tea_encrypt_decrypt (prm->mode, prm->key, enc_dec_mode, prm->files[i], output_filename);
      if (prm->flags & FLAG_VERBOSE) {
        printf("%s: %s => %s : %s\n", (prm->mode == ENCRYPT) ? "Encryption" : "Decryption", prm->files[i], output_filename, (ed_status) ? "Success" : "Failed");
      }
      if (ed_status == true && prm->flags & FLAG_DELETE_SOURCE_FILE) {
          printf("Deletion: %s : %s\n", prm->files[i], tea_delete(prm->files[i]) ? "Success": "Failed");
      }
  }
  return ERR_NONE;
}

bool tea_delete(char *filename) {
  if (unlink(filename) == -1){
    fprintf(stderr, "File %s could not be deleted.\n", filename);
    perror("unlink");
    return false;
  }
  return true;
}

int tea_strip_extension(char *filename, char *extension, char *out) {
  int flen = strlen(filename), el = flen - strlen(extension);
  *out = '\0';
  if (strcmp (filename + el,extension)) {
    return ERR_INVALID_ARG;
  }
  strcpy(out, filename);
  *(out + el) = '\0';
  return ERR_NONE;
}
