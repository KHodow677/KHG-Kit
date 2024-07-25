cmake_minimum_required(VERSION 3.7)

project(khg_phy)

option(BUILD_STATIC "Build as static library" ON)

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  option(FORCE_CLANG_BLOCKS "Force enable Clang blocks" YES)
endif()

if(NOT MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
  if(FORCE_CLANG_BLOCKS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fblocks")
  endif()
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ffast-math")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall")
endif()

file(GLOB khg_phy_source_files "src/*.c")
file(GLOB khg_phy_public_header "${khg_phy_SOURCE_DIR}/include/chipmunk/*.h")
include_directories(${khg_phy_SOURCE_DIR}/include)

if(BUILD_STATIC)
  add_library(khg_phy STATIC ${khg_phy_source_files})
  if(MSVC)
    set_source_files_properties(${khg_phy_source_files} PROPERTIES LANGUAGE CXX)
    set_target_properties(khg_phy PROPERTIES LINKER_LANGUAGE CXX)
  endif(MSVC)
  set_target_properties(khg_phy PROPERTIES OUTPUT_NAME khg_phy)
endif(BUILD_STATIC)
