if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2019" call "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/vcvarsall.bat" x64 || exit /b
if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2017" call "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Auxiliary/Build/vcvarsall.bat" x64 || exit /b
set PATH=%APPVEYOR_BUILD_FOLDER%/openal/bin/Win64;%APPVEYOR_BUILD_FOLDER%\deps\bin;%PATH%

rem Build Corrade
git clone --depth 1 git://github.com/mosra/corrade.git || exit /b
cd corrade || exit /b
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DWITH_INTERCONNECT=OFF ^
    -DUTILITY_USE_ANSI_COLORS=ON ^
    -G Ninja || exit /b
cmake --build . || exit /b
cmake --build . --target install || exit /b
cd .. && cd ..

rem Build
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DCMAKE_PREFIX_PATH="%APPVEYOR_BUILD_FOLDER%/openal" ^
    -DTARGET_GLES=ON ^
    -DTARGET_GLES2=%TARGET_GLES2% ^
    -DTARGET_DESKTOP_GLES=ON ^
    -DWITH_AUDIO=ON ^
    -DWITH_SDL2APPLICATION=OFF ^
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
    -DWITH_DISTANCEFIELDCONVERTER=OFF ^
    -DWITH_FONTCONVERTER=OFF ^
    -DWITH_IMAGECONVERTER=ON ^
    -DWITH_SCENECONVERTER=ON ^
    -DWITH_GL_INFO=ON ^
    -DWITH_AL_INFO=ON ^
    -DBUILD_TESTS=ON ^
    -DBUILD_GL_TESTS=ON ^
    -G Ninja || exit /b
cmake --build . || exit /b

rem Test
set CORRADE_TEST_COLOR=ON
ctest -V -E GLTest || exit /b

rem Test install, after running the tests as for them it shouldn't be needed
cmake --build . --target install || exit /b
