cmake_minimum_required(VERSION 3.5)
project(khgaudio)

add_library(khgaudio)
file(GLOB_RECURSE sources "src/*.c")
target_sources(khgaudio PRIVATE "${sources}")
target_include_directories(khgaudio PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(khgaudio PUBLIC stb_vorbis dr_libs jar_libs miniaudio khgutils)
