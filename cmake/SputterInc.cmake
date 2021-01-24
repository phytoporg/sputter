cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

include_directories(
    ${PROJECT_SOURCE_DIR}/src/lib
    ${GLEW_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIR})
target_link_libraries(${TARGETNAME}
    sputter 
    glfw 
    glog
    png
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    stdc++fs) # LINUX -- appears to be necessary for now, see
              # https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90050
