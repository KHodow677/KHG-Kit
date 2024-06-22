cmake_minimum_required(VERSION 3.5)
project(khg_math)

add_library(khg_math)
target_sources(khg_math PRIVATE 
  "src/vec2.c" 
  "src/vec3.c" 
  "src/vec4.c" 
  "src/mat3.c" 
  "src/mat4.c" 
  "src/quat.c" 
  "src/math.c")
target_include_directories(khg_math PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
