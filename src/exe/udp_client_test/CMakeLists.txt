# TODO Find out why this isn't building the target
cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

set(TARGETNAME udp_client_test)

file(GLOB SOURCES *.cpp)
add_executable(${TARGETNAME} ${SOURCES})

include(${PROJECT_SOURCE_DIR}/cmake/SputterInc.cmake)
