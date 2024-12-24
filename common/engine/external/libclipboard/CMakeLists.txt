cmake_minimum_required(VERSION 3.16)
project(libclipboard)

if (CMAKE_COMPILER_IS_GNUCC)
  set(GCC_COMPILE_FLAGS "-std=c99 -Wall -pedantic -g")
  execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
  if (GCC_VERSION VERSION_GREATER 4.9 OR GCC_VERSION VERSION_EQUAL 4.9)
    set(GCC_COMPILE_FLAGS "${GCC_COMPILE_FLAGS} -fdiagnostics-color=auto")
  endif()
  set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${GCC_COMPILE_FLAGS}")
endif()

if (((UNIX AND NOT APPLE) OR LIBCLIPBOARD_FORCE_X11) AND NOT (LIBCLIPBOARD_FORCE_WIN32 OR LIBCLIPBOARD_FORCE_COCOA))
  include(FindPkgConfig REQUIRED)
  pkg_check_modules(X11 xcb REQUIRED)
  find_package(Threads REQUIRED)

  include_directories(${X11_INCLUDE_DIRS})
  link_directories(${X11_LIBRARY_DIRS})
  set(LIBCLIPBOARD_PRIVATE_LIBS ${LIBCLIPBOARD_PRIVATE_LIBS} ${X11_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/include")

set(HEADERS include/libclipboard/libclipboard.h include/libclipboard/libclipboard-config.h)

set(SOURCE src/clipboard_win32.c src/clipboard_x11.c src/clipboard_cocoa.c src/clipboard_common.c)

add_library(libclipboard ${HEADERS} ${SOURCE})
target_include_directories(libclipboard PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries(libclipboard LINK_PRIVATE ${LIBCLIPBOARD_PRIVATE_LIBS})

