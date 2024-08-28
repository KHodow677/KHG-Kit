#include "khg_utl/error_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

struct memory_struct {
  char *memory;
  size_t size;
};

static size_t curl_write_cb(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct memory_struct *mem = (struct memory_struct *)userp;
  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(ptr == NULL) {
    utl_error_func("Not enough memory", utl_user_defined_data);
    return 0;
  }
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
  return realsize;
}

int ipa_test() {
  CURL *curl;
  CURLcode res;
  struct memory_struct chunk;
  chunk.memory = malloc(1);
  chunk.size = 0;
  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://icanhazip.com/");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
    res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
      utl_error_func("Curl failed", utl_user_defined_data);
      curl_easy_cleanup(curl);
      free(chunk.memory);
      curl_global_cleanup();
      return 1;
    }
    if (chunk.size > 0) {
      if (chunk.memory[chunk.size - 1] == '\n') {
        chunk.memory[chunk.size - 1] = '\0';
        chunk.size--;
      }
      if (chunk.size > 0 && chunk.memory[chunk.size - 1] == '\r') {
        chunk.memory[chunk.size - 1] = '\0';
        chunk.size--;
      }
    }
    printf("Public IP Address: %s\n", chunk.memory);
    curl_easy_cleanup(curl);
    free(chunk.memory);
  }
  curl_global_cleanup();
  return 0;
}
