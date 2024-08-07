#include "test_file_handler.h"
#include "khg_tea/file_handler.h"
#include "unity.h"
#include <unistd.h>

void TEST_TEA_FILE_HANDLER() {
  RUN_TEST(test_tea_operate_encrypt);
  RUN_TEST(test_tea_operate_decrypt);
  RUN_TEST(test_tea_delete);
  RUN_TEST(test_tea_strip_extension);
}

void test_tea_operate_encrypt() {
  tea_operation op;
  op.mode = ENCRYPT;
  op.count = 1;
  op.key[0] = 'k';
  op.files[0] = "res/test_assets/khg_tea/test_file.txt";
  op.flags = 0;
  int result = tea_operate(&op);
  TEST_ASSERT_EQUAL_INT(ERR_NONE, result);
}

void test_tea_operate_decrypt() {
  tea_operation op;
  op.mode = DECRYPT;
  op.count = 1;
  op.key[0] = 'k';
  op.files[0] = "res/test_assets/khg_tea/test_file.txt.3";
  op.flags = 0;
  int result = tea_operate(&op);
  TEST_ASSERT_EQUAL_INT(ERR_NONE, result);
}

void test_tea_delete() {
  char *fp = "res/test_assets/khg_tea/test_delete.txt";
  TEST_ASSERT_EQUAL_INT(0, access(fp, F_OK));
  bool result = tea_delete(fp);
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL_INT(-1, access(fp, F_OK));
  FILE *file = fopen(fp, "w");
  TEST_ASSERT_NOT_NULL(file);
  fputs("DELETE", file);
  fclose(file);
  TEST_ASSERT_EQUAL_INT(0, access(fp, F_OK));
}

void test_tea_strip_extension() {
  char *ofp = "res/test_assets/khg_tea/test_strip_extension.txt.3";
  char *sfp = "res/test_assets/khg_tea/test_strip_extension.txt";
  char output[MAX_FILENAME_LENGTH + 2];
  int result = tea_strip_extension(ofp, ENCRYPTED_FILE_EXTENSION, output);
  TEST_ASSERT_EQUAL_INT(ERR_NONE, result);
  TEST_ASSERT_EQUAL_STRING(sfp, output);
  FILE *file = fopen(output, "w");
  TEST_ASSERT_NOT_NULL(file);
  fputs("STRIP_EXT", file);
  fclose(file);
  TEST_ASSERT_EQUAL_INT(0, access(output, F_OK));
  unlink(output);
}
