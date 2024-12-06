cmake_minimum_required(VERSION 3.16)
project(stb_vorbis)

add_library(stb_vorbis)
target_sources(stb_vorbis PRIVATE "src/stb_vorbis.c")
target_include_directories(stb_vorbis PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_compile_options(stb_vorbis PRIVATE -Wno-stringop-overflow)

