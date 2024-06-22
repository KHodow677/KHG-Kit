cmake_minimum_required(VERSION 3.16)

option(PRODUCTION_BUILD "Make this a production build" ON)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE) #link time optimization

project(executable)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(engine/external/glfw)
add_subdirectory(engine/external/glad)
add_subdirectory(engine/external/dr_libs)
add_subdirectory(engine/external/jar_libs)
add_subdirectory(engine/external/miniaudio)
add_subdirectory(engine/external/stb_image)
add_subdirectory(engine/external/stb_truetype)
add_subdirectory(engine/external/stb_vorbis)
add_subdirectory(engine/khg_audio)
add_subdirectory(engine/khg_math)
add_subdirectory(engine/khg_utils)
add_subdirectory(engine/khg_2d)
add_subdirectory(engine/khg_ui)
add_subdirectory(engine/khg_env)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

add_executable("${CMAKE_PROJECT_NAME}")

set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY C_STANDARD 99)

target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC GLFW_INCLUDE_NONE=1) 

if(PRODUCTION_BUILD)
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="./res/") 
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=1) 
else()
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/")
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=0) 
endif()

target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${MY_SOURCES} )

if(MSVC)
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC _CRT_SECURE_NO_WARNINGS)
	set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
else()
target_compile_options("${CMAKE_PROJECT_NAME}" PRIVATE -fsanitize=address -fno-omit-frame-pointer)
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE -fsanitize=address)
endif()


target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/gameLayer/")
target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/platform/")

target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE glfw glad stb_image stb_truetype khg_audio khg_math khg_utils khg_2d khg_ui khg_env)
