cmake_minimum_required(VERSION 3.16)
project(openssl)

add_library(openssl)
target_sources(openssl PRIVATE "src/openssl.c")
target_include_directories(openssl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

