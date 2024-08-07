#include "unity.h"
#include "khg_tea/test_file_handler.h"
#include "khg_tea/test_tea.h"

void setUp(void) {
}

void tearDown(void) {
}

int main(void) {
  UNITY_BEGIN();
  TEST_TEA_FILE_HANDLER();
  TEST_TEA_TEA();
  return UNITY_END();
}
