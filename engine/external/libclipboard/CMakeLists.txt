cmake_minimum_required(VERSION 3.5)
project(libclipboard)

set(LIBCLIPBOARD_VERSION_MAJOR 1 CACHE STRING "libclipboard major version number")
set(LIBCLIPBOARD_VERSION_MINOR 0 CACHE STRING "libclipboard minor version number")
set(LIBCLIPBOARD_VERSION "${LIBCLIPBOARD_VERSION_MAJOR}.${LIBCLIPBOARD_VERSION_MINOR}" CACHE STRING "libclipboard version number")

option(LIBCLIPBOARD_FORCE_WIN32 "Force building the Win32 backend (default:off)" OFF)
option(LIBCLIPBOARD_FORCE_X11 "Force building the X11 backend (default:off)" OFF)
option(LIBCLIPBOARD_FORCE_COCOA "Force building the Cocoa backend(default:off)" OFF)

option(LIBCLIPBOARD_ADD_SOVERSION "Add soname versions to the built library (default:off)" OFF)
option(LIBCLIPBOARD_USE_STDCALL "Use the stdcall calling convention (default:off)" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries instead of static libraries" OFF)
set(LIBCLIPBOARD_BUILD_SHARED ${BUILD_SHARED_LIBS})

if ((WIN32 OR CYGWIN OR LIBCLIPBOARD_FORCE_WIN32) AND NOT (LIBCLIPBOARD_FORCE_X11 OR LIBCLIPBOARD_FORCE_COCOA))
  set(LIBCLIPBOARD_BUILD_WIN32 TRUE)
endif()

if (((UNIX AND NOT APPLE) OR LIBCLIPBOARD_FORCE_X11) AND NOT (LIBCLIPBOARD_FORCE_WIN32 OR LIBCLIPBOARD_FORCE_COCOA))
  set(LIBCLIPBOARD_BUILD_X11 TRUE)
endif()

if ((APPLE OR LIBCLIPBOARD_FORCE_COCOA) AND NOT (LIBCLIPBOARD_FORCE_WIN32 OR LIBCLIPBOARD_FORCE_X11))
  set(LIBCLIPBOARD_BUILD_COCOA TRUE)
endif()

if (NOT (LIBCLIPBOARD_BUILD_WIN32 OR LIBCLIPBOARD_BUILD_X11 OR LIBCLIPBOARD_BUILD_COCOA))
    message(FATAL_ERROR "Invalid build options. Can only specify one backend to be built.")
endif()

if (CMAKE_COMPILER_IS_GNUCC OR LIBCLIPBOARD_BUILD_COCOA)
  set(GCC_COMPILE_FLAGS "-std=c99 -Wall -pedantic -g")

  execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                  OUTPUT_VARIABLE GCC_VERSION)
  if (GCC_VERSION VERSION_GREATER 4.9 OR GCC_VERSION VERSION_EQUAL 4.9)
    set(GCC_COMPILE_FLAGS "${GCC_COMPILE_FLAGS} -fdiagnostics-color=auto")
  endif()
  set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${GCC_COMPILE_FLAGS}")
endif()

if (CMAKE_COMPILER_IS_GNUCXX OR LIBCLIPBOARD_BUILD_COCOA)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic -g")
endif()

if (LIBCLIPBOARD_BUILD_COCOA)
  set_source_files_properties(src/clipboard_cocoa.c PROPERTIES COMPILE_FLAGS "-x objective-c")
  set(LIBCLIPBOARD_PRIVATE_LIBS ${LIBCLIPBOARD_PRIVATE_LIBS} "-framework Cocoa")
elseif(LIBCLIPBOARD_BUILD_X11)
  include(FindPkgConfig REQUIRED)
  pkg_check_modules(X11 xcb REQUIRED)
  find_package(Threads REQUIRED)

  include_directories(${X11_INCLUDE_DIRS})
  link_directories(${X11_LIBRARY_DIRS})
  set(LIBCLIPBOARD_PRIVATE_LIBS ${LIBCLIPBOARD_PRIVATE_LIBS} ${X11_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/include")

set(HEADERS
  include/libclipboard/libclipboard.h
  include/libclipboard/libclipboard-config.h
)

set(SOURCE
  src/clipboard_win32.c
  src/clipboard_x11.c
  src/clipboard_cocoa.c
  src/clipboard_common.c
)

add_library(libclipboard ${HEADERS} ${SOURCE})
target_include_directories(libclipboard PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries(libclipboard LINK_PRIVATE ${LIBCLIPBOARD_PRIVATE_LIBS})

