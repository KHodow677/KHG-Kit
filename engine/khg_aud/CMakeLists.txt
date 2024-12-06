cmake_minimum_required(VERSION 3.16)
project(khg_aud)

add_library(khg_aud)
file(GLOB_RECURSE sources "src/*.c")
target_sources(khg_aud PRIVATE "${sources}")
target_include_directories(khg_aud PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(khg_aud PUBLIC stb_vorbis dr_libs jar_libs miniaudio khg_utl)
target_compile_options(khg_aud PRIVATE -Wno-stringop-overflow)

