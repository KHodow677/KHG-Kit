cmake_minimum_required(VERSION 3.5)
project(khg_env)

add_library(khg_env)
file(GLOB_RECURSE sources "src/*.c")
target_sources(khg_env PRIVATE "${sources}")
target_include_directories(khg_env PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(khg_env PUBLIC stb_image stb_truetype glad glfw khg_math khg_utils khg_2d)
