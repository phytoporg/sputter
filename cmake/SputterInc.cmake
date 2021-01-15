cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

include_directories(
    ${PROJECT_SOURCE_DIR}/src/lib
    ${GLEW_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS})
target_link_libraries(${TARGETNAME}
    sputter 
    glfw 
    glog
    png
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES})
