cmake_minimum_required(VERSION 3.5)
project(khgutils)

add_library(khgutils)
target_sources(khgutils PRIVATE "src/khgutils.c")
target_include_directories(khgutils PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
