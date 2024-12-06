cmake_minimum_required(VERSION 3.16)

project(miniz)
include(GenerateExportHeader)
set(miniz_SOURCE miniz.c)
add_library(${PROJECT_NAME} ${miniz_SOURCE})
generate_export_header(${PROJECT_NAME})

if(NOT BUILD_SHARED_LIBS)
  string(TOUPPER ${PROJECT_NAME} PROJECT_UPPER)
  set_target_properties(${PROJECT_NAME}
      PROPERTIES INTERFACE_COMPILE_DEFINITIONS ${PROJECT_UPPER}_STATIC_DEFINE)
else()
  set_property(TARGET ${PROJECT_NAME} PROPERTY C_VISIBILITY_PRESET hidden)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:include>
)

file(GLOB INSTALL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
list(APPEND INSTALL_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_export.h)

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<C_COMPILER_ID:GNU>:_GNU_SOURCE>)

