cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

include_directories(
    ${PROJECT_SOURCE_DIR}/src/lib
    ${GLFW_INCLUDE_DIR}
    ${PNG_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIR})

if(LINUX)
    # LINUX -- appears to be necessary for now, see
    # https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90050
    set(ADDITIONAL_LIBRARIES stdc++fs)
endif()

target_link_libraries(${TARGETNAME}
    sputter
    kcp
    ${GLFW_LIBRARY} 
    ${PNG_LIBRARY}
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${ADDITIONAL_LIBRARIES}) 

# Copy requisite DLLs
if (WIN32)
    add_custom_command(TARGET ${TARGETNAME} POST_BUILD # Adds a post-build event to MyTest
        COMMAND ${CMAKE_COMMAND} -E copy_if_different   # which executes "cmake - E copy_if_different..."
        "${GLEW_ROOT}/bin/Release/x64/glew32.dll"   $<TARGET_FILE_DIR:${TARGETNAME}>
        "${GLFW_ROOT}/lib-vc2019/glfw3.dll"         $<TARGET_FILE_DIR:${TARGETNAME}>)
endif()
