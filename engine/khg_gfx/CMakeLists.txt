cmake_minimum_required(VERSION 3.5)
project(khg_gfx)
add_library(khg_gfx)
file(GLOB_RECURSE sources "src/*.c")
target_sources(khg_gfx PRIVATE "${sources}")
target_include_directories(khg_gfx PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(khg_gfx PUBLIC cglm glad glfw stb_image stb_image_resize stb_truetype libclipboard khg_utl)

