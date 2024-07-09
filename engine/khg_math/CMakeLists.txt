cmake_minimum_required(VERSION 3.5)
project(khg_math)

add_library(khg_math)
file(GLOB_RECURSE sources "src/*.c")
target_sources(khg_math PRIVATE "${sources}")
target_include_directories(khg_math PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
