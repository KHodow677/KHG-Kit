cmake_minimum_required(VERSION 3.5)
project(khg2d)

add_library(khg2d)
target_sources(khg2d PRIVATE 
  "src/camera.c" 
  "src/font.c" 
  "src/framebuffer.c" 
  "src/particles.c" 
  "src/renderer2d.c" 
  "src/shader.c" 
  "src/texture.c" 
  "src/textureAtlas.c" 
  "src/utils.c")
target_include_directories(khg2d PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Link the external library to khg2d
target_link_libraries(khg2d PUBLIC khgutils khgmath stb_image stb_truetype glad)
