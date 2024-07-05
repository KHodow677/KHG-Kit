#include "unity.h"

void setUp(void) {
}

void tearDown(void) {
}

void test_function(void) {
  TEST_ASSERT_EQUAL(0, 0);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_function);
  return UNITY_END();
}
