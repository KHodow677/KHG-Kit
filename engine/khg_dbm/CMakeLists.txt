cmake_minimum_required(VERSION 3.16)
project(khg_dbm)

add_library(khg_dbm)
file(GLOB_RECURSE sources "src/*.c")
target_sources(khg_dbm PRIVATE "${sources}")
target_include_directories(khg_dbm PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
