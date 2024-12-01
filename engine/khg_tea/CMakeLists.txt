cmake_minimum_required(VERSION 3.16)
project(khg_tea)

add_library(khg_tea)
file(GLOB_RECURSE sources "src/*.c")
target_sources(khg_tea PRIVATE "${sources}")
target_include_directories(khg_tea PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(khg_tea PUBLIC khg_utl)
