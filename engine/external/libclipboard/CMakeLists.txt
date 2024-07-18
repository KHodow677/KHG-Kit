cmake_minimum_required(VERSION 3.5)
project(libclipboard)

add_library(libclipboard)
file(GLOB_RECURSE sources "src/*.c")
target_sources(libclipboard PRIVATE "${sources}")
target_include_directories(libclipboard PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
