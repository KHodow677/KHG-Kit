cmake_minimum_required(VERSION 3.5)
project(khg_csv)

add_library(khg_csv)
file(GLOB_RECURSE sources "src/*.c")
target_sources(khg_csv PRIVATE "${sources}")
target_include_directories(khg_csv PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
