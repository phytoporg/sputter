cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

file(GLOB SOURCES *.c)
add_library(kcp ${SOURCES})
