cmake_minimum_required(VERSION 3.16)
project(khg_khs)

add_library(khg_khs)
file(GLOB_RECURSE sources "src/*.c")
target_sources(khg_khs PRIVATE "${sources}")
target_include_directories(khg_khs PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(khg_khs PUBLIC khg_utl)
