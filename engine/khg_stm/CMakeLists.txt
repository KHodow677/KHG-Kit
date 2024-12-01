cmake_minimum_required(VERSION 3.16)
project(khg_stm)

add_library(khg_stm)
file(GLOB_RECURSE sources "src/*.c")
target_sources(khg_stm PRIVATE "${sources}")
target_include_directories(khg_stm PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
