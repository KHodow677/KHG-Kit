cmake_minimum_required(VERSION 3.5)
project(khg_ani)

add_library(khg_ani)

file(GLOB_RECURSE sources "src/*.c")
target_sources(khg_ani PRIVATE "${sources}")
target_include_directories(khg_ani PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(khg_ani PUBLIC khg_utl)

