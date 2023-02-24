cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

set(TARGETNAME sputter_net_tests)

file(GLOB SOURCES *.cpp)
add_executable(${TARGETNAME} ${SOURCES})

include_directories(${GTEST_INCLUDE_DIR})

if(UNIX)
    set(ADDITIONAL_LIBRARIES pthread)
endif()

target_link_libraries(
    ${TARGETNAME}
    ${GTEST_LIBRARY}
    ${GTEST_MAIN_LIBRARY}
    ${ADDITIONAL_LIBRARIES})
include(${PROJECT_SOURCE_DIR}/cmake/SputterInc.cmake)

add_test(NAME ${TARGETNAME} COMMAND ${TARGETNAME})

