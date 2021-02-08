cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

include_directories(
    ${PROJECT_SOURCE_DIR}/src/lib 
    ${GLM_INCLUDE_DIRS})

file(GLOB SOURCES *.cpp render/*.cpp system/*.cpp assets/*.cpp physics/*.cpp)
add_library(sputter ${SOURCES})
