cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

include_directories(${PROJECT_SOURCE_DIR}/src/lib)

file(GLOB SOURCES *.cpp render/*.cpp system/*.cpp)
add_library(sputter ${SOURCES})
