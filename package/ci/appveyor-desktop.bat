if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2019" call "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/vcvarsall.bat" %PLATFORM% || exit /b
if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2017" call "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Auxiliary/Build/vcvarsall.bat" %PLATFORM% || exit /b
if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2015" call "C:/Program Files (x86)/Microsoft Visual Studio 14.0/VC/vcvarsall.bat" %PLATFORM% || exit /b
set PATH=%APPVEYOR_BUILD_FOLDER%\deps\bin;%PATH%
if "%PLATFORM%" == "" set PATH=%APPVEYOR_BUILD_FOLDER%/openal/bin/Win32;%PATH%
rem for SOME FREAKING REASON vcvarsall.bat in MSVC2015 sets %PLATFORM% to
rem X64 while I expect it to be x64. With a 32-bit build it fortunately stays
rem empty. Not a problem on 2017 or 2019. So doing a case-insensitive compare
rem here to make things work properly.
if /I "%PLATFORM%" == "x64" set PATH=%APPVEYOR_BUILD_FOLDER%/openal/bin/Win64;%PATH%

rem need to explicitly specify a 64-bit target, otherwise CMake+Ninja can't
rem figure that out -- https://gitlab.kitware.com/cmake/cmake/issues/16259
rem for TestSuite we need to enable exceptions explicitly with /EH as these are
rem currently disabled -- https://github.com/catchorg/Catch2/issues/1113
if "%COMPILER%" == "msvc-clang" set COMPILER_EXTRA=-DCMAKE_CXX_COMPILER="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/Llvm/bin/clang-cl.exe" -DCMAKE_LINKER="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/Llvm/bin/lld-link.exe" -DCMAKE_CXX_FLAGS="-m64 /EHsc"

rem Build Corrade
git clone --depth 1 git://github.com/mosra/corrade.git || exit /b
cd corrade || exit /b
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_BUILD_TYPE=%CONFIGURATION% ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DWITH_INTERCONNECT=OFF ^
    -DUTILITY_USE_ANSI_COLORS=ON ^
    -DBUILD_STATIC=%BUILD_STATIC% ^
    %COMPILER_EXTRA% -G Ninja || exit /b
cmake --build . || exit /b
cmake --build . --target install || exit /b
cd .. && cd ..

rem Build
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_BUILD_TYPE=%CONFIGURATION% ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DCMAKE_PREFIX_PATH="%APPVEYOR_BUILD_FOLDER%/SDL;%APPVEYOR_BUILD_FOLDER%/openal" ^
    -DWITH_AUDIO=ON ^
    -DWITH_SDL2APPLICATION=ON ^
    -DWITH_GLFWAPPLICATION=ON ^
    -DWITH_WINDOWLESSWGLAPPLICATION=ON ^
    -DWITH_WGLCONTEXT=ON ^
    -DWITH_OPENGLTESTER=ON ^
    -DWITH_ANYAUDIOIMPORTER=ON ^
    -DWITH_ANYIMAGECONVERTER=ON ^
    -DWITH_ANYIMAGEIMPORTER=ON ^
    -DWITH_ANYSCENECONVERTER=ON ^
    -DWITH_ANYSCENEIMPORTER=ON ^
    -DWITH_MAGNUMFONT=ON ^
    -DWITH_MAGNUMFONTCONVERTER=ON ^
    -DWITH_OBJIMPORTER=ON ^
    -DWITH_TGAIMAGECONVERTER=ON ^
    -DWITH_TGAIMPORTER=ON ^
    -DWITH_WAVAUDIOIMPORTER=ON ^
    -DWITH_DISTANCEFIELDCONVERTER=ON ^
    -DWITH_FONTCONVERTER=ON ^
    -DWITH_IMAGECONVERTER=ON ^
    -DWITH_SCENECONVERTER=ON ^
    -DWITH_GL_INFO=ON ^
    -DWITH_AL_INFO=ON ^
    -DBUILD_TESTS=ON ^
    -DBUILD_GL_TESTS=ON ^
    -DBUILD_STATIC=%BUILD_STATIC% ^
    -DBUILD_PLUGINS_STATIC=%BUILD_STATIC% ^
    %COMPILER_EXTRA% -G Ninja || exit /b
cmake --build . || exit /b

rem Test
set CORRADE_TEST_COLOR=ON
ctest -V -E GLTest || exit /b

rem Test install, after running the tests as for them it shouldn't be needed
cmake --build . --target install || exit /b
