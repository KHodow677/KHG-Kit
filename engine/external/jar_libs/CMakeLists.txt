cmake_minimum_required(VERSION 3.5)
project(jar_libs)

add_library(jar_libs)
target_sources(jar_libs PRIVATE "src/jar_libs.c")
target_include_directories(jar_libs PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
