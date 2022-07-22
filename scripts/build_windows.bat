setlocal

REM TODO: set up a more robust system than this. For now, it'll do!

REM GLEW bits retrieved from the releases @ https://github.com/nigels-com/glew
REM Building from source in Windows isn't recommended without MSYS32 or a
REM similar environment, which I personally find pretty cumbersome.
SET GLEW_ROOT=C:\libs\glew-2.2.0
set GLEW_INCLUDE_DIR=%GLEW_ROOT%\include
set GLEW_STATIC_LIBRARY=%GLEW_ROOT%\lib\Release\x64\glew32s.lib
set GLEW_VERBOSE=1

REM TODO: GLEW dependencies next. This script doesn't succesfully configure the build as-is.

rmdir /s /q build
mkdir build

cmake -DGLEW_USE_STATIC_LIBS=1 -DGLEW_VERBOSE=%GLEW_VERBOSE% -DGLEW_INCLUDE_DIR=%GLEW_INCLUDE_DIR% -DGLEW_STATIC_LIBRARY_RELEASE=%GLEW_STATIC_LIBRARY% -B ./build -S .
