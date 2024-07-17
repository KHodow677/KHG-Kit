cmake_minimum_required(VERSION 3.5)
project(postgresql)

add_library(postgresql)
target_sources(postgresql PRIVATE "src/postgresql.c")
target_include_directories(postgresql PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
