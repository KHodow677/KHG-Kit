cmake_minimum_required(VERSION 3.16)
project(khg_rgx)

add_library(khg_rgx)

file(GLOB_RECURSE sources "src/*.c")
target_sources(khg_rgx PRIVATE "${sources}")
target_include_directories(khg_rgx PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
