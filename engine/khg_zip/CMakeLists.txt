cmake_minimum_required(VERSION 3.16)
project(khg_zip)

add_library(khg_zip)
file(GLOB_RECURSE sources "src/*.c")
target_sources(khg_zip PRIVATE "${sources}")
target_include_directories(khg_zip PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(khg_zip PUBLIC miniz khg_utl)

