cmake_minimum_required(VERSION 3.16)
project(khg_phy)

add_library(khg_phy)
file(GLOB_RECURSE sources "src/*.c")
target_sources(khg_phy PRIVATE "${sources}")
target_include_directories(khg_phy PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(khg_phy PUBLIC khg_utl)

