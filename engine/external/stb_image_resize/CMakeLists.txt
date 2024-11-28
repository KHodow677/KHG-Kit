cmake_minimum_required(VERSION 3.16)
project(stb_image_resize)

add_library(stb_image_resize)
target_sources(stb_image_resize PRIVATE "src/stb_image_resize.c")
target_include_directories(stb_image_resize PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
