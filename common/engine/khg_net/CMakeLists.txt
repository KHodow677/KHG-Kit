cmake_minimum_required(VERSION 3.16)
project(khg_net)

add_library(khg_net)

file(GLOB_RECURSE sources "src/*.c")
target_sources(khg_net PRIVATE "${sources}")
target_include_directories(khg_net PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(khg_net PUBLIC khg_utl)
