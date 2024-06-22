cmake_minimum_required(VERSION 3.5)
project(khg_utils)

add_library(khg_utils)
target_sources(khg_utils PRIVATE "src/khg_utils.c")
target_include_directories(khg_utils PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
