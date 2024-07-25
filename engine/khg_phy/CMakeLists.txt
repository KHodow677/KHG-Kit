cmake_minimum_required(VERSION 3.5)

project(khg_phy)

option(BUILD_STATIC "Build as static library" ON)

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  option(FORCE_CLANG_BLOCKS "Force enable Clang blocks" YES)
endif()

if(NOT MSVC)
  add_compile_options(-Wno-nan-infinity-disabled)
  add_compile_options(-std=gnu99 -ffast-math -Wall)

  if(CMAKE_C_COMPILER_ID STREQUAL "Clang" AND FORCE_CLANG_BLOCKS)
    add_compile_options(-fblocks)
  endif()
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