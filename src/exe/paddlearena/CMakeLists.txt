cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

set(TARGETNAME paddlearena)

file(GLOB SOURCES *.cpp)
add_executable(${TARGETNAME} ${SOURCES})

include(${PROJECT_SOURCE_DIR}/cmake/SputterInc.cmake)
