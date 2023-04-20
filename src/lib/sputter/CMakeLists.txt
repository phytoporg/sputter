cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

include_directories(
    ${PROJECT_SOURCE_DIR}/src/lib 
    ${GLM_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR})

file(GLOB SOURCES 
    *.cpp 
    assets/*.cpp 
    containers/*.cpp
    core/*.cpp
    game/*.cpp
    input/*.cpp
    log/*.cpp
    math/*.cpp
    memory/*.cpp
    net/*.cpp
    physics/*.cpp
    render/*.cpp 
    system/*.cpp 
    ui/*.cpp)
add_library(sputter ${SOURCES})
