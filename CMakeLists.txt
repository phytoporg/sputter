cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

project(sputter CXX C)

set (MyProject_VERSION_MAJOR 0)
set (MyProject_VERSION_MINOR 0)
set (MyProject_VERSION_PATCH 1)

set (CMAKE_CXX_STANDARD 20)

option(OPTION_ENABLE_LOG_FRAMES "Enable serialized frame logging" OFF)
if (OPTION_ENABLE_LOG_FRAMES)
    add_definitions(-DENABLE_LOG_FRAMES)
endif()

option(OPTION_DEBUG "Enable debug features" OFF)
if (OPTION_DEBUG)
    add_definitions(-DDEBUG)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/external/cmake")

# OpenGL
add_definitions(-DGL_GLEXT_PROTOTYPES=1) # what does this do?
if (POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif(POLICY CMP0072)

find_package(OpenGL 4.3 REQUIRED)
find_package(GLEW REQUIRED FATAL_ERROR)
find_package(GLFW REQUIRED FATAL_ERROR)
find_package(GLM REQUIRED FATAL_ERROR)

find_package(GTest REQUIRED)

enable_testing()

add_subdirectory(src)
