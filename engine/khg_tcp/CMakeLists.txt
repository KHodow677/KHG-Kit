cmake_minimum_required(VERSION 3.5)
project(khg_tcp)

add_library(khg_tcp)

file(GLOB_RECURSE sources "src/*.c")
target_sources(khg_tcp PRIVATE "${sources}")
target_include_directories(khg_tcp PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
