cmake_minimum_required(VERSION 3.16)
project(cglm)

add_library(cglm)
file(GLOB_RECURSE sources "src/*.c")
target_sources(cglm PRIVATE "${sources}")
target_include_directories(cglm PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

