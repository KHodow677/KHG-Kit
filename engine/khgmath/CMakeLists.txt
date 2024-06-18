cmake_minimum_required(VERSION 3.5)
project(khgmath)

add_library(khgmath)
target_sources(khgmath PRIVATE 
  "src/vec2.c" 
  "src/vec3.c" 
  "src/vec4.c" 
  "src/mat3.c" 
  "src/mat4.c" 
  "src/quat.c" 
  "src/math.c")
target_include_directories(khgmath PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
