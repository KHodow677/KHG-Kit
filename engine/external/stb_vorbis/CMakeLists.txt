cmake_minimum_required(VERSION 3.5)
project(stb_vorbis)

add_library(stb_vorbis)
target_sources(stb_vorbis PRIVATE "src/stb_vorbis.c")
target_include_directories(stb_vorbis PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
