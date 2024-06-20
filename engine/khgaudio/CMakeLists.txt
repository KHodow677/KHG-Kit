cmake_minimum_required(VERSION 3.5)
project(khgaudio)

add_library(khgaudio)
target_sources(khgaudio PRIVATE 
  "src/deviceManagement.c"
  "src/utils.c")
target_include_directories(khgaudio PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(khgaudio PUBLIC dr_libs jar_libs miniaudio)
