cmake_minimum_required(VERSION 3.16)
project(khg_ecs)

add_library(khg_ecs)
file(GLOB_RECURSE sources "src/*.c")
target_sources(khg_ecs PRIVATE "${sources}")
target_include_directories(khg_ecs PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
