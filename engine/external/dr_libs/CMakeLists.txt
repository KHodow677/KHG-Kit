cmake_minimum_required(VERSION 3.16)
project(dr_libs)

add_library(dr_libs)
target_sources(dr_libs PRIVATE "src/dr_libs.c")
target_include_directories(dr_libs PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

