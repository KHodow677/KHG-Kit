cmake_minimum_required(VERSION 3.16)
project(khg_kin)

add_library(khg_kin)
file(GLOB_RECURSE sources "src/*.c")
target_sources(khg_kin PRIVATE "${sources}")
target_include_directories(khg_kin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
