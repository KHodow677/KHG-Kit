cmake_minimum_required(VERSION 3.16)
project(khg_utl)

add_library(khg_utl)

file(GLOB_RECURSE sources "src/*.c")
target_sources(khg_utl PRIVATE "${sources}")
target_include_directories(khg_utl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
