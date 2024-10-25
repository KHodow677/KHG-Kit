cmake_minimum_required(VERSION 3.5)
project(khg_ann)

add_library(khg_ann)

file(GLOB_RECURSE sources "src/*.c")
target_sources(khg_ann PRIVATE "${sources}")
target_include_directories(khg_ann PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(khg_ann PUBLIC khg_utl)

