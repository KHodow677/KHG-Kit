cmake_minimum_required(VERSION 3.5)
project(khgenv)

add_library(khgenv)
target_sources(khgenv PRIVATE 
  "src/errorReport.c"
  "src/input.c"
  "src/other.c"
  "src/gameScripting.c") 
target_include_directories(khgenv PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(khgenv PUBLIC stb_image stb_truetype glad glfw khgmath khgutils khg2d)
