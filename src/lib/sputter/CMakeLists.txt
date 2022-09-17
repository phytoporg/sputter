cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

include_directories(
    ${PROJECT_SOURCE_DIR}/src/lib 
    ${GLM_INCLUDE_DIRS}
    ${GLOG_INCLUDE_DIR}
    ${PNG_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR})

message(ERROR ${PNG_INCLUDE_DIR})

file(GLOB SOURCES 
    *.cpp 
    render/*.cpp 
    ui/*.cpp 
    system/*.cpp 
    assets/*.cpp 
    physics/*.cpp
    math/*.cpp
    memory/*.cpp
    game/*.cpp
    core/*.cpp
    input/*.cpp
    containers/*.cpp)
add_library(sputter ${SOURCES})
