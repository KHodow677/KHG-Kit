cmake_minimum_required(VERSION 3.7)

project(khg_phy)

# to change the prefix, run cmake with the parameter:
#   -D CMAKE_INSTALL_PREFIX=/my/prefix

# to change the build type, run cmake with the parameter:
#   -D CMAKE_BUILD_TYPE=<build-type>
# run "cmake --help-variable CMAKE_BUILD_TYPE" for details
if(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif()

option(BUILD_STATIC "Build as static library" ON)

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  option(FORCE_CLANG_BLOCKS "Force enable Clang blocks" YES)
endif()

if(NOT MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99") # always use gnu99
  if(FORCE_CLANG_BLOCKS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fblocks")
  endif()
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ffast-math") # extend release-profile with fast-math
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall") # extend debug-profile with -Wall
endif()

add_subdirectory(src)

#file(GLOB_RECURSE sources "src/*.c")
#target_sources(khg_phy PRIVATE "${sources}")
#target_include_directories(khg_gfx PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
#target_link_libraries(khg_phy PUBLIC cglm glad glfw stb_image stb_image_resize stb_truetype libclipboard khg_utl)
file(GLOB chipmunk_source_files "src/*.c")
file(GLOB chipmunk_public_header "${chipmunk_SOURCE_DIR}/include/chipmunk/*.h")
include_directories(${chipmunk_SOURCE_DIR}/include)

# Chipmunk2D 7.0.3
set(CHIPMUNK_VERSION_MAJOR 7)
set(CHIPMUNK_VERSION_MINOR 0)
set(CHIPMUNK_VERSION_PATCH 3)
set(CHIPMUNK_VERSION "${CHIPMUNK_VERSION_MAJOR}.${CHIPMUNK_VERSION_MINOR}.${CHIPMUNK_VERSION_PATCH}")
message("Configuring Chipmunk2D version ${CHIPMUNK_VERSION}")

if(BUILD_STATIC)
  add_library(khg_phy STATIC
    ${chipmunk_source_files}
  )
  # Tell MSVC to compile the code as C++.
  if(MSVC)
    set_source_files_properties(${chipmunk_source_files} PROPERTIES LANGUAGE CXX)
    set_target_properties(chipmunk_static PROPERTIES LINKER_LANGUAGE CXX)
  endif(MSVC)
  # Sets chipmunk_static to output "libchipmunk.a" not "libchipmunk_static.a"
  set_target_properties(khg_phy PROPERTIES OUTPUT_NAME khg_phy)
  if(INSTALL_STATIC)
    install(TARGETS khg_phy ARCHIVE DESTINATION ${LIB_INSTALL_DIR})
  endif(INSTALL_STATIC)
endif(BUILD_STATIC)


