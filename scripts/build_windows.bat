setlocal
@echo off

REM TODO: set up a more robust system than this. For now, it'll do!

set LIBS_ROOT=C:\libs

REM GLEW bits retrieved from the releases @ https://github.com/nigels-com/glew
REM Building from source in Windows isn't recommended without MSYS32 or a
REM similar environment, which I personally find pretty cumbersome.
set MY_GLEW_ROOT=%LIBS_ROOT%\glew-2.1.0
set GLEW_INCLUDE_DIR=%MY_GLEW_ROOT%\include
set GLEW_STATIC_LIBRARY=%MY_GLEW_ROOT%\lib\Release\x64\glew32.lib
set GLEW_VERBOSE=0

set MY_GLFW_ROOT=%LIBS_ROOT%\glfw-3.3.8
set GLFW_INCLUDE_DIR=%MY_GLFW_ROOT%\include
set GLFW_LIBRARY=%MY_GLFW_ROOT%\lib-vc2019\glfw3.lib

set MY_GLM_ROOT=%LIBS_ROOT%\glm-0.9.9.8
set GLM_INCLUDE_DIR=%MY_GLM_ROOT%

set MY_GTEST_ROOT=%LIBS_ROOT%\gtest-1.12.1
set GTEST_INCLUDE_DIR=%MY_GTEST_ROOT%\include
set GTEST_LIBRARY=%MY_GTEST_ROOT%\lib\gtest.lib

set MY_ZLIB_ROOT=%LIBS_ROOT%\zlib-1.2.12
set ZLIB_INCLUDE_DIR=%MY_ZLIB_ROOT%\include
set ZLIB_LIBRARY=%MY_ZLIB_ROOT%\lib\Release\libz-static.lib

set MY_PNG_ROOT=%LIBS_ROOT%\libpng-1.6.35
set PNG_INCLUDE_DIR=%MY_PNG_ROOT%\include
set PNG_LIBRARY=%MY_PNG_ROOT%\lib\libpng16_staticd.lib

rmdir /s /q build
mkdir build

cmake -DGLEW_ROOT=%MY_GLEW_ROOT% -DGLEW_USE_STATIC_LIBS=1 -DGLEW_VERBOSE=%GLEW_VERBOSE% -DGLEW_INCLUDE_DIR=%GLEW_INCLUDE_DIR% -DGLEW_STATIC_LIBRARY_RELEASE=%GLEW_STATIC_LIBRARY% -DGLFW_ROOT=%MY_GLFW_ROOT% -DGLFW_INCLUDE_DIR=%GLFW_INCLUDE_DIR% -DGLFW_LIBRARY=%GLFW_LIBRARY% -DGLM_INCLUDE_DIR=%GLM_INCLUDE_DIR% -DGTEST_ROOT=%MY_GTEST_ROOT% -DGTEST_INCLUDE_DIR=%GTEST_INCLUDE_DIR% -DGTEST_LIBRARY=%GTEST_LIBRARY% -DZLIB_INCLUDE_DIR=%ZLIB_INCLUDE_DIR% -DZLIB_LIBRARY=%ZLIB_LIBRARY% -DPNG_INCLUDE_DIR=%PNG_INCLUDE_DIR% -DPNG_LIBRARY=%PNG_LIBRARY% -B ./build -S .

cmake --build build -j8
