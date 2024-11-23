cmake_minimum_required(VERSION 3.5)
project(khg_thd)

add_library(khg_thd)
file(GLOB_RECURSE sources "src/*.c")
target_sources(khg_thd PRIVATE "${sources}")
target_include_directories(khg_thd PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(khg_thd PUBLIC khg_utl)
