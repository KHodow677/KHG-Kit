#include "unity.h"
#include "khg_tea/tea.h"
#include <stdint.h>
#include <stdbool.h>

void setUp(void) {
  (void)0;
}

void tearDown(void) {
  (void)0;
}

// Test the TEA coding function
void test_tea_code(void) {
    uint32_t key[4] = {0x01234567, 0x89abcdef, 0xfedcba98, 0x76543210};
    uint32_t plaintext[2] = {0x12345678, 0x9abcdef0};
    uint32_t expected_ciphertext[2] = {0x6a8e48cf, 0xf90f785f};

    // Encrypt the plaintext
    tea_code(plaintext, key);

    // Verify the result
    TEST_ASSERT_EQUAL_UINT32(expected_ciphertext[0], plaintext[0]);
    TEST_ASSERT_EQUAL_UINT32(expected_ciphertext[1], plaintext[1]);
}

// Test the TEA decoding function
void test_tea_decode(void) {
    uint32_t key[4] = {0x01234567, 0x89abcdef, 0xfedcba98, 0x76543210};
    uint32_t ciphertext[2] = {0x9e3779b9, 0xb0a08b76}; // Example ciphertext
    uint32_t expected_plaintext[2] = {0x881cd776, 0xda602dae};

    // Decrypt the ciphertext
    tea_decode(ciphertext, key);

    // Verify the result
    TEST_ASSERT_EQUAL_UINT32(expected_plaintext[0], ciphertext[0]);
    TEST_ASSERT_EQUAL_UINT32(expected_plaintext[1], ciphertext[1]);
}

// Test the tea_encrypt_decrypt function
void test_tea_encrypt(void) {
  char key[KEY_SIZE] = {0};
  char in_file[] = "res/test_assets/khg_tea/test_input.txt";
  char out_file[] = "res/test_assets/khg_tea/test_output.txt";
  bool result = tea_encrypt_decrypt(ENCRYPT, key, !TEA_FLAG_OUTPUT_STDOUT, in_file, out_file);
  TEST_ASSERT_TRUE(result);
}

// Test the tea_encrypt_decrypt function
void test_tea_decrypt(void) {
  char key[KEY_SIZE] = {0};
  char in_file[] = "res/test_assets/khg_tea/test_output.txt";
  char out_file[] = "res/test_assets/khg_tea/test_input.txt";
  bool result = tea_encrypt_decrypt(DECRYPT, key, !TEA_FLAG_OUTPUT_STDOUT, in_file, out_file);
  TEST_ASSERT_TRUE(result);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_tea_code);
  RUN_TEST(test_tea_decode);
  RUN_TEST(test_tea_encrypt);
  RUN_TEST(test_tea_decrypt);
  return UNITY_END();
}
