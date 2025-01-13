cmake_minimum_required(VERSION 3.16)
project(openssl)

add_library(openssl)
target_sources(openssl PRIVATE "src/openssl.c")
target_include_directories(openssl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

if (WIN32)
  target_link_libraries(openssl PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libssl.dll.a" "${CMAKE_CURRENT_SOURCE_DIR}/lib/libcrypto.dll.a")
else()
  target_link_libraries(openssl PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libssl.so" "${CMAKE_CURRENT_SOURCE_DIR}/lib/libcrypto.so")
endif()

